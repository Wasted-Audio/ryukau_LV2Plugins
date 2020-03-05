# Code Walkthrough
This is mostly a note to remember what I was doing.

## Adding a Parameter
Parameter management is structured to avoid changing 5 different switch statement in `ui.cpp` and `plugin.cpp` for each parameter addition.

### Parameter
Parameters are written in `parameter.hpp` and `parameter.cpp`.

To add a parameter, open `parameter.hpp` then add entry to enum `ParameterID::ID`.

```cpp
namespace ParameterID {
enum ID {
  newParam, // Adding a new parameter for example.

  ID_ENUM_LENGTH,
};
} // namespace ParameterID
```

Next, add value scaling to `Scales`.

```cpp
struct Scales {
  static SomeDSP::LogScale<double> newScale;
};
```

Add instance of `Scales::newScale` at `parameter.cpp`.

```cpp
LogScale<double> Scales::newScale(0.0, 10.0, 0.5, 1.0);
```

Arguments for `LogScale` are `(minRawValue, maxRawValue, normalizedValueAtAnchorPoint, rawValueAtAnchorPoint)`. Normalized value is used for UI. It's range is always `[0.0, 1.0]`. Last 2 arguments of `LogScale` changes tuning of parameter. If `newScale` is represented as a knob in UI, then setting the knob to the middle sends a normalized value 0.5 to UI, then it is converted to a raw value 1.0 in DSP code.

Finally, add parameter to `GlobalParameter.value` in `parameter.hpp`.

```cpp
struct GlobalParameter {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    using LogValue = FloatValue<SomeDSP::LogScale<double>>;

    // Adding a new parameter for example.
    value[ID::newParam] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "NewParameterName", kParameterIsAutomable);
  }
  // ...
};
```

`value` is a std::vector. Index is corresponded to `ParameterID::ID`.

There's 2 types of value. `IntValue` and `FloatValue`. They are defined in `value.hpp`. `FloatValue` takes a value scaling for template argument. See `dsp/scales.hpp` for available scales.

Arguments for `make_unique` are `(normalizedValue, scale, parameterName, parameterHint)`. It's calling constructor of `FloatValue` in `value.hpp`. Range of `normalizedValue` is `[0.0, 1.0]`. In DPF, `parameterName` string must only consists of `a-zA-z0-9` and `_` (underscore). Space and other character may crash plugin with segmentation fault.

### UI
UI code is in `ui.cpp`. `ui.hpp` only provides interface for DPF, so it can be ignored. There are also widget codes in `gui` directory.

GUI layout is defined in constructor of `PluginUI` in `ui.cpp`.

Example to add `newParam` as a knob.

```cpp
class TrapezoidSynthUI : public PluginUI {
public:
  TrapezoidSynthUI() : PluginUI(defaultWidth, defaultHeight)
  {
    // ...

    using ID = ParameterID::ID;

    // Adding a new parameter as a knob for example.
    addKnob(10.0f, 10.0f, 60.0f, colorBlue, "newParameter", ID::newParam);
  }

  // ...

  void addKnob(
    float left,
    float top,
    float width,
    Color highlightColor,
    const char *name,
    uint32_t id,
    LabelPosition labelPosition = LabelPosition::bottom)
  {
    // ...
  }
};
```

### DSP
DSP code are written in `plugin.cpp`, `dsp/dspcore.hpp` and `dsp/dspcore.cpp`. Other codes in `dsp` directory are dsp components.

To recieve a raw value from parameter, open `dsp/dspcore.hpp` and add smoother to `DSPCore` class.

```cpp
class DSPCore {
  // ...
private:
  LinearSmoother<float> interpNewParam;
};
```

Next, open `dsp/dspcore.cpp` and go to `DSPCore::setParameters()`.

```cpp
void DSPCore::setParameters()
{
  interpNewParam.push(param.value[ParameterID::newParam]->getFloat());
}
```

If the parameter is defined as `IntValue`, use `getInt()` instead of `getFloat()`.

Finally, `newParam` is ready to use in DSP code at `DSPCore::process()`.

```cpp
void DSPCore::process(const size_t length, float *out0, float *out1)
{
  LinearSmoother<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) { // Processing loop.
    const float paramValue = interpNewParam.process();

    // Outputs interpolated newParam value as signal.
    out0[i] = paramValue;
    out1[i] = paramValue;
  }
}
```

