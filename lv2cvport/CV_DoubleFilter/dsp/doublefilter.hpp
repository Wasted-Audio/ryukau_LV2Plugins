// (c) 2020 Takamitsu Endo
//
// This file is part of CV_DoubleFilter.
//
// CV_DoubleFilter is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_DoubleFilter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_DoubleFilter.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/somemath.hpp"

#include <algorithm>

namespace SomeDSP {

template<typename Sample> class DoubleFilter {
public:
  void reset()
  {
    acc2 = vel2 = pos2 = 0;
    acc1 = vel1 = pos1 = 0;
    x1 = 0;
  }

  void set(Sample sampleRate, Sample cutoffHz, Sample resonance, bool uniformGain)
  {
    Sample x = cutoffHz / sampleRate;
    k2 = Sample(6.5451144600705975) * x + Sample(20.46391326872472) * x * x;

    // Strange tuning boundary around k2 ~= 0.2π.
    if (k2 < Sample(0.6295160864148501)) {
      k1 = Sample(pi) * resonance;
    } else {
      k1 = resonance
        * (Sample(-0.0049691265927442885)
           + Sample(1)
             / (Sample(-471.738128187657) + Sample(1432.5662635997667) * k2 + Sample(345.2853784111966) * k2 * k2 + Sample(-4454.40786711102) * k2 * k2 * k2 + Sample(3468.062963176107) * k2 * k2 * k2 * k2));
    }

    const auto k1Gain = Sample(0.69);
    const auto k1Delta = Sample(0.31); // 1 - k1Gain.
    const auto B_0 = Sample(0.61);
    const auto B_1 = Sample(0.625);
    const auto B_2 = Sample(0.63); // Almost tuning boundary.
    const auto B_3 = Sample(0.635);
    if (uniformGain) {
      // Reduce k1 where k2 is from 0.2π to the tuning boundary (~0.63).
      if (k2 >= B_0 && k2 < B_1)
        k1 *= Sample(1) - k1Delta * (k2 - B_0) / (B_1 - B_0);
      else if (k2 >= B_1 && k2 < B_2)
        k1 *= k1Gain;
      else if (k2 >= B_2 && k2 < B_3)
        k1 *= k1Gain + k1Delta * (k2 - B_2) / (B_3 - B_2);

      k1 *= Sample(0.7);

      v2Gain = somesqrt<Sample>(k1);
    } else {
      // Reduce k1 where k2 is less than tuning boundary (~0.63).
      if (k2 < B_2)
        k1 *= k1Gain;
      else if (k2 >= B_2 && k2 < B_3)
        k1 *= k1Gain + k1Delta * (k2 - B_2) / (B_3 - B_2);

      v2Gain = Sample(1);
    }
  }

  Sample process(Sample x0, bool highpass)
  {
    acc2 = k2 * (vel1 - vel2);
    vel2 += acc2 + x0 - x1;
    pos2 += vel2 * k2 * v2Gain;

    acc1 = -k1 * pos1 - acc2;
    vel1 += acc1;
    pos1 += vel1;

    x1 = x0;

    if (highpass) return pos1 *= Sample(0.999);
    return pos2 *= Sample(0.999);
  }

private:
  Sample k1 = 0;
  Sample k2 = 0;
  Sample v2Gain = 1;

  Sample acc2 = 0;
  Sample vel2 = 0;
  Sample pos2 = 0;

  Sample acc1 = 0;
  Sample vel1 = 0;
  Sample pos1 = 0;

  Sample x1 = 0;
};

} // namespace SomeDSP
