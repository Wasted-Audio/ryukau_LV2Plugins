// (c) 2020 Takamitsu Endo
//
// This file is part of KuramotoModel.
//
// KuramotoModel is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KuramotoModel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KuramotoModel.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../../../lib/vcl/vectormath_trig.h"

#include <complex>

namespace SomeDSP {

template<typename Sample> class AttackGate {
public:
  void reset(Sample sampleRate, Sample seconds)
  {
    value = 0;
    ramp = Sample(1) / (sampleRate * seconds);

    smoother.reset(value);
    smoother.setCutoff(sampleRate, Sample(1) / seconds);
  }

  Sample process()
  {
    value += ramp;
    smoother.push(value >= Sample(1) ? Sample(1) : value);
    return smoother.process();
  }

private:
  ExpSmootherLocal<Sample> smoother;
  Sample value = 0;
  Sample ramp = 0;
};

template<typename Sample, uint16_t size, uint8_t overtone> struct KuramotoOsc {
  std::array<Sample, size> frequency{}; // In radian per sample.
  std::array<std::array<Sample, size>, overtone> phase{};
  std::array<Sample, size> coupling{};
  std::array<Sample, size> couplingDecay{};
  std::array<Sample, size> gain{};
  std::array<Sample, size> decay{};

  void setup(Sample sampleRate) {}

  void trigger()
  {
    for (auto &ph : phase) ph.fill(0);
  }

  void set() {}

  Sample process()
  {
    Sample output = 0;
    for (uint8_t ot = 0; ot < overtone; ++ot) {
      std::complex<Sample> sum(0, 0);

      for (uint16_t idx = 0; idx < size; ++idx)
        sum += gain[idx] * std::polar(Sample(1), phase[ot][idx]);

      sum /= Sample(size);
      auto amp = std::abs(sum);
      auto angle = std::arg(sum);
      auto nWave = ot + 1;

      for (uint16_t idx = 0; idx < size; ++idx) {
        auto cpl = coupling[idx] * (Sample(1) - gain[idx] * couplingDecay[idx]);
        phase[ot][idx]
          += nWave * frequency[idx] + cpl * amp * somesin(angle - phase[ot][idx]);
        output += gain[idx] * somesin(phase[ot][idx]);
        gain[idx] *= decay[idx];
      }
    }
    return output / (size * overtone);
  }
};

template<uint8_t overtone> struct alignas(64) KuramotoOsc16 {
  std::array<Vec16f, overtone> phase;
  Vec16f frequency = 0; // Normalized frequency in [0, 1).
  Vec16f coupling = 0;
  Vec16f couplingDecay = 0;
  Vec16f gain = 0;
  Vec16f decay = 0;

  void setup(float sampleRate) {}

  void trigger()
  {
    for (auto &ph : phase) ph = 0;
  }

  void set() {}

  float process()
  {
    float output = 0;
    for (uint8_t ot = 0; ot < overtone; ++ot) {
      float real = horizontal_add(gain * cos(phase[ot])) / 16;
      float imag = horizontal_add(gain * sin(phase[ot])) / 16;

      float amp = sqrtf(real * real + imag * imag);
      float angle = atan2f(imag, real);

      phase[ot] += (ot + 1) * frequency
        + coupling * (1.0f - gain * couplingDecay) * amp * sin(angle - twopi * phase[ot]);
      phase[ot] -= floor(phase[ot]); // Wrap around.
      output += horizontal_add(gain * sin(twopi * phase[ot]));
      gain *= decay;
    }
    return output / (16 * overtone);
  }
};

} // namespace SomeDSP
