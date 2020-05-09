// (c) 2020 Takamitsu Endo
//
// This file is part of CV_NestedLongAllpass.
//
// CV_NestedLongAllpass is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_NestedLongAllpass is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_NestedLongAllpass.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <algorithm>
#include <array>
#include <climits>
#include <vector>

#include "../../common/dsp/constants.hpp"

namespace SomeDSP {

// 2x oversampled delay with feedback.
template<typename Sample> class Delay {
public:
  Sample w1 = 0;
  Sample rFraction = 0.0;
  int wptr = 0;
  int rptr = 0;
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
    std::fill(buf.begin(), buf.end(), 0);
  }

  Sample process(Sample input, Sample sampleRate, Sample seconds)
  {
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

    return buf[i0] - rFraction * (buf[i0] - buf[i1]);
  }
};

/**
Schroeder allpass filter
https://ccrma.stanford.edu/~jos/pasp/Allpass_Two_Combs.html

Transfer function H(z):
H(z) = (gain + z^{-M}) / (1 + gain * z^{-M})
*/
template<typename Sample> class SchroederAllpass {
public:
  Sample buffer = 0;
  Delay<Sample> delay;

  void setup(Sample sampleRate, Sample maxTime) { delay.setup(sampleRate, maxTime); }

  void reset()
  {
    buffer = 0;
    delay.reset();
  }

  // gain in [0, 1].
  Sample process(Sample input, Sample sampleRate, Sample seconds, Sample gain)
  {
    input -= gain * buffer;
    auto output = buffer + gain * input;
    buffer = delay.process(input, sampleRate, seconds);
    return output;
  }
};

template<typename Sample> struct SchroederAllpassData {
  Sample seconds = 0;
  Sample outerFeed = 0; // in [-1, 1].
  Sample innerFeed = 0; // in [-1, 1].
};

template<typename Sample, size_t nest> class NestedSchroeder {
public:
  std::array<Sample, nest> in{};
  std::array<Sample, nest> buffer{};
  std::array<SchroederAllpass<Sample>, nest> allpass;
  std::array<SchroederAllpassData<Sample>, nest> data;

  void setup(Sample sampleRate, Sample maxTime)
  {
    for (auto &ap : allpass) ap.setup(sampleRate, maxTime);
  }

  void reset()
  {
    in.fill(0);
    buffer.fill(0);
    for (auto &ap : allpass) ap.reset();
    for (auto &dat : data) dat = {0, 0, 0};
  }

  Sample process(Sample input, Sample sampleRate)
  {
    for (size_t idx = 0; idx < nest; ++idx) {
      input -= data[idx].outerFeed * buffer[idx];
      in[idx] = input;
    }

    Sample out = in.back();
    for (size_t idx = nest - 1; idx < nest; --idx) {
      buffer[idx]
        = allpass[idx].process(out, sampleRate, data[idx].seconds, data[idx].innerFeed)
        + data[idx].outerFeed * in[idx];
      out = buffer[idx];
    }

    return out;
  }
};

} // namespace SomeDSP
