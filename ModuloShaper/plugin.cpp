// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of ModuloShaper.
//
// ModuloShaper is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ModuloShaper is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ModuloShaper.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>

#include <memory>
#include <utility>

#include "DistrhoPlugin.hpp"
#include "dsp/dspcore.hpp"

START_NAMESPACE_DISTRHO

class ModuloShaper : public Plugin {
public:
  // Plugin(nParameters, nPrograms, nStates).
  ModuloShaper()
    : Plugin(ParameterID::ID_ENUM_LENGTH, GlobalParameter::Preset::Preset_ENUM_LENGTH, 0)
  {
    auto iset = instrset_detect();
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
    dsp->param.validate();

    setLatency(dsp->getLatency());
    sampleRateChanged(getSampleRate());
  }

protected:
  /* Information */
  const char *getLabel() const override { return "ModuloShaper"; }
  const char *getDescription() const override
  {
    return "A waveshaper using modulo operation.";
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
  int64_t getUniqueId() const override { return d_cconst('u', 'f', 'o', 'l'); }

  void initParameter(uint32_t index, Parameter &parameter) override
  {
    dsp->param.initParameter(index, parameter);

    switch (index) {
      case ParameterID::bypass:
        parameter.designation = kParameterDesignationBypass;
        break;
    }

    parameter.symbol = parameter.name;
  }

  float getParameterValue(uint32_t index) const override
  {
    return dsp->param.getFloat(index);
  }

  void setParameterValue(uint32_t index, float value) override
  {
    dsp->param.setParameterValue(index, value);
  }

  void initProgramName(uint32_t index, String &programName) override
  {
    dsp->param.initProgramName(index, programName);
  }

  void loadProgram(uint32_t index) override { dsp->param.loadProgram(index); }

  void initState(uint32_t /* index */, String & /* key */, String & /* defaultValue */) {}

  String getState(const char *) const { return String("N/A"); }
  void setState(const char * /* key */, const char *) {}

  void sampleRateChanged(double newSampleRate) { dsp->setup(newSampleRate); }
  void activate() { dsp->startup(); }
  void deactivate() { dsp->reset(); }

  void run(const float **inputs, float **outputs, uint32_t frames) override
  {
    if (dsp->param.value[ParameterID::bypass]->getInt()) {
      if (outputs[0] != inputs[0])
        std::memcpy(outputs[0], inputs[0], sizeof(float) * frames);
      if (outputs[1] != inputs[1])
        std::memcpy(outputs[1], inputs[1], sizeof(float) * frames);
      return;
    }

    const auto timePos = getTimePosition();
    if (!wasPlaying && timePos.playing) dsp->startup();
    wasPlaying = timePos.playing;

    dsp->setParameters(timePos.bbt.beatsPerMinute);
    dsp->process(frames, inputs[0], inputs[1], outputs[0], outputs[1]);

    setLatency(dsp->getLatency());
  }

private:
  std::unique_ptr<DSPInterface> dsp;
  bool wasPlaying = false;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModuloShaper)
};

Plugin *createPlugin() { return new ModuloShaper(); }

END_NAMESPACE_DISTRHO
