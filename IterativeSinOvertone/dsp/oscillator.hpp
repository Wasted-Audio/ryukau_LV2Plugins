// (c) 2019 Takamitsu Endo
//
// This file is part of IterativeSinCluster.
//
// IterativeSinCluster is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// IterativeSinCluster is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with IterativeSinCluster.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "constants.hpp"

#include <array>

#include "../../lib/vcl/vectorclass.h"
#include "../../lib/vcl/vectormath_exp.h"
#include "../../lib/vcl/vectormath_trig.h"

namespace SomeDSP {

template<size_t size> struct alignas(64) QuadOscExpAD {
  std::array<Vec16f, size> frequency{};
  std::array<Vec16f, size> phase{};  // [0, 2pi].
  std::array<Vec16f, size> attack{}; // In seconds.
  std::array<Vec16f, size> decay{};  // In seconds.
  std::array<Vec16f, size> gain{};

  std::array<Vec16f, size> u{};
  std::array<Vec16f, size> v{};
  std::array<Vec16f, size> k1{};
  std::array<Vec16f, size> k2{};

  std::array<Vec16f, size> valueA{};
  std::array<Vec16f, size> alphaA{};
  std::array<Vec16f, size> valueD{};
  std::array<Vec16f, size> alphaD{};

  size_t elapsed = 0; // In samples.
  float decayGain = 1;
  const float threshold = 1e-5;

  bool isTerminated() { return decayGain <= threshold; }
  size_t getDecayGain() { return decayGain; }

  void setup(float sampleRate) noexcept
  {
    decayGain = 1.0f;

    for (size_t i = 0; i < size; ++i) {
      // Oscillator.
      k1[i] = tan(float(pi) * frequency[i] / sampleRate);
      k2[i] = 2.0f * k1[i] / (1.0f + k1[i] * k1[i]);

      v[i] = sincos(&u[i], phase[i]);

      // Gain envelope.
      valueA[i] = 1.0f;
      alphaA[i] = pow(Vec16f(threshold), 1.0f / (attack[i] * sampleRate));

      valueD[i] = 1.0f;
      alphaD[i] = pow(Vec16f(threshold), 1.0f / (decay[i] * sampleRate));

      // Gain normalization.
      auto a_g = logf(threshold) / attack[i];
      auto b_g = logf(threshold) / decay[i];
      auto t_g = -log(a_g / b_g + 1.0f) / a_g;
      auto gn = 1.0f / ((1.0f - exp(a_g * t_g)) * exp(b_g * t_g));

      gn = select(attack[i] <= 0.0f, 1.0f, gn);
      gn = select(decay[i] <= 0.0f, 0.0f, gn);

      gain[i] *= gn;
    }
  }

  float process()
  {
    decayGain = 0.0f;
    float sum = 0.0f;
    for (size_t i = 0; i < size; ++i) {
      // Oscillator. u is cos, v is sin output.
      auto tmp = u[i] - k1[i] * v[i];
      v[i] = v[i] + k2[i] * tmp;
      u[i] = tmp - k1[i] * v[i];

      // Gain envelope.
      valueA[i] *= alphaA[i];
      valueD[i] *= alphaD[i];

      decayGain += horizontal_add(valueD[i]);
      sum += horizontal_add(gain[i] * (1.0f - valueA[i]) * valueD[i] * v[i]);
    }
    return sum / (16 * size);
  }
};

} // namespace SomeDSP
