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

template<typename Sample> struct EasyCompressor {
  PController<float> smoother;
  Sample threshold = 0.1;
  Sample targetAmp = 1;
  Sample inHold = 0;
  int32_t counter = 0;
  int32_t holdFrames = 100;

  void prepare(Sample sampleRate, Sample holdSeconds, Sample threshold)
  {
    smoother.setCutoff(sampleRate, Sample(2) / holdSeconds);
    holdFrames = int32_t(sampleRate * holdSeconds);
    this->threshold = threshold;
  }

  void reset()
  {
    smoother.reset(Sample(1));
    targetAmp = Sample(1);
    inHold = 0;
    counter = 0;
  }

  Sample process(Sample input)
  {
    auto inAbs = somefabs(input);

    if (inAbs > inHold) {
      inHold = inAbs;
      counter = holdFrames;
      targetAmp = threshold / inAbs;
    }

    if (counter <= 0) {
      inHold = threshold;
      targetAmp *= Sample(1.01);
      if (targetAmp > Sample(1)) targetAmp = Sample(1);
    } else {
      --counter;
    }

    return smoother.process(targetAmp) * input;
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

template<typename Sample> class HighpassReleaseGate {
public:
  void reset(Sample sampleRate, Sample seconds)
  {
    rel = false;
    value = threshold;
    set(sampleRate, seconds);
  }

  void set(Sample sampleRate, Sample seconds)
  {
    alpha = std::pow(Sample(1) / threshold, Sample(1) / (seconds * sampleRate));
  }

  void release() { rel = true; }
  bool isReleasing() { return rel; }
  bool isTerminated() { return value >= Sample(1); }

  Sample process()
  {
    if (!rel) return Sample(0);
    value *= alpha;
    if (value >= Sample(1)) return Sample(1 - threshold);
    return value - threshold;
  }

protected:
  bool rel = false;
  const Sample threshold = 1e-5;
  Sample value = 0;
  Sample alpha = 0;
};

// https://www.earlevel.com/main/2012/12/15/a-one-pole-filter/
template<typename Sample> struct OnePoleHighpass {
  Sample z1 = 0;

  // Used to set b1 in process().
  static Sample setCutoff(Sample sampleRate, Sample cutoffHz)
  {
    return exp(-twopi * cutoffHz / sampleRate); // Use double.
  }

  void reset() { z1 = 0; }

  Sample process(Sample input, Sample b1)
  {
    z1 = input * (Sample(1) - b1) + z1 * b1;
    return input - z1;
  }
};

template<typename Sample> struct PControllerKSHat {
  Sample value = 0;

  void reset(Sample value = 0) { this->value = value; }
  Sample process(Sample input, Sample kp) { return value += kp * (input - value); }
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

// 2x oversampled delay.
template<typename Sample> class Delay {
public:
  std::array<Sample, 32768> buf; // Min ~11.72Hz when samplerate is 192kHz.
  Sample w1 = 0;
  Sample rFraction = 0;
  int wptr = 0;
  int rptr = 0;

  void reset()
  {
    w1 = 0;
    buf.fill(0);
  }

  void setTime(Sample sampleRate, Sample seconds)
  {
    Sample timeInSample
      = std::clamp<Sample>(Sample(2) * sampleRate * seconds, 0, buf.size());
    auto timeInt = int(timeInSample);

    rFraction = timeInSample - Sample(timeInt);

    rptr = wptr - timeInt;
    if (rptr < 0) rptr += int(buf.size());
  }

  Sample process(Sample input)
  {
    // Write to buffer.
    ++wptr;
    wptr &= 32767; // 32767 = 2^15 - 1. 0x7fff.
    buf[wptr] = Sample(0.5) * (input + w1);

    ++wptr;
    wptr &= 32767; // 32767 = 2^15 - 1. 0x7fff.
    buf[wptr] = input;

    w1 = input;

    // Read from buffer.
    ++rptr;
    rptr &= 32767;
    const unsigned int i1 = rptr;

    ++rptr;
    rptr &= 32767;
    const unsigned int i0 = rptr;

    return buf[i0] - rFraction * (buf[i0] - buf[i1]);
  }
};

template<typename Sample> class KsString {
public:
  Delay<Sample> delay;
  PControllerKSHat<Sample> lowpass;
  OnePoleHighpass<Sample> highpass;
  Sample feedback = 0;

  void setup(Sample sampleRate) { highpass.setCutoff(sampleRate, 20); }

  void reset()
  {
    delay.reset();
    lowpass.reset();
    highpass.reset();
    feedback = 0;
  }

  Sample process(Sample in, Sample kp, Sample b1)
  {
    Sample out = delay.process(in + feedback);
    feedback = lowpass.process(out, kp);
    return highpass.process(out, b1);
  }
};

template<typename Sample, uint16_t size> class KsHat {
public:
  std::array<KsString<Sample>, size> string;
  std::array<Sample, size> buf{};
  Sample distance = 1;

  Sample kp = 0; // Lowpass coefficient.
  Sample b1 = 1; // Highpass coefficient.

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
      buf[idx] = string[idx].process(input, kp, b1);
      out += buf[idx];
    }
    return out / size;
  }
};

} // namespace SomeDSP
