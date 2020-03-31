// (c) 2020 Takamitsu Endo
//
// This file is part of CV_3PoleLP.
//
// CV_3PoleLP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_3PoleLP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_3PoleLP.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/somemath.hpp"

#include <algorithm>

namespace SomeDSP {

template<typename Sample> struct LP3 {
  static Sample lowpassHzToC(Sample sampleRate, Sample cutoffHz)
  {
    const Sample x = cutoffHz / sampleRate;
    return Sample(56.85341479156533) * x * x * x * x * x * x
      + Sample(-60.92051508862034) * x * x * x * x * x
      + Sample(-1.6515635438744682) * x * x * x * x
      + Sample(31.558896956675998) * x * x * x + Sample(-20.61402812645397) * x * x
      + Sample(6.320753515093109) * x; /* + Sample(-0.00015711883321284232); */
  }

  static Sample resonanceToK(Sample c, Sample resonance, bool uniformPeak)
  {
    if (uniformPeak) {
      const Sample kExp = someexp<Sample>(Sample(-5.6852537097945195) * resonance);
      const Sample kMin = Sample(1) - kExp;
      const Sample kMax = Sample(0.9999771732485103)
        - Sample(0.01) * (kExp - Sample(0.0033956716251850594));
      return kMax - (kMax - kMin) * someacos<Sample>(Sample(1) - c) / Sample(halfpi);
    }
    return std::clamp<Sample>(resonance, Sample(0), Sample(1 - 1e-5));
  }

  static Sample highpassHzToDecay(Sample sampleRate, Sample cutoffHz)
  {
    const Sample x = cutoffHz / sampleRate;

    // -3 dB cutoff.
    return Sample(0.5638865655409118)
      + Sample(0.43611343445908823) * someexp<Sample>(Sample(-6.501239408777854) * x);

    // -6 dB cutoff.
    // return Sample(0.39299084333814804)
    //   + Sample(0.607009156661852)
    //   * someexp<Sample>(Sample(-4.939791161282682) * x);
  }

  Sample c = 0;
  Sample k = 0;
  Sample decay = 1;
  bool uniformGain = true;
  Sample acc = 0;
  Sample vel = 0;
  Sample pos = 0;
  Sample x1 = 0;

  void reset() { acc = vel = pos = x1 = 0; }

  void setRaw(
    Sample sampleRate,
    Sample cutoffHz,
    Sample resonance,
    Sample decay,
    bool uniformPeak,
    bool uniformGain)
  {
    const Sample x = cutoffHz / sampleRate;
    c = Sample(56.85341479156533) * x * x * x * x * x * x
      + Sample(-60.92051508862034) * x * x * x * x * x
      + Sample(-1.6515635438744682) * x * x * x * x
      + Sample(31.558896956675998) * x * x * x + Sample(-20.61402812645397) * x * x
      + Sample(6.320753515093109) * x; /* + Sample(-0.00015711883321284232); */

    if (uniformPeak) {
      const Sample kExp = someexp<Sample>(Sample(-5.6852537097945195) * resonance);
      const Sample kMin = Sample(1) - kExp;
      const Sample kMax = Sample(0.9999771732485103)
        - Sample(0.01) * (kExp - Sample(0.0033956716251850594));
      k = kMax - (kMax - kMin) * someacos<Sample>(Sample(1) - c) / Sample(halfpi);
    } else {
      k = std::clamp<Sample>(resonance, Sample(0), Sample(1 - 1e-5));
    }

    this->decay = decay;
    this->uniformGain = uniformGain;
  }

  Sample process(Sample x0)
  {
    acc = k * acc + c * vel;
    vel -= acc + x0 - x1;
    pos -= (uniformGain ? c / (1 - k) : c) * vel;

    pos *= Sample(decay);

    x1 = x0;
    return pos;
  }
};

} // namespace SomeDSP
