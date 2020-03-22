// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_Multiply.
//
// CV_Multiply is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_Multiply is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_Multiply.  If not, see <https://www.gnu.org/licenses/>.

#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

class CV_Multiply : public Plugin {
public:
  // Plugin(nParameters, nPrograms, nStates).
  CV_Multiply() : Plugin(0, 0, 0) { sampleRateChanged(getSampleRate()); }

protected:
  /* Information */
  const char *getLabel() const override { return "UhhyouCV_Multiply"; }
  const char *getDescription() const override { return "Multiply CV port signal."; }
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

  void initParameter(uint32_t /* index */, Parameter & /*parameter*/) override {}
  float getParameterValue(uint32_t /* index */) const override { return 0; }
  void setParameterValue(uint32_t /* index */, float /* value */) override {}
  void sampleRateChanged(double /* newSampleRate */) {}
  void activate() {}
  void deactivate() {}

  void run(const float **in, float **out, uint32_t frames) override
  {
    const auto in0 = in[0];
    const auto in1 = in[1];
    const auto out0 = out[0];
    for (uint32_t i = 0; i < frames; ++i) out0[i] = in0[i] * in1[i];
  }

private:
  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_Multiply)
};

Plugin *createPlugin() { return new CV_Multiply(); }

END_NAMESPACE_DISTRHO
