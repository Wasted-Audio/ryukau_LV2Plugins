// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of CV_DelayLP3.
//
// CV_DelayLP3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_DelayLP3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_DelayLP3.  If not, see <https://www.gnu.org/licenses/>.

#include <utility>

#include "DistrhoPlugin.hpp"
#include "dsp/dspcore.hpp"

START_NAMESPACE_DISTRHO

class CV_DelayLP3 : public Plugin {
public:
  // Plugin(nParameters, nPrograms, nStates).
  CV_DelayLP3() : Plugin(ParameterID::ID_ENUM_LENGTH, 0, 0)
  {
    sampleRateChanged(getSampleRate());
  }

protected:
  /* Information */
  const char *getLabel() const override { return "CV_DelayLP3"; }
  const char *getDescription() const override
  {
    return "A feedback delay with a 3-pole lowpass filter on feedback path.";
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
    if (input && index == 0) {
      port.hints = kAudioPortIsCV;
      port.name = String("Input");
      port.symbol = String("cv_in");
    } else if (input && index == 1) {
      port.hints = kAudioPortIsCV;
      port.name = String("Time");
      port.symbol = String("cv_time");
    } else if (input && index == 2) {
      port.hints = kAudioPortIsCV;
      port.name = String("Feedback");
      port.symbol = String("cv_feedback");
    } else if (input && index == 3) {
      port.hints = kAudioPortIsCV;
      port.name = String("Lowpass");
      port.symbol = String("cv_lowpass");
    } else if (input && index == 4) {
      port.hints = kAudioPortIsCV;
      port.name = String("Highpass");
      port.symbol = String("cv_highpass");
    }

    else if (!input && index == 0) {
      port.hints = kAudioPortIsCV;
      port.name = String("Output");
      port.symbol = String("cv_out");
    }

    else {
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
    return dsp.param.getFloat(index);
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

  void run(const float **inputs, float **outputs, uint32_t frames) override
  {
    if (inputs == nullptr || outputs == nullptr) return;

    const auto timePos = getTimePosition();
    if (!wasPlaying && timePos.playing) dsp.startup();
    wasPlaying = timePos.playing;

    dsp.setParameters();
    dsp.process(
      frames, inputs[0], inputs[1], inputs[2], inputs[3], inputs[4], outputs[0]);
  }

private:
  DSPCore dsp;
  bool wasPlaying = false;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_DelayLP3)
};

Plugin *createPlugin() { return new CV_DelayLP3(); }

END_NAMESPACE_DISTRHO
