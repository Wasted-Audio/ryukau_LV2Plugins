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

#define POCKETFFT_NO_MULTITHREADING
#include "../../../lib/pocketfft/pocketfft_hdronly.h"

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../../../lib/vcl/vectormath_trig.h"

#include <algorithm>
#include <complex>
#include <numeric>

namespace SomeDSP {

constexpr size_t maxMidiNoteNumber = 136; // midi note nubmer 136 ~= 21096 Hz.

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

template<typename T> class PocketFFT {
public:
  pocketfft::shape_t shape;
  pocketfft::stride_t strideR;
  pocketfft::stride_t strideC;
  pocketfft::shape_t axes;
  size_t ndata = 1;

  void setShape(size_t size)
  {
    this->shape = pocketfft::shape_t{size};

    strideR.resize(shape.size());
    strideC.resize(shape.size());

    size_t tmpR = sizeof(T);
    size_t tmpC = sizeof(std::complex<T>);
    for (int i = int(shape.size()) - 1; i >= 0; --i) {
      strideR[i] = tmpR;
      tmpR *= shape[i];
      strideC[i] = tmpC;
      tmpC *= shape[i];
    }

    ndata = 1;
    for (const auto &shp : shape) ndata *= shp;

    axes.resize(shape.size());
    std::iota(axes.begin(), axes.end(), 0);
  }

  void r2c(const T *data_in, std::complex<T> *data_out, bool forward = true, T scale = 1)
  {
    pocketfft::r2c(shape, strideR, strideC, axes, forward, data_in, data_out, scale);
  }

  void c2r(const std::complex<T> *data_in, T *data_out, bool forward = false, T scale = 1)
  {
    pocketfft::c2r(
      shape, strideC, strideR, axes, forward, data_in, data_out, scale / ndata);
  }
};

/**
Last element of table is padded for linear interpolation.
For example, consider following table:

```
tableRaw = [11, 22, 33, 44].
```

which will be padded as following:

```
tablePadded = [11, 22, 33, 44, 11].
                               ^ This element is padded.
```
 */
template<size_t tableSize> struct Wavetable {
  enum InterpType : uint8_t { interpStep, interpLinear, interpCubic };

  static constexpr size_t spectrumSize = tableSize / 2 + 1;
  std::array<std::complex<float>, spectrumSize> spectrum;
  std::array<std::complex<float>, spectrumSize> tmpSpec;
  std::array<float, tableSize> tmpTable;
  std::array<std::array<float, tableSize + 1>, maxMidiNoteNumber + 1> table;
  PocketFFT<float> fft;

  Wavetable() { fft.setShape(tableSize); }

  size_t getTableSize() { return tableSize; }

  void refreshTable(float sampleRate, std::vector<float> &uiTable, uint8_t interpType)
  {
    switch (interpType) {
      case interpStep: {
        for (size_t idx = 0; idx < tableSize; ++idx) {
          size_t uiIdx = uiTable.size() * idx / float(tableSize);
          tmpTable[idx] = uiTable[uiIdx];
        }
      } break;

      case interpLinear: {
        uiTable.push_back(uiTable[0]);
        const size_t uiTableLast = uiTable.size() - 1;
        for (size_t idx = 0; idx < tableSize; ++idx) {
          float targetIdx = uiTableLast * idx / float(tableSize);
          float frac = targetIdx - floorf(targetIdx);
          size_t uiIdx = size_t(targetIdx);
          tmpTable[idx] = uiTable[uiIdx] + frac * (uiTable[uiIdx + 1] - uiTable[uiIdx]);
        }
      } break;

      case interpCubic:
      default: {
        uiTable.insert(uiTable.begin(), uiTable.back());
        uiTable.push_back(uiTable[1]);
        uiTable.push_back(uiTable[2]);
        const size_t uiTableLast = uiTable.size() - 3;
        for (size_t idx = 0; idx < tableSize; ++idx) {
          float targetIdx = 1 + uiTableLast * idx / float(tableSize);
          float frac = targetIdx - floorf(targetIdx);
          size_t uiIdx = size_t(targetIdx);
          tmpTable[idx] = cubicInterp(
            uiTable[uiIdx - 1], uiTable[uiIdx], uiTable[uiIdx + 1], uiTable[uiIdx + 2],
            frac);
        }
      } break;
    }

    fft.r2c(tmpTable.data(), spectrum.data());

    for (size_t i = 0; i < table.size() - 1; ++i)
      fillTable(sampleRate, 440.0 * pow(2.0, (i - 69.0) / 12.0), table[i]);

    table.back().fill(0);
  }

  /**
  Band limiting spectrum by setting frequency bin exceeding nyquist frequency to 0.
  */
  void
  fillTable(float sampleRate, float frequency, std::array<float, tableSize + 1> &table)
  {
    float tableBaseFreq = sampleRate / tableSize;
    size_t bandIdx = size_t(spectrum.size() * tableBaseFreq / frequency);
    bandIdx = std::clamp<size_t>(bandIdx, 1, spectrum.size());

    std::copy_n(spectrum.begin(), bandIdx, tmpSpec.begin());
    std::fill(tmpSpec.begin() + bandIdx, tmpSpec.end(), 0);

    fft.c2r(tmpSpec.data(), table.data());

    table[table.size() - 1] = table[0]; // Fill padded elements.
  }
};

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
  std::array<float, nOsc> prevPhase{};
  std::array<float, nOsc> prevCouplingPhase{};
  std::array<float, nOsc> phase{};
  std::array<float, nOsc> coupling{};
  std::array<float, nOsc> couplingDecay{};
  std::array<float, nOsc> gain{};
  std::array<float, nOsc> decay{};
  float prevImagPhase = 0;

