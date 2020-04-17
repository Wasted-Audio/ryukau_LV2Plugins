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

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../../../common/dsp/somemath.hpp"
#include "../parameter.hpp"

#include "envelope.hpp"

#include <algorithm>
#include <array>
#include <memory>

using namespace SomeDSP;

enum Input {
  inGate,

  inGain,
  inReleaseTime,
  inRate,

  inDecay0,
  inDecay1,
  inDecay2,
  inDecay3,
  inDecay4,
  inDecay5,
  inDecay6,
  inDecay7,

  inHold0,
  inHold1,
  inHold2,
  inHold3,
  inHold4,
  inHold5,
  inHold6,
  inHold7,

  inLevel0,
  inLevel1,
  inLevel2,
  inLevel3,
  inLevel4,
  inLevel5,
  inLevel6,
  inLevel7,
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
  void process(const size_t length, const float **inputs, float *out0);
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
  std::vector<NoteInfo> noteStack; // Top of this stack is current note.

  float sampleRate = 44100.0f;
  float noteRatio = 1.0f;
  bool isGateOpen = false;

  ExpLoopEnvelope<float> envelope;

  LinearSmoother<float> interpGain;
  LinearSmoother<float> interpRate;
  LinearSmoother<float> interpReleaseTime;

  LinearSmoother<float> interpS0DecayTime;
  LinearSmoother<float> interpS1DecayTime;
  LinearSmoother<float> interpS2DecayTime;
  LinearSmoother<float> interpS3DecayTime;
  LinearSmoother<float> interpS4DecayTime;
  LinearSmoother<float> interpS5DecayTime;
  LinearSmoother<float> interpS6DecayTime;
  LinearSmoother<float> interpS7DecayTime;

  LinearSmoother<float> interpS0HoldTime;
  LinearSmoother<float> interpS1HoldTime;
  LinearSmoother<float> interpS2HoldTime;
  LinearSmoother<float> interpS3HoldTime;
  LinearSmoother<float> interpS4HoldTime;
  LinearSmoother<float> interpS5HoldTime;
  LinearSmoother<float> interpS6HoldTime;
  LinearSmoother<float> interpS7HoldTime;

  LinearSmoother<float> interpS0Level;
  LinearSmoother<float> interpS1Level;
  LinearSmoother<float> interpS2Level;
  LinearSmoother<float> interpS3Level;
  LinearSmoother<float> interpS4Level;
  LinearSmoother<float> interpS5Level;
  LinearSmoother<float> interpS6Level;
  LinearSmoother<float> interpS7Level;
};
