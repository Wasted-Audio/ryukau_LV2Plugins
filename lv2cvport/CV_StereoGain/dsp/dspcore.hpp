// (c) 2020 Takamitsu Endo
//
// This file is part of CV_StereoGain.
//
// CV_StereoGain is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_StereoGain is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_StereoGain.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../parameter.hpp"

using namespace SomeDSP;

class DSPCore {
public:
  GlobalParameter param;

  void setup(double sampleRate);
  void reset();   // Stop sounds.
  void startup(); // Reset phase etc.
  void setParameters();
  void process(
    const size_t length,
    const float *in0,
    const float *in1,
    const float *inGain,
    float *out0,
    float *out1);

private:
  float sampleRate = 44100.0f;
  LinearSmoother<float> interpGain;
  LinearSmoother<float> interpDc;
};
