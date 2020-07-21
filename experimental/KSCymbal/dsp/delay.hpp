// (c) 2020 Takamitsu Endo
//
// This file is part of KSCymbal.
//
// KSCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KSCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KSCymbal.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"

#include <cfloat>
#include <random>

namespace SomeDSP {

// Convert `tau` in seconds to 1 pole lowpass filter coefficient.
inline float tau2pole(float sampleRate, float tau)
{
  if (fabsf(tau) < FLT_EPSILON) return 0;
  return expf(-1.0f / (tau * sampleRate));
}

struct InternalLimiter {
  float limit = 1; // Must be positive.
  float clip = 1;  // Must be positive.
  float pole = 1;
  float amp = 1;

  void prepare(float sampleRate, float limit, float clip, float time)
  {
    this->limit = limit;
    this->clip = clip;
    pole = tau2pole(sampleRate, time);
  }

  void reset(float amp = 1) { this->amp = amp; }

  float process(float input)
  {
    auto absed = somefabs(input);
    auto target = absed > limit ? limit / absed : limit;
    amp += pole * (target - amp);
    return std::clamp<float>(input * amp, -clip, clip);
  }
};

template<typename Sample> class AttackGate {
public:
  void reset(Sample sampleRate, Sample seconds)
  {
    value = 0;
    ramp = Sample(1) / (sampleRate * seconds);

    lowpass.reset();
    lowpass.setCutoff(sampleRate, Sample(1) / seconds);
  }

  Sample process()
  {
    value += ramp;
    return lowpass.process(value >= Sample(1) ? Sample(1) : value);
  }

private:
  PController<Sample> lowpass;
  Sample value = 0;
  Sample ramp = 0;
};

// https://ccrma.stanford.edu/~jos/filters/One_Zero.html
template<typename Sample> struct OneZeroLP {
  Sample x1 = 0;
  Sample kp = 0.5;

  void reset() { x1 = 0; }

  Sample process(Sample x0)
  {
    Sample out = kp * (x0 - x1) + x1;
    x1 = x0;
    return out;
  }
};

template<typename Sample> struct RCHP {
  Sample kp = 0.5;
  Sample y = 0;
  Sample x1 = 0;

  void reset() { y = x1 = 0; }

  Sample process(Sample x0)
  {
    y = kp * (y + x0 - x1); // y0 = kp * (y1 + x0 - x1).
    x1 = x0;
    return y;
  }
};

// https://www.earlevel.com/main/2012/12/15/a-one-pole-filter/
template<typename Sample> struct OnePoleHighpass {
  Sample b1 = 0;
  Sample z1 = 0;

  void setCutoff(Sample sampleRate, Sample cutoffHz)
  {
    b1 = exp(-twopi * cutoffHz / sampleRate); // Use double.
  }

  void reset() { z1 = 0; }
  Sample process(Sample input)
  {
    z1 = input * (Sample(1) - b1) + z1 * b1;
    return input - z1;
  }
};

template<typename Sample> class ShortComb {
public:
  std::array<Sample, 512> buf; // At least 20ms when samplerate is 192kHz.
  int wptr = 0;
  int rptr = 0;
  Sample r1 = 0;

  void reset()
  {
    r1 = 0;
    buf.fill(0);
  }

  void setTime(Sample sampleRate, Sample seconds)
  {
    rptr = wptr - std::clamp<int>(sampleRate * seconds, 0, buf.size());
    if (rptr < 0) rptr += int(buf.size());
  }

  Sample process(Sample input)
  {
    input -= Sample(0.3) * r1;

    ++wptr;
    wptr &= 511;
    buf[wptr] = input;

    ++rptr;
    rptr &= 511;
    return r1 = buf[rptr];
  }
};

template<typename Sample> class NaiveDelay {
public:
  std::array<Sample, 16384> buf; // Min ~11.72Hz when samplerate is 192kHz.
  int wptr = 0;
  int rptr = 0;

  void reset() { buf.fill(0); }

  void setTime(Sample sampleRate, Sample seconds)
  {
    rptr = wptr - std::clamp<int>(sampleRate * seconds, 0, buf.size());
    if (rptr < 0) rptr += int(buf.size());
  }

  Sample process(Sample input)
  {
    ++wptr;
    wptr &= 16383; // 16383 = 2^14 - 1. 0x3fff.
    buf[wptr] = input;

    ++rptr;
    rptr &= 16383;
    return buf[rptr];
  }
};

template<typename Sample> class KsString {
public:
  NaiveDelay<Sample> delay;
  PController<Sample> lowpass;
  OnePoleHighpass<Sample> highpass;
  Sample feedback = 0;

  void setup(Sample sampleRate)
  {
    lowpass.setCutoff(sampleRate, 1000);
    highpass.setCutoff(sampleRate, 20);
  }

  void reset()
  {
    delay.reset();
    lowpass.reset();
    highpass.reset();
    feedback = 0;
  }

  Sample process(Sample in)
  {
    Sample out = delay.process(in + feedback);
    feedback = lowpass.process(out);
    return highpass.process(out);
  }
};

template<typename Sample, uint16_t size> class KsHat {
public:
  std::array<KsString<Sample>, size> string;
  std::array<Sample, size> buf{};
  Sample distance = 1;

  void setup(Sample sampleRate)
  {
    for (auto &str : string) str.setup(sampleRate);
  }

  void reset()
  {
    for (auto &str : string) str.reset();
    buf.fill(0);
  }

  void trigger(Sample distance)
  {
    this->distance = distance;
    reset();
  }

  Sample process(Sample input)
  {
    Sample out = 0;
    for (uint16_t idx = 0; idx < size; ++idx) {
      Sample dist = (idx < 1) ? distance : distance - buf[idx - 1];
      Sample leftover = (input <= dist) ? 0 : input - dist;
      input -= Sample(0.9) * leftover;
      buf[idx] = string[idx].process(input);
      out += buf[idx];
    }
    return out / size;
  }
};

} // namespace SomeDSP
