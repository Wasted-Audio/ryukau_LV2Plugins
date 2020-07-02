// (c) 2020 Takamitsu Endo
//
// This file is part of L4Reverb.
//
// L4Reverb is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// L4Reverb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with L4Reverb.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../../../lib/vcl/vectormath_hyp.h"

#include <algorithm>
#include <array>
#include <climits>
#include <numeric>
#include <vector>

namespace SomeDSP {

// Approximation of `440 * powf(2, (x * 120 - 69) / 12);`.
// x in [0.0, 1.0].
inline float mapCutoff(float x)
{
  return (float(2.358608708691953) + float(12.017595467921483) * x
          + float(12.200543970909193) * x * x + float(65.1589512791118) * x * x * x)
    / (float(0.2835017917034045) + float(-0.6282361955447577) * x
       + float(0.4872433705867005) * x * x + float(-0.13155292689641543) * x * x * x);
}

template<typename Sample> class Biquad {
public:
  ExpSmootherLocal<Sample> b0;
  ExpSmootherLocal<Sample> b1;
  ExpSmootherLocal<Sample> b2;
  ExpSmootherLocal<Sample> a1;
  ExpSmootherLocal<Sample> a2;

  Sample x1 = 0;
  Sample x2 = 0;
  Sample y1 = 0;
  Sample y2 = 0;

  void reset()
  {
    b0.reset(0);
    b1.reset(0);
    b2.reset(0);
    a1.reset(0);
    a2.reset(0);
    x1 = x2 = 0;
    y1 = y2 = 0;
  }

  void setSmoothTime(Sample sampleRate, Sample seconds)
  {
    auto kp = std::clamp<Sample>(
      PController<double>::cutoffToP(sampleRate, Sample(1) / seconds), 0, 1);
    b0.kp = kp;
    b1.kp = kp;
    b2.kp = kp;
    a1.kp = kp;
    a2.kp = kp;
  }

  void setLowpass(Sample sampleRate, Sample cutoffHz, Sample resonance, bool push = true)
  {
    cutoffHz = std::clamp(cutoffHz, Sample(20), Sample(20000));
    resonance = std::clamp(resonance, Sample(1e-5), Sample(1));

    Sample omega = twopi * cutoffHz / sampleRate;
    Sample cos_omega = somecos(omega);
    Sample alpha = somesin(omega) / (Sample(2) * resonance);
    Sample a0 = Sample(1) + alpha;

    if (push) {
      b0.push((Sample(1) - cos_omega) / Sample(2) / a0);
      b1.push((Sample(1) - cos_omega) / a0);
      b2.push((Sample(1) - cos_omega) / Sample(2) / a0);
      a1.push((-Sample(2) * cos_omega) / a0);
      a2.push((Sample(1) - alpha) / a0);
    } else {
      b0.reset((Sample(1) - cos_omega) / Sample(2) / a0);
      b1.reset((Sample(1) - cos_omega) / a0);
      b2.reset((Sample(1) - cos_omega) / Sample(2) / a0);
      a1.reset((-Sample(2) * cos_omega) / a0);
      a2.reset((Sample(1) - alpha) / a0);
    }
  }

  void setHighpass(Sample sampleRate, Sample cutoffHz, Sample resonance, bool push = true)
  {
    cutoffHz = std::clamp(cutoffHz, Sample(20), Sample(20000));
    resonance = std::clamp(resonance, Sample(1e-5), Sample(1));

    Sample omega = twopi * cutoffHz / sampleRate;
    Sample cos_omega = somecos(omega);
    Sample alpha = somesin(omega) / (Sample(2) * resonance);
    Sample a0 = Sample(1) + alpha;

    if (push) {
      b0.push((Sample(1) + cos_omega) / Sample(2) / a0);
      b1.push(-(Sample(1) + cos_omega) / a0);
      b2.push((Sample(1) + cos_omega) / Sample(2) / a0);
      a1.push((-Sample(2) * cos_omega) / a0);
      a2.push((Sample(1) - alpha) / a0);
    } else {
      b0.reset((Sample(1) + cos_omega) / Sample(2) / a0);
      b1.reset(-(Sample(1) + cos_omega) / a0);
      b2.reset((Sample(1) + cos_omega) / Sample(2) / a0);
      a1.reset((-Sample(2) * cos_omega) / a0);
      a2.reset((Sample(1) - alpha) / a0);
    }
  }

  Sample process(Sample x0)
  {
    Sample y0 = b0.process() * x0 + b1.process() * x1 + b2.process() * x2
      - a1.process() * y1 - a2.process() * y2;

    x2 = x1;
    x1 = x0;
    y2 = y1;
    y1 = y0;

    return y0;
  }
};

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

template<typename Sample> class ExpDecayCurve {
public:
  void reset(Sample sampleRate, Sample seconds)
  {
    value = Sample(1);
    set(sampleRate, seconds);
  }

  void set(Sample sampleRate, Sample seconds)
  {
    alpha = somepow(threshold, Sample(1) / (seconds * sampleRate));
  }

  bool isTerminated() { return value <= threshold; }

  Sample process()
  {
    if (value <= threshold) return Sample(0);
    value *= alpha;
    return value - threshold;
  }

protected:
  constexpr static Sample threshold = 1e-5;
  Sample value = 0;
  Sample alpha = 0;
};

struct alignas(64) GaussianNoise {
  Vec16ui buffer{0};

