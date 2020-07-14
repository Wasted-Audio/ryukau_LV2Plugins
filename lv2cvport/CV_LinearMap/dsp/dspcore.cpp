// (c) 2020 Takamitsu Endo
//
// This file is part of CV_LinearMap.
//
// CV_LinearMap is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_LinearMap is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_LinearMap.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

void DSPCore::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  reset();
}

void DSPCore::reset() {}

void DSPCore::startup() {}

void DSPCore::setParameters()
{
  using ID = ParameterID::ID;

  interpAdd.push(
    (int32_t(param.value[ID::addInt]->getInt()) - 128)
    + param.value[ID::addFrac]->getFloat());
  interpMul.push(
    (int32_t(param.value[ID::mulInt]->getInt()) - 128)
    + param.value[ID::mulFrac]->getFloat());
}

void DSPCore::process(const size_t length, const float *in0, float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    out0[i] = interpAdd.process() + interpMul.process() * in0[i];
  }
}
