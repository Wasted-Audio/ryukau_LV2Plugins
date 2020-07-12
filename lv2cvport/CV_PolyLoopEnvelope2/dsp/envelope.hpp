// (c) 2020 Takamitsu Endo
//
// This file is part of CV_PolyLoopEnvelope2.
//
// CV_PolyLoopEnvelope2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_PolyLoopEnvelope2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_PolyLoopEnvelope2.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/smoother.hpp"

#include <algorithm>
#include <array>
#include <numeric>

namespace SomeDSP {

template<typename Sample> class PolyLoopEnvelope {
public: // For UI.
  Sample getAttackTime()
  {
    size_t start = static_cast<size_t>(loopStart);
    if (start > decayTime.size()) start = decayTime.size();

    Sample sum = 0;
    for (size_t i = 0; i < start; ++i) sum += decayTime[i] + holdTime[i];
    return sum;
  }

  Sample getLoopTime()
  {
    size_t start = static_cast<size_t>(loopStart);
    size_t end = static_cast<size_t>(loopEnd);
    if (start > decayTime.size()) start = decayTime.size();
    if (end >= decayTime.size()) end = decayTime.size() - 1;

    Sample sum = 0;
    for (size_t i = start; i <= end; ++i) sum += decayTime[i] + holdTime[i];
    return sum;
  }

  Sample getReleaseTime() { return releaseTime; }
  uint8_t getState() { return static_cast<uint8_t>(state); }
  Sample getMax() { return *std::max_element(level.begin(), level.end()); }
  Sample getMin() { return *std::min_element(level.begin(), level.end()); }

public: // DSP.
  static const uint8_t nSections = 2;

  void setup(Sample sampleRate) { this->sampleRate = sampleRate; }

  void trigger()
  {
    state = State::section0;
    counter = 0;
    prevLevel = value;
  }

  // One-shot if start <= end.
  void setLoop(uint8_t start, uint8_t end)
  {
    if (start > end) return;
    loopStart = static_cast<State>(start);
    loopEnd = end < nSections ? static_cast<State>(end) : State::release;
  }

  // rate must be greater than 0.
  // *Time and curve must be positive.
  void set(
    Sample rate,
    Sample releaseTime,
    Sample releaseCurve,
    std::array<Sample, nSections> decayTime,
    std::array<Sample, nSections> holdTime,
    std::array<Sample, nSections> level,
    std::array<Sample, nSections> curve)
  {
    Sample period = Sample(1) / rate;
    this->releaseTime = releaseTime * period;
    this->releaseCurve = releaseCurve;

    this->decayTime = decayTime;
    this->holdTime = holdTime;
    this->level = level;
    this->curve = curve;

    for (size_t i = 0; i < nSections; ++i) {
      this->decayTime[i] *= period;
      this->holdTime[i] *= period;
    }
  }

  void release()
  {
    if (loopEnd >= State::release) return;
    state = State::release;
    counter = 0;
    prevLevel = value;
  }

  void terminate()
  {
    state = State::terminated;
    value = 0;
  }

  Sample
  processSection(Sample level, Sample sectionTime, Sample transitionTime, Sample curve)
  {
    ++counter;
    if (counter >= uint32_t(sampleRate * sectionTime)) {
      state = state == loopEnd ? loopStart : incrementState(state);
      counter = 0;
      prevLevel = level;
    }

    Sample trLen = sampleRate * transitionTime;
    if (counter < uint32_t(trLen)) {
      Sample ratio = curve >= 0
        ? somepow(counter / trLen, curve + Sample(1))
        : Sample(1) - somepow((trLen - counter) / trLen, somefabs(curve) + Sample(1));
      return prevLevel + ratio * (level - prevLevel);
    }

    return level;
  }

  Sample processRelease(Sample sectionTime, Sample transitionTime, Sample curve)
  {
    ++counter;
    if (counter >= uint32_t(sampleRate * sectionTime)) {
      state = State::terminated;
      counter = 0;
      prevLevel = 0;
    }

    Sample trLen = sampleRate * transitionTime;
    if (counter < uint32_t(trLen)) {
      Sample ratio = curve >= 0
        ? somepow(counter / trLen, curve + Sample(1))
        : Sample(1) - somepow((trLen - counter) / trLen, somefabs(curve) + Sample(1));
      return prevLevel - ratio * prevLevel;
    }

    return 0;
  }

  Sample process()
  {
    switch (state) {
      case State::section0:
        value
          = processSection(level[0], decayTime[0] + holdTime[0], decayTime[0], curve[0]);
        break;

      case State::section1:
        value
          = processSection(level[1], decayTime[1] + holdTime[1], decayTime[1], curve[1]);
        break;

      case State::release:
        value = processRelease(releaseTime, releaseTime, releaseCurve);
        break;

      default:
        return 0;
    }
    return value;
  }

private:
  enum class State : uint8_t {
    section0,
    section1,
    release,
    terminated,
  };

  inline State incrementState(State state)
  {
    return static_cast<State>(static_cast<uint8_t>(state) + 1);
  }

  State state = State::terminated;
  State loopStart = State::section0;
  State loopEnd = State::section1;

  uint32_t counter = 0;

  Sample sampleRate = 44100;
  Sample value = 0;
  Sample prevLevel = 0;

  Sample releaseTime = 1;
  Sample releaseCurve = 1;
  std::array<Sample, nSections> decayTime{};
  std::array<Sample, nSections> holdTime{};
  std::array<Sample, nSections> level{};
  std::array<Sample, nSections> curve{};
};

} // namespace SomeDSP
