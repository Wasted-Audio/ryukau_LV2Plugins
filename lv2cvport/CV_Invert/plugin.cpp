// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_Invert.
//
// CV_Invert is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_Invert is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_Invert.  If not, see <https://www.gnu.org/licenses/>.

#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

class CV_Invert : public Plugin {
public:
  // Plugin(nParameters, nPrograms, nStates).
  CV_Invert() : Plugin(0, 0, 0) { sampleRateChanged(getSampleRate()); }

protected:
  /* Information */
  const char *getLabel() const override { return "UhhyouCV_Invert"; }
  const char *getDescription() const override
  {
    return "Invert sign of signal. For example, amplitude +1 becomes -1 after passing "
           "through CV_Invert.";
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
    } else if (!input && index == 0) {
      port.hints = kAudioPortIsCV;
      port.name = String("Output");
      port.symbol = String("cv_out");
    } else {
      Plugin::initAudioPort(input, index, port);
    }
  }

  void initParameter(uint32_t /* index */, Parameter & /*parameter*/) override {}
  float getParameterValue(uint32_t /* index */) const override { return 0; }
  void setParameterValue(uint32_t /* index */, float /* value */) override {}
  void sampleRateChanged(double /* newSampleRate */) {}
  void activate() {}
  void deactivate() {}

  void run(const float **inputs, float **outputs, uint32_t frames) override
  {
    if (inputs == nullptr || outputs == nullptr) return;

    const float *in0 = inputs[0];
    float *out0 = outputs[0];
    for (uint32_t i = 0; i < frames; ++i) out0[i] = -in0[i];
  }

private:
  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_Invert)
};

Plugin *createPlugin() { return new CV_Invert(); }

END_NAMESPACE_DISTRHO
