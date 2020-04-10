// (c) 2020 Takamitsu Endo
//
// This file is part of CV_HoldFilter.
//
// CV_HoldFilter is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_HoldFilter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_HoldFilter.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/somemath.hpp"

#include <algorithm>
#include <array>

namespace SomeDSP {

template<typename Sample> class LinearInterp {
public:
  void reset() { x0 = x1 = 0; }

  void push(Sample input)
  {
    x1 = x0;
    x0 = input;
  }

  // t in [0, 1].
  Sample process(Sample t) { return x1 + t * (x0 - x1); }

private:
  Sample x0 = 0;
  Sample x1 = 0;
};

/**
Highpass filter.

```python
import numpy
from scipy import signal
sos = signal.ellip(8, 0.1, 20, "high", output="sos", fs=48000)
```
*/
template<typename Sample> class DCSuppressor {
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

  Sample process(Sample input)
  {
    x0[0] = input;
    x0[1] = y0[0];
    x0[2] = y0[1];
    x0[3] = y0[2];

    y0[0] = co[0][0] * x0[0] + co[0][1] * x1[0] + co[0][2] * x2[0] - co[0][3] * y1[0]
      - co[0][4] * y2[0];
    y0[1] = co[1][0] * x0[1] + co[1][1] * x1[1] + co[1][2] * x2[1] - co[1][3] * y1[1]
      - co[1][4] * y2[1];
    y0[2] = co[2][0] * x0[2] + co[2][1] * x1[2] + co[2][2] * x2[2] - co[2][3] * y1[2]
      - co[2][4] * y2[2];
    y0[3] = co[3][0] * x0[3] + co[3][1] * x1[3] + co[3][2] * x2[3] - co[3][3] * y1[3]
      - co[3][4] * y2[3];

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
  const std::array<std::array<Sample, 5>, 4> co{{
    {0.9804280136255111, -1.9608560272510223, 0.9804280136255111, -1.9884538127692686,
     0.9885006110984004},
    {1.0, -2.0, 1.0, -1.9965590724667643, 0.996575511597267},
    {1.0, -2.0, 1.0, -1.998797993019254, 0.9988065671194735},
    {1.0, -2.0, 1.0, -1.999680409748494, 0.9996868172921567},
  }};
};

/**
Lowpass filter specialized for 8x oversampling.

```python
import numpy
from scipy import signal
sos = signal.ellip(16, 1, 120, 20000, "low", output="sos", fs=48000 * 4)
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
    {1.1068570976407285e-05, 1.491113217842306e-05, 1.1068570976407283e-05,
     -1.8048585670186443, 0.824285062631925},
    {1.0, -0.43358556254866565, 0.9999999999999999, -1.7631122489846536,
     0.8550517565421605},
    {1.0, -1.1063580953807355, 1.0, -1.7049833419461118, 0.8979805174518768},
    {1.0, -1.3542488185919934, 0.9999999999999999, -1.65401660574791, 0.935852303696926},
    {1.0, -1.4602698066659838, 0.9999999999999996, -1.6186587550899554,
     0.9626255473380627},
    {1.0, -1.5098016140893005, 0.9999999999999996, -1.5975881555029678,
     0.979584813584606},
    {1.0, -1.5332374658365886, 1.0000000000000002, -1.5870884218466406,
     0.9900443672832111},
    {1.0, -1.5427117975912512, 1.0000000000000002, -1.584500123075381, 0.997035057085821},
  }};
};

template<typename Sample> class HoldFilter {
public:
  //
  // `type` value mapping
  //
  // 0: Naive
  // 1: PolyBLEP Residual 4
  // 2: PolyBLEP Residual 6
  // 3: PolyBLEP Residual 8
  uint32_t type = 0;

  void reset()
  {
    xHold = yHold = 0;
    g0 = g1 = g2 = g3 = 0;
    t0 = t1 = t2 = t3 = 0;
    y0 = y1 = y2 = y3 = y4 = y5 = y6 = y7 = 0;
    phase = 1;
  }

  // Range of resonance is in (0, 1].
  void set(
    Sample sampleRate, Sample frequency, Sample resonance, Sample pulseWidth, Sample edge)
  {
    auto tk = frequency / sampleRate;
    tickHigh = tk * pulseWidth;
    tickLow = tk * (Sample(2) - pulseWidth);

    alpha = std::clamp<Sample>(resonance, Sample(1e-5), Sample(1));
    this->edge = edge;
  }

  Sample process(Sample x0)
  {
    switch (type) {
      default:
      case 0:
        break; // Naive.
      case 1:
        return process4(x0);
      case 2:
        return process6(x0);
      case 3:
        return process8(x0);
    }
    return processNaive(x0);
  }

  // Naive implementation. 0 sample latency.
  Sample processNaive(Sample x0)
  {
    if (phase >= Sample(1)) {
      auto edgeOut = x0 - edge * xHold;
      xHold = (x0 - xHold) * alpha;
      y0 = xHold / alpha;
      phase -= somefloor<Sample>(phase);
      tick = tick == tickLow ? tickHigh : tickLow;

      phase += tick;
      return edgeOut;
    }
    phase += tick;
    return y0;
  }

  // 4 sample latency.
  Sample process4(Sample x0)
  {
    g1 = g0;
    t1 = t0;

    if (phase >= Sample(1)) {
      xHold = (x0 - xHold) * alpha;

      Sample newHold = xHold / alpha;
      g0 = newHold - yHold;
      yHold = newHold;

      phase -= somefloor<Sample>(phase);
      t0 = phase * Sample(0.5) / tick;

      tick = tick == tickLow ? tickHigh : tickLow;
    } else {
      g0 = 0;
      t0 = 0;
    }
    phase += tick;

    y3 = y2;
    y2 = y1;
    y1 = y0;
    y0 = yHold;

    if (g1 != Sample(0)) {
      Sample t
        = std::clamp<Sample>(t1, Sample(0), Sample(1)); // Clamp to prevent loud output.

      Sample polyBlepResidual4_0 = -t * t * t * t / Sample(24) + t * t * t / Sample(6)
        - t * t / Sample(4) + t / Sample(6) - Sample(1) / Sample(24);
      Sample polyBlepResidual4_1 = t * t * t * t / Sample(8) - t * t * t / Sample(3)
        + Sample(2) * t / Sample(3) - Sample(1) / Sample(2);
      Sample polyBlepResidual4_2 = -t * t * t * t / Sample(8) + t * t * t / Sample(6)
        + t * t / Sample(4) + t / Sample(6) + Sample(1) / Sample(24);
      Sample polyBlepResidual4_3 = t * t * t * t / Sample(24);

      g1 *= edge;

      y0 += g1 * polyBlepResidual4_0;
      y1 += g1 * polyBlepResidual4_1;
      y2 += g1 * polyBlepResidual4_2;
      y3 += g1 * polyBlepResidual4_3;
    }

    return y3;
  }

  // 6 sample latency.
  Sample process6(Sample x0)
  {
    g2 = g1;
    g1 = g0;

    t2 = t1;
    t1 = t0;

    if (phase >= Sample(1)) {
      xHold = (x0 - xHold) * alpha;

      Sample newHold = xHold / alpha;
      g0 = newHold - yHold;
      yHold = newHold;

      phase -= somefloor<Sample>(phase);
      t0 = phase * Sample(0.5) / tick;

      tick = tick == tickLow ? tickHigh : tickLow;
    } else {
      g0 = 0;
      t0 = 0;
    }
    phase += tick;

    y5 = y4;
    y4 = y3;
    y3 = y2;
    y2 = y1;
    y1 = y0;
    y0 = yHold;

    if (g2 != Sample(0)) {
      Sample t
        = std::clamp<Sample>(t2, Sample(0), Sample(1)); // Clamp to prevent loud output.

      Sample polyBlepResidual6_0 = -t * t * t * t * t * t / Sample(720)
        + t * t * t * t * t / Sample(120) - t * t * t * t / Sample(48)
        + t * t * t / Sample(36) - t * t / Sample(48) + t / Sample(120)
        - Sample(1) / Sample(720);
      Sample polyBlepResidual6_1 = t * t * t * t * t * t / Sample(144)
        - t * t * t * t * t / Sample(30) + t * t * t * t / Sample(24)
        + t * t * t / Sample(18) - Sample(5) * t * t / Sample(24)
        + Sample(13) * t / Sample(60) - 29 / Sample(360);
      Sample polyBlepResidual6_2 = -t * t * t * t * t * t / Sample(72)
        + t * t * t * t * t / Sample(20) - t * t * t / Sample(6)
        + Sample(11) * t / Sample(20) - 1 / Sample(2);
      Sample polyBlepResidual6_3 = t * t * t * t * t * t / Sample(72)
        - t * t * t * t * t / Sample(30) - t * t * t * t / Sample(24)
        + t * t * t / Sample(18) + (5 * t * t) / Sample(24) + Sample(13) * t / Sample(60)
        + Sample(29) / Sample(360);
      Sample polyBlepResidual6_4 = -t * t * t * t * t * t / Sample(144)
        + t * t * t * t * t / Sample(120) + t * t * t * t / Sample(48)
        + t * t * t / Sample(36) + t * t / Sample(48) + t / Sample(120)
        + Sample(1) / Sample(720);
      Sample polyBlepResidual6_5 = t * t * t * t * t * t / Sample(720);

      g2 *= edge;

      y0 += g2 * polyBlepResidual6_0;
      y1 += g2 * polyBlepResidual6_1;
      y2 += g2 * polyBlepResidual6_2;
      y3 += g2 * polyBlepResidual6_3;
      y4 += g2 * polyBlepResidual6_4;
      y5 += g2 * polyBlepResidual6_5;
    }

    return y5;
  }

  // 8 sample latency.
  Sample process8(Sample x0)
  {
    g3 = g2;
    g2 = g1;
    g1 = g0;

    t3 = t2;
    t2 = t1;
    t1 = t0;

    if (phase >= Sample(1)) {
      xHold = (x0 - xHold) * alpha;

      Sample newHold = xHold / alpha;
      g0 = newHold - yHold;
      yHold = newHold;

      phase -= somefloor<Sample>(phase);
      t0 = phase * Sample(0.5) / tick;

      tick = tick == tickLow ? tickHigh : tickLow;
    } else {
      g0 = 0;
      t0 = 0;
    }
    phase += tick;

    y7 = y6;
    y6 = y5;
    y5 = y4;
    y4 = y3;
    y3 = y2;
    y2 = y1;
    y1 = y0;
    y0 = yHold;

    if (g3 != Sample(0)) {
      Sample t
        = std::clamp<Sample>(t3, Sample(0), Sample(1)); // Clamp to prevent loud output.

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
        - t * t * t * t * t / Sample(80) + (19 * t * t * t * t) / Sample(576)
        + t * t * t / Sample(48) - Sample(49) * t * t / Sample(288)
        + Sample(397) * t / Sample(1680) - Sample(4541) / Sample(40320);
      Sample polyBlepResidual8_3 = t * t * t * t * t * t * t * t / Sample(1152)
        - t * t * t * t * t * t * t / Sample(252) + t * t * t * t * t / Sample(45)
        - t * t * t / Sample(9) + Sample(151) * t / Sample(315) - Sample(1) / Sample(2);
      Sample polyBlepResidual8_4 = -t * t * t * t * t * t * t * t / Sample(1152)
        + t * t * t * t * t * t * t / Sample(336) + t * t * t * t * t * t / Sample(288)
        - t * t * t * t * t / Sample(80) - (19 * t * t * t * t) / Sample(576)
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

      g3 *= edge;

      y0 += g3 * polyBlepResidual8_0;
      y1 += g3 * polyBlepResidual8_1;
      y2 += g3 * polyBlepResidual8_2;
      y3 += g3 * polyBlepResidual8_3;
      y4 += g3 * polyBlepResidual8_4;
      y5 += g3 * polyBlepResidual8_5;
      y6 += g3 * polyBlepResidual8_6;
      y7 += g3 * polyBlepResidual8_7;
    }

    return y7;
  }

private:
  Sample xHold = 0;
  Sample yHold = 0;

  Sample g0 = 0; // g for Gain of PolyBLEP residual.
  Sample g1 = 0;
  Sample g2 = 0;
  Sample g3 = 0;

  Sample t0 = 0; // t for time of PolyBLEP residual.
  Sample t1 = 0;
  Sample t2 = 0;
  Sample t3 = 0;

  Sample y0 = 0;
  Sample y1 = 0;
  Sample y2 = 0;
  Sample y3 = 0;
  Sample y4 = 0;
  Sample y5 = 0;
  Sample y6 = 0;
  Sample y7 = 0;

  Sample alpha = 1e-5;
  Sample edge = 1;
  Sample phase = 0;
  Sample tick = 0;
  Sample tickHigh = 0;
  Sample tickLow = 0;
};

} // namespace SomeDSP
