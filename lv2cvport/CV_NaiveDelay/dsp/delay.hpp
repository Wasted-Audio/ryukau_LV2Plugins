// (c) 2020 Takamitsu Endo
//
// This file is part of CV_NaiveDelay.
//
// CV_NaiveDelay is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_NaiveDelay is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_NaiveDelay.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <algorithm>
#include <array>
#include <climits>
#include <vector>

#include "../../common/dsp/constants.hpp"

namespace SomeDSP {

// 2x oversampled delay with feedback.
template<typename Sample> class NaiveDelay {
public:
  Sample w1 = 0;
  Sample r1 = 0;
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

  Sample process(Sample input, Sample sampleRate, Sample seconds, Sample feedback)
  {
    input += feedback * r1;

    // Set delay time.
    rptr = wptr - std::clamp<int>(Sample(2) * sampleRate * seconds, 0, int(buf.size()));
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
    const size_t i0 = rptr;
    ++rptr;
    if (rptr >= int(buf.size())) rptr -= buf.size();

    return r1 = buf[i0];
  }
};

} // namespace SomeDSP
