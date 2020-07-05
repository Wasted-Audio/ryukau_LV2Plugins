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

template<typename Sample> class alignas(64) ExpDecayCurve {
public:
  void reset(Sample sampleRate, Sample seconds)
  {
    value = Sample(1);
    set(sampleRate, seconds);
  }

  void set(Sample sampleRate, Sample seconds)
  {
    alpha = somepow<Sample>(threshold, Sample(1) / (seconds * sampleRate));
  }

  bool isTerminated() { return value <= threshold; }

  Sample process()
  {
    if (value <= threshold) return Sample(0);
    value *= alpha;
    return value - threshold;
  }

protected:
  const Sample threshold = 1e-5;
  Sample value = 0;
  Sample alpha = 0;
};

template<typename Sample, uint16_t size> struct KuramotoOsc {
  std::array<Sample, size> frequency{}; // In radian per sample.
  std::array<Sample, size> phase{};
  std::array<Sample, size> coupling{};
  std::array<Sample, size> couplingDecay{};
  std::array<Sample, size> gain{};
  std::array<Sample, size> decay{};

  void setup(Sample sampleRate) {}

  void trigger() { phase.fill(0); }

  void set() {}

  Sample process()
  {
    std::complex<Sample> sum(0, 0);

    for (uint16_t idx = 0; idx < size; ++idx)
      sum += gain[idx] * std::polar(Sample(1), phase[idx]);

    sum /= Sample(size);
    auto amp = std::abs(sum);
    auto angle = std::arg(sum);

    Sample output = 0;
    for (uint16_t idx = 0; idx < size; ++idx) {
      auto cpl = coupling[idx] * (Sample(1) - gain[idx] * couplingDecay[idx]);
      phase[idx] += frequency[idx] + cpl * amp * somesin(angle - phase[idx]);
      output += gain[idx] * somesin(phase[idx]);
      gain[idx] *= decay[idx];
    }

    return output / size;
  }
};

} // namespace SomeDSP
