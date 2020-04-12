// (c) 2020 Takamitsu Endo
//
// This file is part of ModuloShaper.
//
// ModuloShaper is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ModuloShaper is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ModuloShaper.  If not, see <https://www.gnu.org/licenses/>.

#include "../../common/dsp/somemath.hpp"

#include <algorithm>

namespace SomeDSP {

template<typename Sample> class Butter8Lowpass {
public:
  void reset()
  {
    x0.fill(0);
    x1.fill(0);
    x2.fill(0);
    y0.fill(0);
    y1.fill(0);
    y2.fill(0);
  }

  void setCutoff(Sample sampleRate, Sample cutoffHz)
  {
    Sample oc1 = 4 * Sample(twopi) * cutoffHz / sampleRate; // oc for ω_c.
    Sample oc2 = oc1 * oc1;
    Sample mid_oc = 2.8284271247461903 * oc1; // 2^(3/2) = 2.8284271247461903

    co[0] = oc2;                                    // b0
    co[1] = Sample(2) * oc2;                        // b1
    co[2] = oc2;                                    // b2
    co[3] = Sample(2) * oc2 - Sample(8);            // a1
    co[4] = oc2 - mid_oc + Sample(4);               // a2
    co[5] = Sample(1) / (oc2 + mid_oc + Sample(4)); // a0
  }

  Sample process(Sample input)
  {
    x0[0] = input;
    x0[1] = y0[0];
    x0[2] = y0[1];
    x0[3] = y0[2];

    y0[0] = co[5]
      * (co[0] * x0[0] + co[1] * x1[0] + co[2] * x2[0] - co[3] * y1[0] - co[4] * y2[0]);
    y0[1] = co[5]
      * (co[0] * x0[1] + co[1] * x1[1] + co[2] * x2[1] - co[3] * y1[1] - co[4] * y2[1]);
    y0[2] = co[5]
      * (co[0] * x0[2] + co[1] * x1[2] + co[2] * x2[2] - co[3] * y1[2] - co[4] * y2[2]);
    y0[3] = co[5]
      * (co[0] * x0[3] + co[1] * x1[3] + co[2] * x2[3] - co[3] * y1[3] - co[4] * y2[3]);

    x2[0] = x1[0];
    x2[1] = x1[1];
    x2[2] = x1[2];
    x2[3] = x1[3];

    x1[0] = x0[0];
    x1[1] = x0[1];
    x1[2] = x0[2];
    x1[3] = x0[3];

    y2[0] = y1[0];
    y2[1] = y1[1];
    y2[2] = y1[2];
    y2[3] = y1[3];

    y1[0] = y0[0];
    y1[1] = y0[1];
    y1[2] = y0[2];
    y1[3] = y0[3];

    return y0[3];
  }

  std::array<Sample, 4> x0{};
  std::array<Sample, 4> x1{};
  std::array<Sample, 4> x2{};
  std::array<Sample, 4> y0{};
  std::array<Sample, 4> y1{};
  std::array<Sample, 4> y2{};
  std::array<Sample, 6> co{};
};

/**
Lowpass filter specialized for 8x oversampling.

```python
import numpy
from scipy import signal
sos = signal.cheby1(16, 0.1, 19000, "low", output="sos", fs=48000 * 4)
```
*/
template<typename Sample> class DecimationLowpass {
public:
  void reset()
  {
    x0.fill(0);
    x1.fill(0);
    x2.fill(0);
    y0.fill(0);
    y1.fill(0);
    y2.fill(0);
  }

  void push(Sample input)
  {
    x0[0] = input;
    x0[1] = y0[0];
    x0[2] = y0[1];
    x0[3] = y0[2];
    x0[4] = y0[3];
    x0[5] = y0[4];
    x0[6] = y0[5];
    x0[7] = y0[6];

    y0[0] = co[0][0] * x0[0] + co[0][1] * x1[0] + co[0][2] * x2[0] - co[0][3] * y1[0]
      - co[0][4] * y2[0];
    y0[1] = co[1][0] * x0[1] + co[1][1] * x1[1] + co[1][2] * x2[1] - co[1][3] * y1[1]
      - co[1][4] * y2[1];
    y0[2] = co[2][0] * x0[2] + co[2][1] * x1[2] + co[2][2] * x2[2] - co[2][3] * y1[2]
      - co[2][4] * y2[2];
    y0[3] = co[3][0] * x0[3] + co[3][1] * x1[3] + co[3][2] * x2[3] - co[3][3] * y1[3]
      - co[3][4] * y2[3];
    y0[4] = co[4][0] * x0[4] + co[4][1] * x1[4] + co[4][2] * x2[4] - co[4][3] * y1[4]
      - co[4][4] * y2[4];
    y0[5] = co[5][0] * x0[5] + co[5][1] * x1[5] + co[5][2] * x2[5] - co[5][3] * y1[5]
      - co[5][4] * y2[5];
    y0[6] = co[6][0] * x0[6] + co[6][1] * x1[6] + co[6][2] * x2[6] - co[6][3] * y1[6]
      - co[6][4] * y2[6];
    y0[7] = co[7][0] * x0[7] + co[7][1] * x1[7] + co[7][2] * x2[7] - co[7][3] * y1[7]
      - co[7][4] * y2[7];

    x2[0] = x1[0];
    x2[1] = x1[1];
    x2[2] = x1[2];
    x2[3] = x1[3];
    x2[4] = x1[4];
    x2[5] = x1[5];
    x2[6] = x1[6];
    x2[7] = x1[7];

    x1[0] = x0[0];
    x1[1] = x0[1];
    x1[2] = x0[2];
    x1[3] = x0[3];
    x1[4] = x0[4];
    x1[5] = x0[5];
    x1[6] = x0[6];
    x1[7] = x0[7];

    y2[0] = y1[0];
    y2[1] = y1[1];
    y2[2] = y1[2];
    y2[3] = y1[3];
    y2[4] = y1[4];
    y2[5] = y1[5];
    y2[6] = y1[6];
    y2[7] = y1[7];

    y1[0] = y0[0];
    y1[1] = y0[1];
    y1[2] = y0[2];
    y1[3] = y0[3];
    y1[4] = y0[4];
    y1[5] = y0[5];
    y1[6] = y0[6];
    y1[7] = y0[7];
  }

  Sample output() { return y0[7]; }

  std::array<Sample, 8> x0{};
  std::array<Sample, 8> x1{};
  std::array<Sample, 8> x2{};
  std::array<Sample, 8> y0{};
  std::array<Sample, 8> y1{};
  std::array<Sample, 8> y2{};
  const std::array<std::array<Sample, 5>, 8> co{{
    {1.037461353040174e-12, 2.074922706080348e-12, 1.037461353040174e-12,
     -1.7996569067448427, 0.8130122505660884},
    {1.0, 2.0, 1.0, -1.7797400857773829, 0.8208016638807095},
    {1.0, 2.0, 1.0, -1.7439946113394638, 0.8358014343214331},
    {1.0, 2.0, 1.0, -1.6996176694028573, 0.8570102011388824},
    {1.0, 2.0, 1.0, -1.6553794808551048, 0.883245305197156},
    {1.0, 2.0, 1.0, -1.620088217313659, 0.9133701015095791},
    {1.0, 2.0, 1.0, -1.6014310832801397, 0.9464360348921607},
    {1.0, 2.0, 1.0, -1.6052599612535787, 0.9817143017294799},
  }};
};

template<typename Sample> struct ModuloShaper {
  Sample gain = 1;
  Sample add = 1;
  Sample mul = 1;

  Sample x1 = 0;
  DecimationLowpass<Sample> lowpass;

  void reset()
  {
    x1 = 0;
    lowpass.reset();
  }

  Sample process(Sample x0)
  {
    Sample sign = somecopysign<Sample>(Sample(1), x0);
    x0 = somefabs<Sample>(x0 * gain);
    Sample floored = somefloor<Sample>(x0);
    Sample height = somepow<Sample>(add, floored);
    return sign
      * ((x0 - floored) * somepow<Sample>(mul, floored) * height + Sample(1) - height);
  }

  float process4x(Sample x0)
  {
    lowpass.push(process(x1));
    lowpass.push(process(x1 + float(0.25) * (x0 - x1)));
    lowpass.push(process(x1 + float(0.50) * (x0 - x1)));
    lowpass.push(process(x1 + float(0.75) * (x0 - x1)));
    x1 = x0;
    return lowpass.output();
  }
};

template<typename Sample> class ModuloShaperPolyBLEP {
private:
  Sample x0 = 0;
  Sample x1 = 0;
  Sample x2 = 0;
  Sample x3 = 0;
  Sample x4 = 0;
  Sample x5 = 0;
  Sample x6 = 0;
  Sample x7 = 0;

  Sample f0 = 0;
  Sample f1 = 0;
  Sample f2 = 0;
  Sample f3 = 0;

  Sample lastInt = 0;

public:
  Sample gain = 1;
  Sample add = 1;
  Sample mul = 1;

  void reset()
  {
    x0 = x1 = x2 = x3 = x4 = x5 = x6 = x7 = 0;
    f0 = f1 = f2 = f3 = 0;
    lastInt = 0;
  }

  Sample process4(Sample input)
  {
    Sample sign = somecopysign<Sample>(Sample(1), input);
    input = somefabs<Sample>(input * gain);
    Sample floored = somefloor<Sample>(input);
    Sample height = somepow<Sample>(add, floored);
    input = sign
      * ((input - floored) * somepow<Sample>(mul, floored) * height + Sample(1) - height);

    if (f1 != 0) {
      Sample x2_abs = somefabs<Sample>(x2);
      Sample t = std::clamp<Sample>(
        (Sample(1) - x2_abs) / (somefabs<Sample>(x1) + Sample(1) - x2_abs), Sample(0),
        Sample(1));

      Sample polyBlepResidual4_0 = -t * t * t * t / Sample(24) + t * t * t / Sample(6)
        - t * t / Sample(4) + t / Sample(6) - Sample(1) / Sample(24);
      Sample polyBlepResidual4_1 = t * t * t * t / Sample(8) - t * t * t / Sample(3)
        + Sample(2) * t / Sample(3) - Sample(1) / Sample(2);
      Sample polyBlepResidual4_2 = -t * t * t * t / Sample(8) + t * t * t / Sample(6)
        + t * t / Sample(4) + t / Sample(6) + Sample(1) / Sample(24);
      Sample polyBlepResidual4_3 = t * t * t * t / Sample(24);

      x0 += f1 * polyBlepResidual4_0;
      x1 += f1 * polyBlepResidual4_1;
      x2 += f1 * polyBlepResidual4_2;
      x3 += f1 * polyBlepResidual4_3;
    }

    f1 = f0;

    if (floored != lastInt) {
      if (floored < lastInt) height *= Sample(0.5);
      f0 = height * sign * somecopysign<Sample>(Sample(1), lastInt - floored);
      lastInt = floored;
    } else {
      f0 = 0;
    }

    x3 = x2;
    x2 = x1;
    x1 = x0;
    x0 = input;

    return x3;
  }

  Sample process8(Sample input)
  {
    Sample sign = somecopysign<Sample>(Sample(1), input);
    input = somefabs<Sample>(input * gain);
    Sample floored = somefloor<Sample>(input);
    Sample height = somepow<Sample>(add, floored);
    input = sign
      * ((input - floored) * somepow<Sample>(mul, floored) * height + Sample(1) - height);

    if (f3 != 0) {
      Sample x4_abs = somefabs<Sample>(x4);
      Sample t = std::clamp<Sample>(
        (Sample(1) - x4_abs) / (somefabs<Sample>(x3) + Sample(1) - x4_abs), Sample(0),
        Sample(1));

      Sample polyBlepResidual8_0 = -t * t * t * t * t * t * t * t / Sample(40320)
        + t * t * t * t * t * t * t / Sample(5040) - t * t * t * t * t * t / Sample(1440)
        + t * t * t * t * t / Sample(720) - t * t * t * t / Sample(576)
        + t * t * t / Sample(720) - t * t / Sample(1440) + t / Sample(5040)
        - Sample(1) / Sample(40320);
      Sample polyBlepResidual8_1 = t * t * t * t * t * t * t * t / Sample(5760)
        - t * t * t * t * t * t * t / Sample(840) + t * t * t * t * t * t / Sample(360)
        - t * t * t * t / Sample(72) + t * t * t / Sample(30)
        - Sample(7) * t * t / Sample(180) + t / Sample(42) - Sample(31) / Sample(5040);
      Sample polyBlepResidual8_2 = -t * t * t * t * t * t * t * t / Sample(1920)
        + t * t * t * t * t * t * t / Sample(336) - t * t * t * t * t * t / Sample(288)
        - t * t * t * t * t / Sample(80) + t * t * t * t * float(19) / Sample(576)
        + t * t * t / Sample(48) - Sample(49) * t * t / Sample(288)
        + Sample(397) * t / Sample(1680) - Sample(4541) / Sample(40320);
      Sample polyBlepResidual8_3 = t * t * t * t * t * t * t * t / Sample(1152)
        - t * t * t * t * t * t * t / Sample(252) + t * t * t * t * t / Sample(45)
        - t * t * t / Sample(9) + Sample(151) * t / Sample(315) - Sample(1) / Sample(2);
      Sample polyBlepResidual8_4 = -t * t * t * t * t * t * t * t / Sample(1152)
        + t * t * t * t * t * t * t / Sample(336) + t * t * t * t * t * t / Sample(288)
        - t * t * t * t * t / Sample(80) - t * t * t * t * float(19) / Sample(576)
        + t * t * t / Sample(48) + Sample(49) * t * t / Sample(288)
        + Sample(397) * t / Sample(1680) + Sample(4541) / Sample(40320);
      Sample polyBlepResidual8_5 = t * t * t * t * t * t * t * t / Sample(1920)
        - t * t * t * t * t * t * t / Sample(840) - t * t * t * t * t * t / Sample(360)
        + t * t * t * t / Sample(72) + t * t * t / Sample(30)
        + Sample(7) * t * t / Sample(180) + t / Sample(42) + Sample(31) / Sample(5040);
      Sample polyBlepResidual8_6 = -t * t * t * t * t * t * t * t / Sample(5760)
        + t * t * t * t * t * t * t / Sample(5040) + t * t * t * t * t * t / Sample(1440)
        + t * t * t * t * t / Sample(720) + t * t * t * t / Sample(576)
        + t * t * t / Sample(720) + t * t / Sample(1440) + t / Sample(5040)
        + Sample(1) / Sample(40320);
      Sample polyBlepResidual8_7 = t * t * t * t * t * t * t * t / Sample(40320);

      x0 += f3 * polyBlepResidual8_0;
      x1 += f3 * polyBlepResidual8_1;
      x2 += f3 * polyBlepResidual8_2;
      x3 += f3 * polyBlepResidual8_3;
      x4 += f3 * polyBlepResidual8_4;
      x5 += f3 * polyBlepResidual8_5;
      x6 += f3 * polyBlepResidual8_6;
      x7 += f3 * polyBlepResidual8_7;
    }

    f3 = f2;
    f2 = f1;
    f1 = f0;

    if (floored != lastInt) {
      if (floored < lastInt) height *= Sample(0.5);
      f0 = height * sign * somecopysign<Sample>(Sample(1), lastInt - floored);
      lastInt = floored;
    } else {
      f0 = 0;
    }

    x7 = x6;
    x6 = x5;
    x5 = x4;
    x4 = x3;
    x3 = x2;
    x2 = x1;
    x1 = x0;
    x0 = input;

    return x7;
  }
};

} // namespace SomeDSP
