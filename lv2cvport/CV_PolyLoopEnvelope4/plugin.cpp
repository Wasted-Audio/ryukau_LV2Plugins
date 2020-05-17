// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of CV_PolyLoopEnvelope4.
//
// CV_PolyLoopEnvelope4 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_PolyLoopEnvelope4 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_PolyLoopEnvelope4.  If not, see <https://www.gnu.org/licenses/>.

#include <utility>

#include "DistrhoPlugin.hpp"
#include "dsp/dspcore.hpp"

START_NAMESPACE_DISTRHO

class CV_PolyLoopEnvelope4 : public Plugin {
public:
  // Plugin(nParameters, nPrograms, nStates).
  CV_PolyLoopEnvelope4() : Plugin(ParameterID::ID_ENUM_LENGTH, 0, 0)
  {
    sampleRateChanged(getSampleRate());
    lastNoteId.reserve(dsp.maxVoice + 1);
    alreadyRecievedNote.reserve(dsp.maxVoice);
  }

protected:
  /* Information */
  const char *getLabel() const override { return "CV_PolyLoopEnvelope4"; }
  const char *getDescription() const override
  {
    return "Polynomial curve envelope with 4 sections for loop.";
  }
  const char *getMaker() const override { return "Uhhyou"; }
  const char *getHomePage() const override
  {
    return "https://github.com/ryukau/LV2Plugins";
  }
  const char *getLicense() const override { return "GPLv3"; }
  uint32_t getVersion() const override
  {
    return d_version(MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
  }
  int64_t getUniqueId() const override { return d_cconst('u', '0', '0', '0'); }

  void initAudioPort(bool input, uint32_t index, AudioPort &port)
  {
    // See also: enum Input in `dsp/dspcore.cpp`.

    if (input && index == Input::inGate) {
      port.hints = kAudioPortIsCV;
      port.name = String("Trig/Gate");
      port.symbol = String("cv_gate");
    } else if (input && index == Input::inGain) {
      port.hints = kAudioPortIsCV;
      port.name = String("Gain");
      port.symbol = String("cv_gain");
    } else if (input && index == Input::inRate) {
      port.hints = kAudioPortIsCV;
      port.name = String("Rate");
      port.symbol = String("cv_rate");
    }

    else if (input && index == Input::inReleaseTime) {
      port.hints = kAudioPortIsCV;
      port.name = String("ReleaseTime");
      port.symbol = String("cv_release_time");
    } else if (input && index == Input::inReleaseCurve) {
      port.hints = kAudioPortIsCV;
      port.name = String("ReleaseCurve");
      port.symbol = String("cv_release_curve");
    }

    else if (input && index == Input::inDecay0) {
      port.hints = kAudioPortIsCV;
      port.name = String("Decay0");
      port.symbol = String("cv_decay0");
    } else if (input && index == Input::inDecay1) {
      port.hints = kAudioPortIsCV;
      port.name = String("Decay1");
      port.symbol = String("cv_decay1");
    } else if (input && index == Input::inDecay2) {
      port.hints = kAudioPortIsCV;
      port.name = String("Decay2");
      port.symbol = String("cv_decay2");
    } else if (input && index == Input::inDecay3) {
      port.hints = kAudioPortIsCV;
      port.name = String("Decay3");
      port.symbol = String("cv_decay3");
    }

    else if (input && index == Input::inHold0) {
      port.hints = kAudioPortIsCV;
      port.name = String("Hold0");
      port.symbol = String("cv_hold0");
    } else if (input && index == Input::inHold1) {
      port.hints = kAudioPortIsCV;
      port.name = String("Hold1");
      port.symbol = String("cv_hold1");
    } else if (input && index == Input::inHold2) {
      port.hints = kAudioPortIsCV;
      port.name = String("Hold2");
      port.symbol = String("cv_hold2");
    } else if (input && index == Input::inHold3) {
      port.hints = kAudioPortIsCV;
      port.name = String("Hold3");
      port.symbol = String("cv_hold3");
    }

    else if (input && index == Input::inLevel0) {
      port.hints = kAudioPortIsCV;
      port.name = String("Level0");
      port.symbol = String("cv_level0");
    } else if (input && index == Input::inLevel1) {
      port.hints = kAudioPortIsCV;
      port.name = String("Level1");
      port.symbol = String("cv_level1");
    } else if (input && index == Input::inLevel2) {
      port.hints = kAudioPortIsCV;
      port.name = String("Level2");
      port.symbol = String("cv_level2");
    } else if (input && index == Input::inLevel3) {
      port.hints = kAudioPortIsCV;
      port.name = String("Level3");
      port.symbol = String("cv_level3");
    }

    else if (input && index == Input::inCurve0) {
      port.hints = kAudioPortIsCV;
      port.name = String("Curve0");
      port.symbol = String("cv_curve0");
    } else if (input && index == Input::inCurve1) {
      port.hints = kAudioPortIsCV;
      port.name = String("Curve1");
      port.symbol = String("cv_curve1");
    } else if (input && index == Input::inCurve2) {
      port.hints = kAudioPortIsCV;
      port.name = String("Curve2");
      port.symbol = String("cv_curve2");
    } else if (input && index == Input::inCurve3) {
      port.hints = kAudioPortIsCV;
      port.name = String("Curve3");
      port.symbol = String("cv_curve3");
    }

    else if (!input && index == 0) {
      port.hints = kAudioPortIsCV;
      port.name = String("Output");
      port.symbol = String("cv_out");
    } else {
      Plugin::initAudioPort(input, index, port);
    }
  }

  void initParameter(uint32_t index, Parameter &parameter) override
  {
    dsp.param.initParameter(index, parameter);
    parameter.symbol = parameter.name;
  }

  float getParameterValue(uint32_t index) const override
  {
    return dsp.param.getParameterValue(index);
  }

  void setParameterValue(uint32_t index, float value) override
  {
    dsp.param.setParameterValue(index, value);
  }

  void initProgramName(uint32_t index, String &programName) override
  {
    dsp.param.initProgramName(index, programName);
  }

  void loadProgram(uint32_t index) override { dsp.param.loadProgram(index); }

  void sampleRateChanged(double newSampleRate) { dsp.setup(newSampleRate); }
  void activate() {}
  void deactivate() { dsp.reset(); }

  void handleMidi(const MidiEvent ev)
  {
    if (ev.size != 3) return;

    switch (ev.data[0] & 0xf0) {
      // Note off.
      case 0x80: {
        auto it = std::find_if(
          lastNoteId.begin(), lastNoteId.end(),
          [&](const std::pair<uint8_t, uint32_t> &p) { return p.first == ev.data[1]; });
        if (it == std::end(lastNoteId)) break;
        dsp.pushMidiNote(false, ev.frame, it->second, 0, 0, 0);
        lastNoteId.erase(it);
      } break;

      // Note on. data[1]: note number, data[2] velocity.
      case 0x90:
        if (ev.data[2] > 0) {
          auto it = std::find_if(
            alreadyRecievedNote.begin(), alreadyRecievedNote.end(),
            [&](const uint8_t &noteNo) { return noteNo == ev.data[1]; });
          if (it != std::end(alreadyRecievedNote)) break;
          dsp.pushMidiNote(
            true, ev.frame, noteId, ev.data[1], 0.0f, ev.data[2] / float(INT8_MAX));
          lastNoteId.push_back(std::pair<uint8_t, uint32_t>(ev.data[1], noteId));
          alreadyRecievedNote.push_back(ev.data[1]);
          noteId += 1;
        }
        break;

      default:
        break;
    }
  }

  void run(
    const float **inputs,
    float **outputs,
    uint32_t frames,
    const MidiEvent *midiEvents,
    uint32_t midiEventCount) override
  {
    if (outputs == nullptr) return;

    for (size_t i = 0; i < midiEventCount; ++i) handleMidi(midiEvents[i]);
    alreadyRecievedNote.resize(0);

    dsp.setParameters();
    dsp.process(frames, inputs, outputs[0]);
  }

private:
  DSPCore dsp;
  bool wasPlaying = false;
  uint32_t noteId = 0;
  std::vector<std::pair<uint8_t, uint32_t>> lastNoteId;
  std::vector<uint8_t> alreadyRecievedNote;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_PolyLoopEnvelope4)
};

Plugin *createPlugin() { return new CV_PolyLoopEnvelope4(); }

END_NAMESPACE_DISTRHO
