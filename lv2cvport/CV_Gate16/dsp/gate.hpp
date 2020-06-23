// (c) 2020 Takamitsu Endo
//
// This file is part of CV_Gate16.
//
// CV_Gate16 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_Gate16 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_Gate16.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../../../common/dsp/somemath.hpp"

#include <vector>

namespace SomeDSP {

/**
Unused.
*/
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

enum GateType : uint32_t { typeTrigger, typeGate, typeDC };

template<typename Sample> class Gate {
public:
  bool open = false;
  uint32_t type = 0;
  uint32_t delay = 0;
  ExpSmoother<Sample> gain;

  void setType(uint32_t type, bool isNoteOn)
  {
    this->type = type;

    if (type == typeGate)
      open = isNoteOn;
    else
      open = type == typeDC;
  }

  void reset()
  {
    open = false;
    delay = 0;
    gain.reset(0);
  }

  void trigger(Sample sampleRate, Sample delaySeconds)
  {
    open = true;
    delay = type == typeDC ? 0 : uint32_t(delaySeconds * sampleRate);
  }

  void release()
  {
    if (type == typeGate) open = false;
  }

  Sample process()
  {
    if (delay > 0) {
      --delay;
      return 0;
    }
    Sample out = open * gain.process();
    if (type == typeTrigger) open = false;
    return out;
  }
};

} // namespace SomeDSP
