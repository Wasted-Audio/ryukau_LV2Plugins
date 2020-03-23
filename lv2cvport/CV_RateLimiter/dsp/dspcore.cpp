// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_RateLimiter.
//
// CV_RateLimiter is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_RateLimiter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_RateLimiter.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

void DSPCore::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  rateLimiter.setup(sampleRate);

  reset();
}

void DSPCore::reset() { rateLimiter.reset(); }

void DSPCore::startup() {}

void DSPCore::setParameters()
{
  interpRisingSlewRate.push(param.value[ParameterID::risingSlewRate]->getFloat());
  interpFallingSlewRate.push(
    -param.value[ParameterID::fallingSlewRate]->getFloat()); // Must be negative.
}

void DSPCore::process(const size_t length, const float *in0, float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    SmootherCommon<float>::setBufferIndex(i);

    rateLimiter.setRate(interpRisingSlewRate.process(), interpFallingSlewRate.process());
    out0[i] = rateLimiter.process(in0[i]);
  }
}
