// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of SevenDelay.
//
// SevenDelay is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SevenDelay is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SevenDelay.  If not, see <https://www.gnu.org/licenses/>.

#include "DistrhoPlugin.hpp"
#include "dsp/dspcore.hpp"

START_NAMESPACE_DISTRHO

class SevenDelay : public Plugin {
public:
  // Plugin(nParameters, nPrograms, nStates).
  SevenDelay()
    : Plugin(ParameterID::ID_ENUM_LENGTH, GlobalParameter::Preset::Preset_ENUM_LENGTH, 0)
  {
    sampleRateChanged(getSampleRate());
  }

protected:
  /* Information */
  const char *getLabel() const override { return "SevenDelay"; }
  const char *getDescription() const override
  {
    return "A stereo delay tuned towards short delay.";
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
  int64_t getUniqueId() const override { return d_cconst('u', 's', 'e', 'v'); }

  void initParameter(uint32_t index, Parameter &parameter) override
  {
    dsp.param.initParameter(index, parameter);

    switch (index) {
      case ParameterID::bypass:
        parameter.designation = kParameterDesignationBypass;
        break;
    }

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

  void activate() { dsp.startup(); }

  void deactivate() { dsp.reset(); }

  void run(const float **inputs, float **outputs, uint32_t frames) override
  {
    if (dsp.param.value[ParameterID::bypass]->getInt()) {
      if (outputs[0] != inputs[0])
        std::memcpy(outputs[0], inputs[0], sizeof(float) * frames);
      if (outputs[1] != inputs[1])
        std::memcpy(outputs[1], inputs[1], sizeof(float) * frames);
      return;
    }

    const auto timePos = getTimePosition();

    if (!wasPlaying && timePos.playing) dsp.startup();
    wasPlaying = timePos.playing;

    dsp.setParameters(timePos.bbt.beatsPerMinute);
    dsp.process(frames, inputs[0], inputs[1], outputs[0], outputs[1]);
  }

private:
  DSPCore dsp;
  bool wasPlaying = false;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SevenDelay)
};

Plugin *createPlugin() { return new SevenDelay(); }

END_NAMESPACE_DISTRHO