Keep in mind that changing parameter value for every sample is expensive for CPU. Sometimes  it's better to set parameters at outside of processing loop. Usual places are `DSPCore::setParameters()` and `DSPCore::noteOn()`. When parameter is only changed at outside, `LinearSmoother` is unnecessary.

## Note Handling
In DPF, programmer have to deal with raw MIDI events to get note information. MIDI handling is done in `Plugin::handleMidi()` method in `plugin.cpp`.

`handleMidi()` is called for each processing loop, which is `Plugin::run()` method.

### Discarding Duplicate Note-On
Some MIDI keyboard sends duplicate MIDI note-on for each key press. Those duplicates are discarded.

First note-on received will be stored in `alreadyRecievedNote`. If following note-on have same key number in any of `alreadyRecievedNote`, it will be discarded.

### Note ID
MIDI note-on/off events only provides key number and velocity. This causes problem when receiving note-on event with same key number. For example, think about receiveing event in following order:

1. Note-on (key=60, velocity=100)
2. Note-on (key=60, velocity=100)
3. Note-off (key=60, velocity=100)
4. Note-off (key=60, velocity=100)

A problem is that we can't know which note-on corresponds to which note-off.

In VST 3, plugin receives note ID instead of MIDI note-on/off event. Above example becomes like this:

1. Note-on (id=0, key=60, velocity=100)
2. Note-on (id=1, key=60, velocity=100)
3. Note-off (id=0, key=60, velocity=100)
4. Note-off (id=1, key=60, velocity=100)

Now we know which note to stop when receiving note-off event.

Plugins in this repository imitates this note ID idea, but with an assumption that first note-off always corresponds to first note-on with same key number.

For implementation detail, a std::vector `lastNoteId` holds (id, key) pair. `std::find_if` is used to search.

- When a note-on comes in, a new note ID is assigned and pushed into `lastNoteId`.
- When a note-off comes in, search `lastNoteId` and call `noteOff` method of correspond note. Then pop note ID corresponds to note-off key number.

Reference:

- [Summary of MIDI Messages](https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message)


### Note-On Timing
DPF provides offset of MIDI events. Offset is number of samples from start of processing call.

Plugins in this repository only handles note-on/off offsets.

Offset is passed to `Plugin::run()` to `DSPCore::pushMidiNote()`. Received notes are stored in `DSPCore::midiNotes`, then picked up in `DSPCore::processMidiNote()`, which is called in`DSPCore::process()`.