  GaussianNoise(uint32_t seed = 0) { setSeed(seed); }

  void setSeed(uint32_t seed)
  {
    for (int idx = 0; idx < 16; ++idx) {
      seed = 1664525L * seed + 1013904223L;
      buffer.insert(idx, seed);
    }
  }

  float process()
  {
    buffer = 1664525L * buffer + 1013904223L;
    return horizontal_add(to_float(buffer) / float(UINT32_MAX)) / 8.0f - 1.0f;
  }
};

template<typename Sample> class NoiseAD {
public:
  GaussianNoise noise;
  Biquad<float> lowpass;
  AttackGate<float> attack;
  ExpDecayCurve<float> decay;

  void prepare(Sample sampleRate, Sample attackSeconds, Sample decaySeconds)
  {
    attack.reset(sampleRate, attackSeconds);
    decay.reset(sampleRate, decaySeconds);

    lowpass.setLowpass(sampleRate, 20000.0f, 0.5f, false);
    lowpass.setLowpass(sampleRate, 20.0f, 0.5f, true);
    lowpass.setSmoothTime(sampleRate, decaySeconds);
  }

  Sample process()
  {
    return attack.process() * decay.process() * lowpass.process(noise.process());
  }
};

// 2x oversampled delay.
template<typename Sample> class Delay {
public:
  Sample w1 = 0;
  Sample rFraction = 0.0;
  int wptr = 0;
  int rptr = 0;
  int size = 0;
  std::vector<Sample> buf;

  void setup(Sample sampleRate, Sample maxTime)
  {
    size = int(Sample(2) * sampleRate * maxTime) + 1;
    if (size < 4) size = 4;

    buf.resize(size);
  }

  void reset()
  {
    w1 = 0;
    std::fill(buf.begin(), buf.end(), 0);
  }

  Sample process(Sample input, Sample sampleRate, Sample seconds)
  {
    // Set delay time.
    Sample timeInSample
      = std::clamp<Sample>(Sample(2) * sampleRate * seconds, 0, size - 1);

    auto timeInt = int(timeInSample);
    rFraction = timeInSample - Sample(timeInt);

    rptr = wptr - timeInt;
    if (rptr < 0) rptr += size;

    // Write to buffer.
    buf[wptr] = Sample(0.5) * (input + w1);
    ++wptr;
    if (wptr >= size) wptr -= size;

    buf[wptr] = input;
    ++wptr;
    if (wptr >= size) wptr -= size;

    w1 = input;

    // Read from buffer.
    const unsigned int i1 = rptr;
    ++rptr;
    if (rptr >= size) rptr -= size;

    const unsigned int i0 = rptr;
    ++rptr;
    if (rptr >= size) rptr -= size;

    return buf[i0] - rFraction * (buf[i0] - buf[i1]);
  }
};

template<typename Sample> class alignas(64) RingDelay {
public:
  std::array<Delay<Sample>, 16> delay;
  Vec16f delayOut = 0.0f;
  Vec16f feedback = 0.0f;
  Biquad<Sample> lowpass;
  Biquad<Sample> highpass;
  Sample sumFeedback = 0;

  void setup(Sample sampleRate, Sample maxTime)
  {
    for (auto &dly : delay) dly.setup(sampleRate, maxTime);
    lowpass.setLowpass(sampleRate, 1000.0f, 0.5f);
    highpass.setHighpass(sampleRate, 20.0f, 0.5f);
    reset();
  }

  void reset()
  {
    for (auto &dly : delay) dly.reset();
    delayOut = 0.0f;
    feedback = 0.0f;
    lowpass.reset();
    highpass.reset();
  }

  void push(Sample sampleRate, Sample lowpassCutoffHz, Sample highpassCutoffHz)
  {
    lowpass.setLowpass(sampleRate, lowpassCutoffHz, 0.5f);
    highpass.setHighpass(sampleRate, highpassCutoffHz, 0.5f);
  }

  Sample process(
    Sample sampleRate, Sample input, Vec16f delayTime, Sample propagation = Sample(0.5))
  {
    input -= 0.5f * lowpass.process(sumFeedback);

    for (int i = 0; i < delay.size(); ++i)
      delayOut.insert(i, delay[i].process(input + feedback[i], sampleRate, delayTime[i]));

    Vec16f rotL
      = permute16<15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14>(delayOut);
    Vec16f rotR
      = permute16<1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0>(delayOut);

    feedback = 0.97f
      * tanh(propagation * delayOut + (Sample(0.5) * propagation) * (rotL + rotR));

    return sumFeedback = highpass.process(horizontal_add(delayOut)) / delay.size();
  }
};

template<typename Sample, uint8_t size> class KsDelay {
public:
  std::array<Delay<Sample>, size> delay;
  std::array<Biquad<Sample>, size> lowpass;
  std::array<Biquad<Sample>, size> highpass;
  std::array<ExpSmoother<float>, size> smoothFrequencyHz;
  std::array<Sample, size> feedback{};
  ExpDecayCurve<Sample> pitchDecay;
  ExpSmoother<float> smoothPitchMod;
  ExpSmoother<float> smoothFeed;

  void setup(Sample sampleRate)
  {
    for (uint8_t idx = 0; idx < size; ++idx) {
      delay[idx].setup(sampleRate, 0.2f);
      lowpass[idx].setLowpass(sampleRate, 400.0f, 0.5f);
      highpass[idx].setHighpass(sampleRate, 20.0f, 0.5f);
    }
  }

  void reset(Sample pitchMod, Sample feed)
  {
    for (uint8_t idx = 0; idx < size; ++idx) {
      delay[idx].reset();
      lowpass[idx].reset();
      highpass[idx].reset();
      feedback[idx] = 0;
      smoothFrequencyHz[idx].reset(1.0f);
    }

    smoothPitchMod.reset(pitchMod);
    smoothFeed.reset(feed);
  }

  void push(Sample pitchMod, Sample feed)
  {
    smoothPitchMod.push(pitchMod);
    smoothFeed.push(feed);
  }

  void prepare(
    Sample sampleRate,
    Sample noteFrequency,
    Sample pitchDecayAmountHz,
    Sample pitchDecaySeconds,
    Sample lowpassCutoffHz,
    Sample lowpassDecayAmountHz,
    Sample lowpassDecaySeconds)
  {
    pitchDecay.reset(sampleRate, pitchDecaySeconds);
    for (uint8_t idx = 0; idx < size; ++idx) {
      smoothFrequencyHz[idx].push((idx + 1) * noteFrequency);

      lowpass[idx].setLowpass(
        sampleRate, (idx + 1) * lowpassCutoffHz + lowpassDecayAmountHz, 0.5, false);
      lowpass[idx].setLowpass(sampleRate, lowpassCutoffHz, 0.5);
      lowpass[idx].setSmoothTime(sampleRate, lowpassDecaySeconds);
    }
  }

  Sample process(Sample sampleRate, Sample input)
  {
    constexpr float cutoffOffset = float(2.358608708691953) / float(0.2835017917034045);

    auto pitchMod
      = mapCutoff(smoothPitchMod.process() * pitchDecay.process()) - cutoffOffset;

    float sum = 0;
    for (uint8_t idx = 0; idx < size; ++idx) {
      float ksFreq = smoothFrequencyHz[idx].process() + pitchMod;
      float sig
        = highpass[idx].process(input + sometanh(lowpass[idx].process(feedback[idx])));
      feedback[idx] = delay[idx].process(sig, sampleRate, 1.0f / ksFreq);
      sum += feedback[idx] / (idx + 1);
    }

    for (uint8_t idx = 0; idx < size - 1; ++idx)
      feedback[idx] -= smoothFeed.process() * feedback[idx + 1];

    return sum;
  }
};

} // namespace SomeDSP
