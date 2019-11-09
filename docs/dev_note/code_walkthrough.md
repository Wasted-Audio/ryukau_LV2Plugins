# Code Walkthrough
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

Add initialization of `Scales::newScale` at `parameter.cpp`.

```cpp
LogScale<double> Scales::newScale(0.0, 10.0, 0.5, 1.0);
```

Arguments for `LogScale` are `(minValue, maxValue, normalizedValueAnchorPoint, scaledValueCorrespondToAnchorPoint)`. Normalized value is used for UI. It's range is always `[0.0, 1.0]`. Last 2 arguments of `LogScale` changes tuning of parameter. If `newScale` is represented as a knob in UI, then setting the knob to the middle sends a normalized value 0.5 to UI, then it is converted to a raw value 1.0 in DSP code.

Finally, add initialization to `GlobalParameter.value` in `parameter.hpp`.

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

`value` is a vector. Index is correspond to `ParameterID::ID`.

There's 2 types of value. `IntValue` and `FloatValue`. They are defined in `value.hpp`. `FloatValue` takes a value scaling for template argument. See `dsp/scales.hpp` for available scales.

Arguments for `make_unique` are `(normalizedValue, scale, parameterName, parameterHint)`. Range of `normalizedValue` is `[0.0, 1.0]`. In DPF, `parameterName` string must only consists of `a-zA-z0-9` and `_` (underscore). Space and other character may crash plugin with segmentation fault.

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

Keep in mind that changing parameter value for every sample is expensive for CPU. Sometimes  it's better to set parameters at outside of processing loop. Usual places for outside are `DSPCore::setParameters()` and `DSPCore::noteOn()`. When parameter is only changed at outside, `LinearSmoother` is unnecessary.
