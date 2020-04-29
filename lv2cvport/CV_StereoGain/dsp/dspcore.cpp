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

#include "dspcore.hpp"

void DSPCore::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  reset();
}

void DSPCore::reset()
{
  interpGain.reset(param.value[ParameterID::gain]->getFloat());
  interpDc.reset(param.value[ParameterID::dc]->getFloat());
}

void DSPCore::startup() {}

void DSPCore::setParameters()
{
  interpGain.push(param.value[ParameterID::gain]->getFloat());
  interpDc.push(param.value[ParameterID::dc]->getFloat());
}

void DSPCore::process(
  const size_t length,
  const float *in0,
  const float *in1,
  const float *inGain,
  float *out0,
  float *out1)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    SmootherCommon<float>::setBufferIndex(i);

    const float dc = interpDc.process();
    const float gain = interpGain.process() + inGain[i];

    out0[i] = dc + gain * in0[i];
    out1[i] = dc + gain * in1[i];
  }
}
