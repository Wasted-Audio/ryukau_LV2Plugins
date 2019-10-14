// (c) 2019 Takamitsu Endo
//
// This file is part of SyncSawSynth.
//
// SyncSawSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SyncSawSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SyncSawSynth.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <array>
#include <memory>

#include "constants.hpp"
#include "somemath.hpp"

namespace SomeDSP {

enum class BiquadType {
  lowpass,
  highpass,
  bandpass,
  notch,
};

enum class ShaperType { hardclip, tanh, sinRunge, cubicExpDecayAbs };

template<typename Sample, size_t nFilter> class SerialFilter {
public:
  BiquadType type = BiquadType::lowpass;
  Sample fs;
  Sample f0;
  Sample q;

  Sample b0 = 0.0;
  Sample b1 = 0.0;
  Sample b2 = 0.0;
  Sample a0 = 0.0;
  Sample a1 = 0.0;
  Sample a2 = 0.0;

  Sample w0 = 0.0;
  Sample cos_w0 = 0.0;
  Sample sin_w0 = 0.0;
  Sample alpha = 0.0;

  std::array<Sample, nFilter> x1{};
  std::array<Sample, nFilter> x2{};
  std::array<Sample, nFilter> y1{};
  std::array<Sample, nFilter> y2{};

  Sample output = 0.0;
  Sample feedback = 0.0;
  Sample saturation = 1.0;
  ShaperType shaper = ShaperType::sinRunge;

  SerialFilter(Sample sampleRate, Sample cutoff, Sample resonance)
    : fs(sampleRate), f0(cutoff), q(resonance)
  {
  }

  void reset()
  {
    b0 = b1 = b2 = 0.0f;
    a0 = a1 = a2 = 0.0f;
    clear();
  }

  void clear()
  {
    output = 0.0;
    feedback = 0.0;
    x1.fill(0);
    x2.fill(0);
    y1.fill(0);
    y2.fill(0);
  }

  Sample clamp(Sample value, Sample min, Sample max)
  {
    return value < min ? min : value > max ? max : value;
  }

  void setLowpass()
  {
    alpha = sin_w0 / (Sample(2.0) * q);
    b0 = (Sample(1.0) - cos_w0) / Sample(2.0);
    b1 = Sample(1.0) - cos_w0;
    b2 = (Sample(1.0) - cos_w0) / Sample(2.0);
    a0 = Sample(1.0) + alpha;
    a1 = -Sample(2.0) * cos_w0;
    a2 = Sample(1.0) - alpha;
  }

  void setHighpass()
  {
    alpha = sin_w0 / (Sample(2.0) * q);
    b0 = (Sample(1.0) + cos_w0) / Sample(2.0);
    b1 = -(Sample(1.0) + cos_w0);
    b2 = (Sample(1.0) + cos_w0) / Sample(2.0);
    a0 = Sample(1.0) + alpha;
    a1 = -Sample(2.0) * cos_w0;
    a2 = Sample(1.0) - alpha;
  }

  void setBandpass()
  {
    // 0.34657359027997264 = log(2) / 2.
    alpha = sin_w0 * somesinh<Sample>(0.34657359027997264 * q * w0 / sin_w0);
    b0 = alpha;
    b1 = 0.0;
    b2 = -alpha;
    a0 = Sample(1.0) + alpha;
    a1 = -Sample(2.0) * cos_w0;
    a2 = Sample(1.0) - alpha;
  }

  void setNotch()
  {
    alpha = sin_w0 * somesinh<Sample>(0.34657359027997264 * q * w0 / sin_w0);
    b0 = Sample(1.0);
    b1 = -Sample(2.0) * cos_w0;
    b2 = Sample(1.0);
    a0 = Sample(1.0) + alpha;
    a1 = -Sample(2.0) * cos_w0;
    a2 = Sample(1.0) - alpha;
  }

  void setCutoffQ(Sample hz, Sample q)
  {
    f0 = clamp(hz, Sample(20.0), Sample(20000.0));
    this->q = clamp(q, Sample(1e-5), Sample(1.0));

    w0 = twopi * f0 / fs;
    cos_w0 = somecos<Sample>(w0);
    sin_w0 = somesin<Sample>(w0);
    switch (type) {
      default:
      case BiquadType::lowpass:
        setLowpass();
        break;

      case BiquadType::highpass:
        setHighpass();
        break;

      case BiquadType::bandpass:
        setBandpass();
        break;

      case BiquadType::notch:
        setNotch();
        break;
    }
  }

  Sample shaperSinRunge(Sample x)
  {
    return somesin<Sample>(Sample(2.0 * pi) * x) / (Sample(1.0) + Sample(10.0) * x * x);
  }

  Sample shaperCubicExpDecayAbs(Sample x)
  {
    // Solve x for: diff(x^3*exp(-x), x) = 0,
    // then we get: x = 0, 27 * math.exp(-3).
    // 0.7439087749328765 = 1 / (27 * math.exp(-3))
    return 0.7439087749328765 * x * x * x * someexp<Sample>(-somefabs<Sample>(x));
  }

  Sample shaperHardclip(Sample x)
  {
    return x < Sample(-1.0) ? Sample(-1.0) : x > Sample(1.0) ? Sample(1.0) : x;
  }

  Sample process(Sample input)
  {
    input = saturation * (input - feedback * output);
    switch (shaper) {
      default:
      case ShaperType::hardclip:
        input = shaperHardclip(input);
        break;

      case ShaperType::tanh:
        input = sometanh<Sample>(input);
        break;

      case ShaperType::sinRunge:
        input = shaperSinRunge(input);
        break;

      case ShaperType::cubicExpDecayAbs:
        input = shaperCubicExpDecayAbs(input);
        break;
    }

    for (size_t i = 0; i < nFilter; ++i) {
      output = (b0 * input + b1 * x1[i] + b2 * x2[i] - a1 * y1[i] - a2 * y2[i]) / a0;

      x2[i] = x1[i];
      x1[i] = input;

      y2[i] = y1[i];
      y1[i] = output;

      input = output;
    }

    if (std::isfinite(output)) return output;
    clear();
    return 0.0;
  }
};

} // namespace SomeDSP
