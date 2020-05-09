// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_NestedLongAllpass.
//
// CV_NestedLongAllpass is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_NestedLongAllpass is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_NestedLongAllpass.  If not, see <https://www.gnu.org/licenses/>.

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

  auto timeMul = param.value[ID::timeMultiply]->getFloat();
  auto outerMul = param.value[ID::outerFeedMultiply]->getFloat();
  auto innerMul = param.value[ID::innerFeedMultiply]->getFloat();
  for (size_t idx = 0; idx < nestingDepth; ++idx) {
    interpTime[idx].reset(timeMul * param.value[ID::time0 + idx]->getFloat());
    interpOuterFeed[idx].reset(outerMul * param.value[ID::outerFeed0 + idx]->getFloat());
    interpInnerFeed[idx].reset(innerMul * param.value[ID::innerFeed0 + idx]->getFloat());
  }
}

void DSPCore::startup() {}

void DSPCore::setParameters()
{
  using ID = ParameterID::ID;

  auto timeMul = param.value[ID::timeMultiply]->getFloat();
  auto outerMul = param.value[ID::outerFeedMultiply]->getFloat();
  auto innerMul = param.value[ID::innerFeedMultiply]->getFloat();
  for (size_t idx = 0; idx < nestingDepth; ++idx) {
    interpTime[idx].push(timeMul * param.value[ID::time0 + idx]->getFloat());
    interpOuterFeed[idx].push(outerMul * param.value[ID::outerFeed0 + idx]->getFloat());
    interpInnerFeed[idx].push(innerMul * param.value[ID::innerFeed0 + idx]->getFloat());
  }
}

void DSPCore::process(const size_t length, const float *in0, float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    SmootherCommon<float>::setBufferIndex(i);

    for (size_t idx = 0; idx < nestingDepth; ++idx) {
      delay.data[idx].seconds = interpTime[idx].process();
      delay.data[idx].outerFeed = interpOuterFeed[idx].process();
      delay.data[idx].innerFeed = interpInnerFeed[idx].process();
    }

    out0[i] = delay.process(in0[i], sampleRate);
  }
}