## CPU Dispatching
In this repository, [vector class library](https://github.com/vectorclass/version2) (VCL) is used to write SIMD instructions.

Basics of CPU dispatching is explained in [VCL manual](https://github.com/vectorclass/manual) - 9.9 Instruction sets and CPU dispatching. VCL is also providing [example code](https://github.com/vectorclass/version2/blob/master/dispatch_example.cpp) for CPU dispatching.

VCL example uses function pointer for CPU dispatching. In this repository macro, class and `std::unique_ptr` is used.

### Building
Before going to read here, it's better to take a look at [CPU dispatching example code](https://github.com/vectorclass/version2/blob/master/dispatch_example.cpp) provided by VCL.

To enable different set of SIMD instructions, we must compile source code with different options. For example, it could be something like code below. Details of `dspcore.cpp` is described later.

```bash
# -fPIC is required for shared object.
g++ -O3 -fPIC -mavx512f -mfma -mavx512vl -mavx512bw -mavx512dq -std=c++17 -c \
  dspcore.cpp -o dspcore.avx512.o
g++ -O3 -fPIC -mavx2 -mfma -std=c++17 -c \
  dspcore.cpp -o dspcore.avx2.o
g++ -O3 -fPIC -msse4.1 -std=c++17 -c \
  dspcore.cpp -o dspcore.sse41.o
g++ -O3 -fPIC -msse2 -std=c++17 -c \
  dspcore.cpp -o dspcore.sse2.o

# Changing order of object file (*.o) may crash application with illegal instruction.
# https://stackoverflow.com/questions/15406658/cpu-dispatcher-for-visual-studio-for-avx-and-sse
g++ -std=c++17 -O3 -Wall
  ../../lib/vcl/instrset_detect.cpp \
  ./dspcore.sse2.o                     \
  ./dspcore.sse41.o                    \
  ./dspcore.avx2.o                     \
  ./dspcore.avx512.o                   \
  main.cpp
```

In general case, source code for each instruction set may be different. With VCL, a single source code can be compiled into different instruction set.

Above bash script was translated to GNU make. For example, take a look at `IterativeSinCluster/Makefile`.

### Prepareing Class
In order to dispatching for appropriate function/method/class, there must be different identifier for each instruction set. And interface of each function/method/class must be identical.

In `dsp/dspcore.hpp`, `DSPInterface` is defined. And `DSPCore_SSE2`, `DSPCore_SSE41`, `DSPCore_AVX2`, `DSPCore_AVX512` are derived from `DSPInterface`.

Implementation of `DSPCore_*` is identical to each other (thanks to VCL), so macro is used to reduce duplication.

```cpp
class DSPInterface {
public:
  void process(/* ... */);
  // ...
};

#define DSPCORE_CLASS(INSTRSET)                          \
  class DSPCore_##INSTRSET final : public DSPInterface { \
  public:                                                \
    void process(/* ... */);                             \
    /* ... */                                            \
  };

DSPCORE_CLASS(AVX512)
DSPCORE_CLASS(AVX2)
DSPCORE_CLASS(SSE41)
DSPCORE_CLASS(SSE2)
```

Inside macro, `//` can't be used for comment. `##` is used to concatenate argument to a word in macro text.

Macro is also used in `dspcore.cpp` to separate code for different compilation.

```cpp
#include "dspcore.hpp"
#include "../../lib/vcl/vectorclass.h"

// INSTRSET is provided by VCL.
#if INSTRSET >= 10
#define DSPCORE_NAME DSPCore_AVX512
#elif INSTRSET >= 8
#define DSPCORE_NAME DSPCore_AVX2
#elif INSTRSET >= 5
#define DSPCORE_NAME DSPCore_SSE41
#elif INSTRSET == 2
#define DSPCORE_NAME DSPCore_SSE2
#else
#error Unsupported instruction set
#endif

void DSPCORE_NAME::process(/* ... */) {
  // ...
}
```

Reference:

- [c++ - Can I comment multi-line macros? - Stack Overflow](https://stackoverflow.com/questions/24751387/can-i-comment-multi-line-macros)
- [c - What are the applications of the ## preprocessor operator and gotchas to consider? - Stack Overflow](https://stackoverflow.com/questions/216875/what-are-the-applications-of-the-preprocessor-operator-and-gotchas-to-conside)

### Branching
Branching for different CPU instruction is done in `Plugin` constructor written in `plugin.cpp`.

```cpp
class SomePlugin : public Plugin {
public:
  SomePlugin()
    : Plugin(ParameterID::ID_ENUM_LENGTH, GlobalParameter::Preset::Preset_ENUM_LENGTH, 0)
  {
    auto iset = instrset_detect(); // instrset_detect() is provided by VCL.
    if (iset >= 10) {
      dsp = std::make_unique<DSPCore_AVX512>();
    } else if (iset >= 8) {
      dsp = std::make_unique<DSPCore_AVX2>();
    } else if (iset >= 5) {
      dsp = std::make_unique<DSPCore_SSE41>();
    } else if (iset >= 2) {
      dsp = std::make_unique<DSPCore_SSE2>();
    } else {
      std::cerr << "\nError: Instruction set SSE2 not supported on this computer";
      exit(EXIT_FAILURE);
    }
    // ...
  }
  // ...

private:
  std::unique_ptr<DSPInterface> dsp;
  // ...
};
```

Lifetime of `dsp` must be same with entire lifetime of `Plugin` class. This is because I'm not sure about how DAW access `Plugin`. It's possible that different threads access different method of `Plugin` at same time. If that happens, changing instance pointed by `dsp` causes segmentation fault.

For example, think about this implementation:

```cpp
void SomePlugin::sampleRateChanged(double newSampleRate) {
  dsp = std::make_unique<DSPCore>(); // This is bad.
  dsp->setup(newSampleRate);
}

void SomePlugin::run(/* ... */) {
  // ...
  dsp->process(frames, outputs[0], outputs[1]);
}
```

Then, thread A is calling `Plugin::run()`, and thread B calls `Plugin::sampleRateChanged()`. It's possible that `dsp` in thread A will points destructed address.
