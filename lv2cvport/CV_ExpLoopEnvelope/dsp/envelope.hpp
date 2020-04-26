// (c) 2020 Takamitsu Endo
//
// This file is part of CV_ExpLoopEnvelope.
//
// CV_ExpLoopEnvelope is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_ExpLoopEnvelope is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_ExpLoopEnvelope.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/smoother.hpp"

#include <algorithm>
#include <array>
#include <numeric>

namespace SomeDSP {

template<typename Sample> class ExpLoopEnvelope {
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
  void setup(Sample sampleRate)
  {
    this->sampleRate = sampleRate;
    tailLength = uint32_t(0.01 * sampleRate);
  }

  // seconds must be positive.
  inline uint32_t secondToSample(Sample seconds)
  {
    return uint32_t(sampleRate * seconds);
  }

  void trigger()
  {
    state = State::section0;
    counter = 0;
  }

  // Assuming State::section1 == 1.
  // One-shot if start == end.
  void setLoop(uint8_t start, uint8_t end)
  {
    if (start > end) return;
    loopStart = static_cast<State>(start);
    loopEnd = end <= nSections ? static_cast<State>(end) : State::release;
  }

  // rate must be greater than 0.
  // *Time must be positive.
  void set(Sample rate, Sample releaseTime)
  {
    period = Sample(1) / rate;
    this->releaseTime = releaseTime * period;
  }

  void setDecayTime(
    Sample s0,
    Sample s1,
    Sample s2,
    Sample s3,
    Sample s4,
    Sample s5,
    Sample s6,
    Sample s7)
  {
    this->decayTime[0] = s0 * period;
    this->decayTime[1] = s1 * period;
    this->decayTime[2] = s2 * period;
    this->decayTime[3] = s3 * period;
    this->decayTime[4] = s4 * period;
    this->decayTime[5] = s5 * period;
    this->decayTime[6] = s6 * period;
    this->decayTime[7] = s7 * period;
  }

  void setHoldTime(
    Sample s0,
    Sample s1,
    Sample s2,
    Sample s3,
    Sample s4,
    Sample s5,
    Sample s6,
    Sample s7)
  {
    this->holdTime[0] = s0 * period;
    this->holdTime[1] = s1 * period;
    this->holdTime[2] = s2 * period;
    this->holdTime[3] = s3 * period;
    this->holdTime[4] = s4 * period;
    this->holdTime[5] = s5 * period;
    this->holdTime[6] = s6 * period;
    this->holdTime[7] = s7 * period;
  }

  void setLevel(
    Sample s0,
    Sample s1,
    Sample s2,
    Sample s3,
    Sample s4,
    Sample s5,
    Sample s6,
    Sample s7)
  {
    this->level[0] = s0;
    this->level[1] = s1;
    this->level[2] = s2;
    this->level[3] = s3;
    this->level[4] = s4;
    this->level[5] = s5;
    this->level[6] = s6;
    this->level[7] = s7;
  }

  void release()
  {
    if (loopEnd < State::release) state = State::release;
  }

  void terminate()
  {
    state = State::terminated;
    pController.reset(0);
  }

  Sample processSection(Sample level, Sample sectionTime, Sample transitionTime)
  {
    ++counter;
    if (counter >= secondToSample(sectionTime)) {
      state = state == loopEnd ? loopStart : incrementState(state);
      counter = 0;
    }

    // Use double for cutoffToP(). When using float, max time becomes around 0.5 sec,
    // which is too short for envelope.
    pController.setP(
      PController<double>::cutoffToP(sampleRate, Sample(1) / transitionTime));
    return pController.process(level);
  }

  Sample process()
  {
    switch (state) {
      case State::section0:
        value = processSection(level[0], decayTime[0] + holdTime[0], decayTime[0]);
        break;

      case State::section1:
        value = processSection(level[1], decayTime[1] + holdTime[1], decayTime[1]);
        break;

      case State::section2:
        value = processSection(level[2], decayTime[2] + holdTime[2], decayTime[2]);
        break;

      case State::section3:
        value = processSection(level[3], decayTime[3] + holdTime[3], decayTime[3]);
        break;

      case State::section4:
        value = processSection(level[4], decayTime[4] + holdTime[4], decayTime[4]);
        break;

      case State::section5:
        value = processSection(level[5], decayTime[5] + holdTime[5], decayTime[5]);
        break;

      case State::section6:
        value = processSection(level[6], decayTime[6] + holdTime[6], decayTime[6]);
        break;

      case State::section7:
        value = processSection(level[7], decayTime[7] + holdTime[7], decayTime[7]);
        break;

      case State::release:
        pController.setP(
          PController<double>::cutoffToP(sampleRate, Sample(1) / releaseTime));
        value = pController.process(0);
        if (value < threshold) {
          value = threshold;
          state = State::tail;
          tailCounter = tailLength;
        }
        break;

      case State::tail:
        --tailCounter;
        value = threshold * tailCounter / float(tailLength);
        if (tailCounter == 0) {
          state = State::terminated;
          pController.reset(0);
        } else {
          pController.reset(value);
        }
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
    section2,
    section3,
    section4,
    section5,
    section6,
    section7,
    release,
    tail,
    terminated,
  };

  inline State incrementState(State state)
  {
    return static_cast<State>(static_cast<uint8_t>(state) + 1);
  }

  const Sample threshold = 1e-5;
  const uint8_t nSections = 7;

  uint32_t tailLength = 32;
  uint32_t tailCounter = tailLength;

  PController<Sample> pController;
  State state = State::terminated;
  State loopStart = State::section1;
  State loopEnd = State::section2;

  uint32_t counter = 0;

  Sample sampleRate = 44100;
  Sample value = 0;

  Sample period = 1;
  Sample releaseTime = 1;
  std::array<Sample, 8> decayTime{};
  std::array<Sample, 8> holdTime{};
  std::array<Sample, 8> level{};
};

} // namespace SomeDSP
