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

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../parameter.hpp"

using namespace SomeDSP;

// Based on: https://www.mathworks.com/help/simulink/slref/ratelimiter.html
template<typename Sample> class RateLimiter {
public:
  void setup(Sample sampleRate) { dt = Sample(1) / sampleRate; }
  void reset() { y1 = 0; }

  void setRate(Sample risingSlewRate, Sample fallingSlewRate)
  {
    rise = risingSlewRate;
    fall = fallingSlewRate;
  }

  Sample process(Sample input)
  {
    auto rate = (input - y1) / dt;
    if (rate > rise)
      y1 += dt * rise;
    else if (rate < fall)
      y1 += dt * fall;
    else
      y1 = input;
    return y1;
  }

private:
  Sample dt = 1.0 / 44100.0;
  Sample y1 = 0;
  Sample rise = 10000;  // Should be positive.
  Sample fall = -10000; // Should be negative.
};

class DSPCore {
public:
  static const size_t maxVoice = 32;
  GlobalParameter param;

  void setup(double sampleRate);
  void reset();   // Stop sounds.
  void startup(); // Reset phase etc.
  void setParameters();
  void process(const size_t length, const float *in0, float *out0);

private:
  float sampleRate = 44100.0f;
  RateLimiter<float> rateLimiter;
  LinearSmoother<float> interpRisingSlewRate;
  LinearSmoother<float> interpFallingSlewRate;
};
