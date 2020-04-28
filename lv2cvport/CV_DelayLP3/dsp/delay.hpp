// (c) 2020 Takamitsu Endo
//
// This file is part of CV_DelayLP3.
//
// CV_DelayLP3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_DelayLP3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_DelayLP3.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <algorithm>
#include <array>
#include <climits>
#include <vector>

#include "../../common/dsp/constants.hpp"

namespace SomeDSP {

template<typename Sample> struct LP3 {
  Sample acc = 0;
  Sample vel = 0;
  Sample pos = 0;
  Sample x1 = 0;

  void reset() { acc = vel = pos = x1 = 0; }

  Sample process(
    Sample x0, Sample sampleRate, Sample lowpassHz, Sample resonance, Sample highpassHz)
  {
    // clang-format off

    const Sample x = lowpassHz / sampleRate;
    const Sample c
      = Sample(56.85341479156533)   * x * x * x * x * x * x
      + Sample(-60.92051508862034)  * x * x * x * x * x
      + Sample(-1.6515635438744682) * x * x * x * x
      + Sample(31.558896956675998)  * x * x * x
      + Sample(-20.61402812645397)  * x * x
      + Sample(6.320753515093109)   * x;

    const Sample y = highpassHz / sampleRate;
    const Sample decay = (
          Sample(-0.5568264156772206)
        + Sample(131.4949975016369) * y
        + Sample(82.57629637000909) * y * y
      ) / (
          Sample(-0.5541628806243104)
        + Sample(127.54099261383865) * y
        + Sample(869.4780295729987)  * y * y
      )
      - Sample(0.004806411880040606);

    // clang-format on

    const Sample k = resonance;

    acc = k * acc + c * vel;
    vel -= acc + x0 - x1;
    pos -= c * vel;

    pos *= decay;

    x1 = x0;
    return pos;
  }
};

// 2x oversampled delay with feedback.
template<typename Sample> class DelayLP3 {
public:
  Sample w1 = 0;
  Sample r1 = 0;
  Sample rFraction = 0.0;
  int wptr = 0;
  int rptr = 0;
  LP3<Sample> lp3;
  std::vector<Sample> buf;

  void setup(Sample sampleRate, Sample maxTime)
  {
    auto size = int(Sample(2) * sampleRate * maxTime) + 1;
    buf.resize(size < 0 ? 4 : size);

    reset();
  }

  void reset()
  {
    w1 = 0;
    r1 = 0;
    lp3.reset();
    std::fill(buf.begin(), buf.end(), 0);
  }

  Sample process(
    Sample input,
    Sample sampleRate,
    Sample seconds,
    Sample feedback,
    Sample lowpassHz,
    Sample resonance,
    Sample highpassHz)
  {
    input += feedback * lp3.process(r1, sampleRate, lowpassHz, resonance, highpassHz);

    // Set delay time.
    Sample timeInSample
      = std::clamp<Sample>(Sample(2) * sampleRate * seconds, 0, buf.size());

    int timeInt = int(timeInSample);
    rFraction = timeInSample - Sample(timeInt);

    rptr = wptr - timeInt;
    if (rptr < 0) rptr += buf.size();

    // Write to buffer.
    buf[wptr] = input - Sample(0.5) * (input - w1);
    ++wptr;
    if (wptr >= int(buf.size())) wptr -= buf.size();

    buf[wptr] = input;
    ++wptr;
    if (wptr >= int(buf.size())) wptr -= buf.size();

    w1 = input;

    // Read from buffer.
    const size_t i1 = rptr;
    ++rptr;
    if (rptr >= int(buf.size())) rptr -= buf.size();

    const size_t i0 = rptr;
    ++rptr;
    if (rptr >= int(buf.size())) rptr -= buf.size();

    return r1 = buf[i0] - rFraction * (buf[i0] - buf[i1]);
  }
};

} // namespace SomeDSP
