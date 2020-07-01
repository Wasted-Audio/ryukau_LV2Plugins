// (c) 2020 Takamitsu Endo
//
// This file is part of BubbleSynth.
//
// BubbleSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BubbleSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BubbleSynth.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/smoother.hpp"

namespace SomeDSP {

template<typename Sample> class IntDelay {
public:
  int wptr = 0; // Write pointer.
  int rptr = 0; // Read pointer.
  int size = 4;
  std::vector<Sample> buf;

  void setup(Sample sampleRate, Sample maxTime)
  {
    size = int(sampleRate * maxTime) + 1;
    size = size < 0 ? 4 : size;
    buf.resize(size);

    reset();
  }

  void reset() { std::fill(buf.begin(), buf.end(), 0); }

  void setTime(Sample sampleRate, Sample seconds)
  {
    rptr = wptr - std::clamp<int>(sampleRate * seconds, 0, size);
    if (rptr < 0) rptr += size;
  }

  Sample process(Sample input)
  {
    buf[wptr] = input;
    ++wptr;
    if (wptr >= size) wptr -= size;

    const size_t i0 = rptr;
    ++rptr;
    if (rptr >= size) rptr -= size;

    return buf[i0];
  }
};

template<typename Sample> inline void trimNoteFreq(Sample &noteFreq)
{
  if (somefabs(noteFreq) < Sample(0.001)) noteFreq = Sample(0.001);
}

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

template<typename Sample> class BubbleOsc {
public:
  void setup(Sample sampleRate) { delay.setup(sampleRate, Sample(0.1)); }

  void reset() { delay.reset(); }

  void prepare(
    Sample sampleRate,
    Sample gain,
    Sample radius,
    Sample xi,
    Sample attack,
    Sample decayRatio,
    Sample delaySeconds)
  {
    time = 0;
    tick = Sample(1) / sampleRate;

    auto dd = (Sample(0.13) + Sample(0.0072) / somesqrt(radius)) / radius;
    xi_d = xi * dd;
    c0 = Sample(6 * pi) / radius;
    decay = someexp(-decayRatio * dd / sampleRate);
    this->gain = gain;

    gate.reset(sampleRate, attack);
    delay.setTime(sampleRate, delaySeconds);
  }

  Sample process()
  {
    auto out = gate.process() * gain * somesin(c0 * time * (Sample(1) + xi_d * time));
    time += tick;
    gain *= decay;
    return delay.process(out);
  }

private:
  IntDelay<Sample> delay;
  AttackGate<Sample> gate;
  Sample time = 0;
  Sample tick = 0;
  Sample xi_d = 0;
  Sample c0 = 0;
  Sample gain = 1;
  Sample decay = 0;
};

} // namespace SomeDSP