  void setup(float sampleRate) {}

  void trigger()
  {
    phase.fill(0);
    prevPhase.fill(0);
    prevCouplingPhase.fill(0);
    prevImagPhase = 0;
  }

  void set() {}

  inline float
  freqToPitch(float frequency, float equalTemperament = 12.0f, float a4Hz = 440.0f)
  {
    return log2f(frequency / a4Hz) * equalTemperament + 69.0f;
  }

  // phase must be in [0, 1).
  float interpTable(
    float sampleRate,
    float notePitch,
    float phase,
    std::array<std::array<float, tableSize + 1>, maxMidiNoteNumber + 1> &table)
  {
    phase *= tableSize;

    // auto L = uint16_t(phase);
    // float frac = phase - floorf(phase);
    // return table[L] + frac * (table[L + 1] - table[L]);

    if (notePitch <= 0) {
      float frac = phase - floor(phase);
      uint16_t x0 = phase;
      uint16_t x1 = x0 + 1;
      return table[0][x0] + frac * (table[0][x1] - table[0][x0]);
    } else if (notePitch >= maxMidiNoteNumber) {
      return 0;
    }

    // Bilinear interpolation. U: up, D: down, L: left, R: right.
    uint16_t U = uint16_t(notePitch);
    uint16_t L = uint16_t(phase);
    uint16_t D = U + 1;
    uint16_t R = L + 1;
    auto xFrac = phase - floor(phase);
    auto yFrac = notePitch - floor(notePitch);
    auto x0 = table[U][L] + xFrac * (table[U][R] - table[U][L]);
    auto x1 = table[D][L] + xFrac * (table[D][R] - table[D][L]);
    return x0 + yFrac * (x1 - x0);
  }

  float process(
    float sampleRate,
    std::array<std::array<float, tableSize + 1>, maxMidiNoteNumber + 1> &table)
  {
    float output = 0;

    std::complex<float> sum(0, 0);

    for (uint16_t idx = 0; idx < nOsc; ++idx)
      sum += gain[idx] * std::polar<float>(1, float(twopi) * phase[idx]);

    sum /= float(nOsc);
    float imagAngle = std::arg(sum) / float(twopi);
    float realAngle = imagAngle + 0.25;
    float imagPhase = imagAngle - floorf(imagAngle);
    float realPhase = realAngle - floorf(realAngle);
    const float nyquist = sampleRate / 2.0f;
    float diffImagPhase = imagPhase - prevImagPhase;
    float meanApproxPitch
      = freqToPitch((diffImagPhase - floorf(diffImagPhase)) * nyquist);
    float real = interpTable(sampleRate, meanApproxPitch, realPhase, table);
    float imag = interpTable(sampleRate, meanApproxPitch, imagPhase, table);
    float amp = sqrtf(real * real + imag * imag);
    prevImagPhase = imagPhase;

    for (uint16_t idx = 0; idx < nOsc; ++idx) {
      float cplPhase = imagAngle - phase[idx];
      cplPhase -= floorf(cplPhase);
      float diffCplPhase = cplPhase - prevCouplingPhase[idx];
      diffCplPhase -= floorf(diffCplPhase);
      float tableOut
        = interpTable(sampleRate, freqToPitch(diffCplPhase * nyquist), cplPhase, table);
      prevCouplingPhase[idx] = cplPhase;

      float cpl = coupling[idx] * (float(1) - gain[idx] * couplingDecay[idx]);
      phase[idx] += frequency[idx] + cpl * amp * tableOut;
      phase[idx] -= floorf(phase[idx]);

      float diffPhase = phase[idx] - prevPhase[idx];
      diffPhase -= floorf(diffPhase);
      output += gain[idx]
        * interpTable(sampleRate, freqToPitch(diffPhase * nyquist), phase[idx], table);
      gain[idx] *= decay[idx];
    }
    prevPhase = phase;

    return output / nOsc;
  }
};

} // namespace SomeDSP
