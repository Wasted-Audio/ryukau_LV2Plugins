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

// Range of t is in [0, 1]. Interpoltes between y1 and y2.
inline float cubicInterp(float y0, float y1, float y2, float y3, float t)
{
  auto t2 = t * t;
  auto c0 = y1 - y2;
  auto c1 = (y2 - y0) * 0.5f;
  auto c2 = c0 + c1;
  auto c3 = c0 + c2 + (y3 - y1) * 0.5f;
  return c3 * t * t2 - (c2 + c3) * t2 + c1 * t + y1;
}

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

template<size_t tableSize> struct alignas(64) LfoWavetable {
  std::array<float, tableSize + 1> table;

  enum InterpType : int32_t { interpStep, interpLinear, interpCubic };

  void refreshTable(std::vector<float> &uiTable, int interpType)
  {
    const size_t last = table.size() - 1;
    switch (interpType) {
      case interpStep: {
        for (size_t idx = 0; idx < last; ++idx) {
          size_t uiIdx = uiTable.size() * idx / float(last);
          table[idx] = uiTable[uiIdx];
        }
      } break;

      case interpLinear: {
        uiTable.push_back(uiTable[0]);
        const size_t uiTableLast = uiTable.size() - 1;
        for (size_t idx = 0; idx < last; ++idx) {
          float targetIdx = uiTableLast * idx / float(last);
          float frac = targetIdx - floorf(targetIdx);
          size_t uiIdx = size_t(targetIdx);
          table[idx] = uiTable[uiIdx] + frac * (uiTable[uiIdx + 1] - uiTable[uiIdx]);
        }
      } break;

      case interpCubic:
      default: {
        uiTable.insert(uiTable.begin(), uiTable.back());
        uiTable.push_back(uiTable[1]);
        uiTable.push_back(uiTable[2]);
        const size_t uiTableLast = uiTable.size() - 3;
        for (size_t idx = 0; idx < last; ++idx) {
          float targetIdx = 1 + uiTableLast * idx / float(last);
          float frac = targetIdx - floorf(targetIdx);
          size_t uiIdx = size_t(targetIdx);
          table[idx] = cubicInterp(
            uiTable[uiIdx - 1], uiTable[uiIdx], uiTable[uiIdx + 1], uiTable[uiIdx + 2],
            frac);
        }
      } break;
    }
    table[last] = table[0];
  }
};

template<uint16_t tableSize, uint16_t nOsc> struct KuramotoTableOsc {
  std::array<float, nOsc> frequency{}; // Normalized frequency in [0, 1).
  std::array<float, nOsc> approxFreq{};
  std::array<float, nOsc> prevPhase{};
  std::array<float, nOsc> phase{};
  std::array<float, nOsc> coupling{};
  std::array<float, nOsc> couplingDecay{};
  std::array<float, nOsc> gain{};
  std::array<float, nOsc> decay{};

  void setup(float sampleRate) {}

  void trigger()
  {
    prevPhase.fill(0);
    phase.fill(0);
  }

  void set() {}

  // phase must be in [0, 1).
  float
  interpTable(float sampleRate, float phase, std::array<float, tableSize + 1> &table)
  {
    phase *= tableSize;

    auto L = uint16_t(phase);
    float frac = phase - floorf(phase);
    return table[L] + frac * (table[L + 1] - table[L]);
  }

  float process(float sampleRate, std::array<float, tableSize + 1> &table)
  {
    float output = 0;

    std::complex<float> sum(0, 0);

    for (uint16_t idx = 0; idx < nOsc; ++idx)
      sum += gain[idx] * std::polar<float>(1, float(twopi) * phase[idx]);

    sum /= float(nOsc);
    float angle = std::arg(sum) / float(twopi);

    float cosAngle = angle + 0.25;
    float real = interpTable(sampleRate, cosAngle - floorf(cosAngle), table);
    float imag = interpTable(sampleRate, angle - floorf(angle), table);
    float amp = sqrtf(real * real + imag * imag);

    for (uint16_t idx = 0; idx < nOsc; ++idx) {
      if (prevPhase[idx] > phase[idx]) prevPhase[idx] -= 1.0f;
      approxFreq[idx] = (phase[idx] - prevPhase[idx]) * sampleRate / 2.0f;
    }

    for (uint16_t idx = 0; idx < nOsc; ++idx) {
      float diffPhase = angle - phase[idx];
      float tableOut = interpTable(sampleRate, diffPhase - floorf(diffPhase), table);
      float cpl = coupling[idx] * (float(1) - gain[idx] * couplingDecay[idx]);
      phase[idx] += frequency[idx] + cpl * amp * tableOut;
      phase[idx] -= floorf(phase[idx]);
      output += gain[idx] * interpTable(sampleRate, phase[idx], table);
      gain[idx] *= decay[idx];
    }
    prevPhase = phase;

    return output / nOsc;
  }
};

} // namespace SomeDSP
