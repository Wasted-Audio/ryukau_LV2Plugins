// (c) 2020 Takamitsu Endo
//
// This file is part of CV_RampFilter.
//
// CV_RampFilter is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_RampFilter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_RampFilter.  If not, see <https://www.gnu.org/licenses/>.

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

    y0[0] = co[0][0] * x0[0] + co[0][1] * x1[0] + co[0][2] * x2[0] - co[0][3] * y1[0]
      - co[0][4] * y2[0];
    y0[1] = co[1][0] * x0[1] + co[1][1] * x1[1] + co[1][2] * x2[1] - co[1][3] * y1[1]
      - co[1][4] * y2[1];

    x2[0] = x1[0];
    x2[1] = x1[1];

    x1[0] = x0[0];
    x1[1] = x0[1];

    y2[0] = y1[0];
    y2[1] = y1[1];

    y1[0] = y0[0];
    y1[1] = y0[1];

    return y0[1];
  }

  std::array<Sample, 2> x0{};
  std::array<Sample, 2> x1{};
  std::array<Sample, 2> x2{};
  std::array<Sample, 2> y0{};
  std::array<Sample, 2> y1{};
  std::array<Sample, 2> y2{};
  const std::array<std::array<Sample, 5>, 8> co{{
    {0.9991452220522873, -1.9982904441045746, 0.9991452220522873, -1.9987909473252388,
     0.9987913754346018},
    {1.0, -2.0, 1.0, -1.9994987657679466, 0.9994991940289132},
  }};
};

template<typename Sample> class RampFilter {
public:
  bool highpass = false;

  void reset()
  {
    dcSuppressor.reset();
    phase = 1;
    ramp = 0;
    y0 = hold = 0;
  }

  void curveFunc(Sample bias, Sample &x, Sample &y)
  {
    if (bias >= Sample(1)) {
      x = Sample(1);
      y = Sample(1);
    } else if (bias > Sample(0.25)) {
      x = bias - Sample(0.25);

      y = -0.0646320567764957
        + (0.19217237645832694 + 2.0646794630244787 * x - 2.9810338484383716 * x * x)
          / (0.007084872235402778 + 0.7426715942426754 * x + 1.9274172346660643 * x * x);
      y = somepow<Sample>(Sample(2), y);
    } else {
      x = bias;
      y = Sample(1) / (Sample(1) - bias);
    }
  }

  // Range of resonance is in [0, 1].
  void set(
    Sample sampleRate,
    Sample frequency,
    Sample resonance,
    Sample rampLimit,
    Sample bias,
    Sample biasTuning)
  {
    tick = frequency / sampleRate;
    alpha = resonance;

    if (bias >= 0) {
      curveFunc(Sample(1) - bias, rise, fall);
      fall *= biasTuning;
    } else {
      curveFunc(Sample(1) + bias, fall, rise);
      rise *= biasTuning;
    }

    Sample absRise = somefabs<Sample>(rise);
    Sample absFall = somefabs<Sample>(fall);
    gain = std::min(absRise, absFall) / std::max(absRise, absFall);
    limit = rampLimit / gain;
  }

  Sample process(Sample x0)
  {
    if (phase >= Sample(1)) {
      ramp = (x0 - hold) * tick * alpha;
      if (ramp >= 0)
        ramp = std::min<Sample>(ramp * rise, limit);
      else
        ramp = std::max<Sample>(ramp * fall, -limit);

      hold = y0;
      phase -= somefloor<Sample>(phase);
    }
    phase += tick;
    y0 += ramp;
    return highpass ? dcSuppressor.process(y0 * gain) : y0 * gain;
  }

private:
  DCSuppressor<Sample> dcSuppressor;

  Sample alpha = 1e-5;
  Sample rise = 1;
  Sample fall = 1;
  Sample gain = 1;

  Sample phase = 1;
  Sample tick = 0;

  Sample ramp = 0;
  Sample limit = 1;

  Sample hold = 0;
  Sample y0 = 0;
};

// 4 sample latency.
template<typename Sample> class RampFilterPolyBLAMP4 {
public:
  void reset()
  {
    hold = 0;
    y0 = y1 = y2 = y3 = 0;
    r0 = r1 = 0;
    t0 = t1 = 0;
    ramp = lastRamp = 0;
    phase = 0;
  }

  void set(Sample sampleRate, Sample frequency, Sample resonance, Sample bias)
  {
    tick = frequency / sampleRate;
    alpha = resonance;

    // TODO: set rise and fall from bias.

    Sample absRise = somefabs<Sample>(rise);
    Sample absFall = somefabs<Sample>(fall);
    gain = std::min(absRise, absFall) / std::max(absRise, absFall);
  }

  Sample process(Sample x0)
  {
    r1 = r0;
    t1 = t0;

    if (phase >= Sample(1)) {
      ramp = (x0 - hold) * tick * alpha;
      ramp *= ramp >= 0 ? rise : fall;

      hold = y0;

      r0 = ramp;

      phase -= somefloor<Sample>(phase);
      t0 = phase / tick;
    } else {
      t0 = Sample(-1);
      r0 = Sample(0);
    }
    phase += tick;

    y3 = y2;
    y2 = y1;
    y1 = y0;
    y0 += ramp;

    if (t1 >= Sample(0)) {
      Sample t = std::clamp<Sample>(t1, Sample(0), Sample(1));

      y0 += r1 * polyBlampResidual4_0(t);
      y1 += r1 * polyBlampResidual4_1(t);
      y2 += r1 * polyBlampResidual4_2(t);
      y3 += r1 * polyBlampResidual4_3(t);

      y0 -= lastRamp * polyBlampResidual4_3(1 - t);
      y1 -= lastRamp * polyBlampResidual4_2(1 - t);
      y2 -= lastRamp * polyBlampResidual4_1(1 - t);
      y3 -= lastRamp * polyBlampResidual4_0(1 - t);

      lastRamp = r1;
    }

    return y3 * gain;
  }

  inline Sample polyBlampResidual4_0(Sample t)
  {
    return -t * t * t * t * t / Sample(120) + t * t * t * t / Sample(24)
      - t * t * t / Sample(12) + t * t / Sample(12) - t / Sample(24)
      + Sample(1) / Sample(120);
  }

  inline Sample polyBlampResidual4_1(Sample t)
  {
    return t * t * t * t * t / Sample(40) - t * t * t * t / Sample(12) + t * t / Sample(3)
      - t / Sample(2) + Sample(7) / Sample(30);
  }

  inline Sample polyBlampResidual4_2(Sample t)
  {
    return -t * t * t * t * t / Sample(40) + t * t * t * t / Sample(24)
      + t * t * t / Sample(12) + t * t / Sample(12) + t / Sample(24)
      + Sample(1) / Sample(120);
  }

  inline Sample polyBlampResidual4_3(Sample t) { return t * t * t * t * t / Sample(120); }

private:
  Sample hold = 0;

  Sample y0 = 0;
  Sample y1 = 0;
  Sample y2 = 0;
  Sample y3 = 0;

  Sample r0 = 0;
  Sample r1 = 0;
  Sample ramp = 0;
  Sample lastRamp = 0;

  Sample t0 = 0;
  Sample t1 = 0;

  Sample rise = 1;
  Sample fall = 1;
  Sample gain = 1;

  Sample alpha = 1e-5;
  Sample phase = 0;
  Sample tick = 0;
};

} // namespace SomeDSP
