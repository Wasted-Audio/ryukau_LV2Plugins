// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_SchroederAllpass.
//
// CV_SchroederAllpass is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_SchroederAllpass is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_SchroederAllpass.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

void DSPCore::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  delay.setup(sampleRate, Scales::time.getMax());

  reset();
}

void DSPCore::reset()
{
  using ID = ParameterID::ID;

  delay.reset();

  interpTime.reset(param.value[ID::time]->getFloat());
  interpFeedback.reset(param.value[ID::feedback]->getFloat());
}

void DSPCore::startup() {}

void DSPCore::setParameters()
{
  using ID = ParameterID::ID;

  interpTime.push(param.value[ID::time]->getFloat());
  interpFeedback.push(param.value[ID::feedback]->getFloat());
}

void DSPCore::process(
  const size_t length,
  const float *in0,
  const float *inTime,
  const float *inFeedback,
  float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    SmootherCommon<float>::setBufferIndex(i);

    out0[i] = delay.process(
      in0[i], sampleRate,
      std::clamp(interpTime.process() + inTime[i], 0.0f, float(Scales::time.getMax())),
      std::clamp(interpFeedback.process() + inFeedback[i], -1.0f, 1.0f));
  }
}
