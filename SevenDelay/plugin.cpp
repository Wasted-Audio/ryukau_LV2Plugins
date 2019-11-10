// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019 Takamitsu Endo
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
  SevenDelay() : Plugin(22, 1, 0) { sampleRateChanged(getSampleRate()); }

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
        parameter.name = "Bypass";
        parameter.hints = kParameterIsAutomable | kParameterIsBoolean;
        parameter.designation = kParameterDesignationBypass;
        break;

      case ParameterID::time:
        parameter.name = "Time";
        parameter.unit = "s";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::feedback:
        parameter.name = "Feedback";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::offset:
        parameter.name = "Stereo";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::wetMix:
        parameter.name = "WetMix";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::dryMix:
        parameter.name = "DryMix";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::tempoSync:
        parameter.name = "TempoSync";
        parameter.hints = kParameterIsAutomable | kParameterIsBoolean;
        break;

      case ParameterID::negativeFeedback:
        parameter.name = "NegativeFeedback";
        parameter.hints = kParameterIsAutomable | kParameterIsBoolean;
        break;

      case ParameterID::lfoTimeAmount:
        parameter.name = "LFO_To_Time";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::lfoToneAmount:
        parameter.name = "LFO_to_Allpass";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::lfoFrequency:
        parameter.name = "LFO_Frequency";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::lfoShape:
        parameter.name = "LFO_Shape";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::lfoInitialPhase:
        parameter.name = "LFO_Initial_Phase";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::lfoHold:
        parameter.name = "LFO_Phase_Hold";
        parameter.hints = kParameterIsAutomable | kParameterIsBoolean;
        break;

      case ParameterID::smoothness:
        parameter.name = "Smoothness";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::inSpread:
        parameter.name = "Input_Stereo_Spread";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::inPan:
        parameter.name = "Input_Pan";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::outSpread:
        parameter.name = "Output_Stereo_Spread";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::outPan:
        parameter.name = "Output_Pan";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::toneCutoff:
        parameter.name = "Allpass_Cutoff";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::toneQ:
        parameter.name = "Allpass_Q";
        parameter.hints = kParameterIsAutomable;
        break;

      case ParameterID::dckill:
        parameter.name = "DC_Kill";
        parameter.hints = kParameterIsAutomable;
        break;

        // Add parameter here.

      default:
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Empty";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        break;
    }

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
    switch (index) {
      case 0:
        programName = "Default";
        break;

        // Add program here.
    }
  }

  void loadProgram(uint32_t index) override
  {
    switch (index) {
      case 0:
        break;

        // Add program here.
    }
  }

  void sampleRateChanged(double newSampleRate) { dsp.setup(newSampleRate); }

  void activate() { dsp.startup(); }

  void deactivate() { dsp.reset(); }

  void run(const float **inputs, float **outputs, uint32_t frames) override
  {
    if (dsp.param.value[ParameterID::bypass]->getRaw()) {
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
