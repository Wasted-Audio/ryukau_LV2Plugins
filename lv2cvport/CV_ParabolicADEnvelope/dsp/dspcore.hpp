// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_ParabolicADEnvelope.
//
// CV_ParabolicADEnvelope is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_ParabolicADEnvelope is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_ParabolicADEnvelope.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../../../common/dsp/somemath.hpp"
#include "../parameter.hpp"

#include <algorithm>
#include <array>
#include <memory>

using namespace SomeDSP;

template<typename Sample> class ParabolicADEnvelope {
public:
  void setup(Sample sampleRate) { this->sampleRate = sampleRate; }

  void
  reset(Sample attackSeconds, Sample attackCurve, Sample decaySeconds, Sample decayCurve)
  {
    counter = 0;
    position = 0;
    velocity = 0;

    auto fs2 = sampleRate * sampleRate;

    auto tempA = Sample(2) / (attackSeconds * attackSeconds) / fs2;
    accelA = tempA / attackCurve;
    brakeA = tempA / (Sample(1) - attackCurve);
    timeA = uint32_t(attackSeconds * attackCurve * sampleRate);
    timeP = uint32_t(attackSeconds * sampleRate);

    auto tempD = Sample(2) / (decaySeconds * decaySeconds) / fs2;
    accelD = tempD / decayCurve;
    brakeD = tempD / (Sample(1) - decayCurve);
    timeD = timeP + uint32_t(decaySeconds * decayCurve * sampleRate);
    timeE = timeP + uint32_t(decaySeconds * sampleRate);
  }

  void terminate() { counter = timeE; }

  Sample process()
  {
    if (counter < timeA) {
      velocity += accelA;
    } else if (counter < timeP) {
      velocity -= brakeA;
    } else if (counter == timeP) {
      velocity = 0;
    } else if (counter < timeD) {
      velocity -= accelD;
    } else if (counter < timeE && position > 0) {
      velocity += brakeD;
    } else {
      return 0;
    }
    counter += 1;
    position += velocity;
    return position;
  }

private:
  Sample sampleRate = 44100;

  uint32_t counter = 0; // In samples.

  Sample position = 0;
  Sample velocity = 0;

  Sample accelA = 0;
  Sample brakeA = 0;
  uint32_t timeA = 0; // In samples.
  uint32_t timeP = 0; // In samples.
  Sample accelD = 0;
  Sample brakeD = 0;
  uint32_t timeD = 0; // In samples.
  uint32_t timeE = 0; // In samples.
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
  ParabolicADEnvelope<float> envelope;
  LinearSmoother<float> interpGain;
};
