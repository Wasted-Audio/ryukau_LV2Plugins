// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_HoldFilter.
//
// CV_HoldFilter is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_HoldFilter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_HoldFilter.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../parameter.hpp"
#include "holdfilter.hpp"

using namespace SomeDSP;

class DSPCore {
public:
  static const size_t maxVoice = 32;
  GlobalParameter param;

  void setup(double sampleRate);
  void reset();   // Stop sounds.
  void startup(); // Reset phase etc.
  void setParameters();
  uint32_t getLatency();
  void process(
    const size_t length,
    const float *in0,
    const float *inCutoff,
    const float *inResonance,
    const float *inPulseWidth,
    float *out0);

private:
  float sampleRate = 44100.0f;
  uint32_t overSample = 1;
  bool highpass = false;
  float output = 0;

  LinearInterp<float> inputInterp;
  HoldFilter<double> filter;
  DecimationLowpass<float> decimationLowpass;
  DCSuppressor<float> dcSuppressor;

  LinearSmoother<float> interpGain;
  LinearSmoother<float> interpCutoff;
  LinearSmoother<float> interpResonance;
  LinearSmoother<float> interpPulseWidth;
  LinearSmoother<float> interpEdge;
};
