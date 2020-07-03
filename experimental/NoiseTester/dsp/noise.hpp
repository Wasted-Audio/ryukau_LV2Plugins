// (c) 2020 Takamitsu Endo
//
// This file is part of NoiseTester.
//
// NoiseTester is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// NoiseTester is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with NoiseTester.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"

#include <array>
#include <climits>
#include <random>

namespace SomeDSP {

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

struct NoiseGenerator {
  enum class Type { Gaussian, Uniform };

  std::minstd_rand rng;

  NoiseGenerator(uint32_t seed = 0) { setSeed(seed); }
  void setSeed(uint32_t seed) { rng.seed(seed); }

  float process(Type type)
  {
    switch (type) {
      case Type::Gaussian: {
        std::normal_distribution<float> dist(0.0f, 0.3f);
        return dist(rng);
      }

      case Type::Uniform: {
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        return dist(rng);
      }
    }

    return 0.0f; // Just in case.
  }
};

} // namespace SomeDSP
