// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_LinearADSREnvelope.
//
// CV_LinearADSREnvelope is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_LinearADSREnvelope is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_LinearADSREnvelope.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../parameter.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>

using namespace SomeDSP;

template<typename Sample> class LinearADSREnvelope {
public:
  void setup(Sample sampleRate) { this->sampleRate = sampleRate; }

  Sample adaptTime(Sample seconds, Sample noteFreq)
  {
    const Sample cycle = Sample(1) / noteFreq;
    return seconds >= cycle ? seconds : cycle > Sample(0.1) ? Sample(0.1) : cycle;
  }

  Sample secondToDelta(Sample seconds) { return 1 / (sampleRate * seconds); }

  void trigger(
    Sample attackTime,
    Sample decayTime,
    Sample sustainLevel,
    Sample releaseTime,
    Sample noteFreq)
  {
    state = stateAttack;
    value = Sample(1);
    atkOffset = state == stateTerminated ? 0 : out;
    atkRange = Sample(1) - atkOffset;
    set(attackTime, decayTime, sustainLevel, releaseTime, noteFreq);
  }

  void set(
    Sample attackTime,
    Sample decayTime,
    Sample sustainLevel,
    Sample releaseTime,
    Sample noteFreq)
  {
    sus.push(std::clamp<Sample>(sustainLevel, Sample(0), Sample(1)));
    atk = secondToDelta(adaptTime(attackTime, noteFreq));
    dec = secondToDelta(adaptTime(decayTime, noteFreq));
    rel = secondToDelta(adaptTime(releaseTime, noteFreq));
  }

  void release()
  {
    state = stateRelease;
    value = Sample(1);
    relRange = out;
  }

  void terminate() { state = stateTerminated; }

  Sample process()
  {
    sus.process();

    if (value <= Sample(0)) {
      state += 1;
      value = Sample(1);
    }

    switch (state) {
      case stateAttack:
        value -= atk;
        out = atkOffset + atkRange * (Sample(1) - value);
        break;

      case stateDecay:
        value -= dec;
        out = (Sample(1) - sus.getValue()) * value + sus.getValue();
        break;

      case stateSustain:
        out = sus.getValue();
        break;

      case stateRelease:
        value -= rel;
        out = relRange * value;
        break;

      default:
        return Sample(0);
    }
    return std::clamp<Sample>(out, Sample(0), Sample(1));
  }

protected:
  enum State : int32_t {
    stateAttack,
    stateDecay,
    stateSustain,
    stateRelease,
    stateTerminated
  };

  Sample sampleRate = 44100;
  LinearSmoother<Sample> sus;
  Sample atk = 0.01;
  Sample atkOffset = 0;
  Sample atkRange = 1;
  Sample dec = 0.01;
  Sample rel = 0.01;
  Sample relRange = 0.5;
  Sample value = 0;
  Sample out = 0;
  int32_t state = stateTerminated;
};

struct NoteInfo {
  int32_t id;
  float velocity;
};

class DSPCore {
public:
  static const size_t maxVoice = 32;
  GlobalParameter param;

  void setup(double sampleRate);
  void reset(); // Stop sounds.
  void setParameters();
  void process(const size_t length, float *out0);
  void noteOn(int32_t noteId, int16_t pitch, float tuning, float velocity);
  void noteOff(int32_t noteId);

  struct MidiNote {
    bool isNoteOn;
    uint32_t frame;
    int32_t id;
    int16_t pitch;
    float tuning;
    float velocity;
  };

  std::vector<MidiNote> midiNotes;

  void pushMidiNote(
    bool isNoteOn,
    uint32_t frame,
    int32_t noteId,
    int16_t pitch,
    float tuning,
    float velocity)
  {
    MidiNote note;
    note.isNoteOn = isNoteOn;
    note.frame = frame;
    note.id = noteId;
    note.pitch = pitch;
    note.tuning = tuning;
    note.velocity = velocity;
    midiNotes.push_back(note);
  }

  void processMidiNote(uint32_t frame)
  {
    while (true) {
      auto it = std::find_if(midiNotes.begin(), midiNotes.end(), [&](const MidiNote &nt) {
        return nt.frame == frame;
      });
      if (it == std::end(midiNotes)) return;
      if (it->isNoteOn)
        noteOn(it->id, it->pitch, it->tuning, it->velocity);
      else
        noteOff(it->id);
      midiNotes.erase(it);
    }
  }

private:
  float sampleRate = 44100.0f;
  std::vector<NoteInfo> noteStack; // Top of this stack is current note.
  LinearADSREnvelope<float> envelope;
  LinearSmoother<float> interpGain;
};
