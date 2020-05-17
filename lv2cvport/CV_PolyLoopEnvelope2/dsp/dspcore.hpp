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
  inRate,

  inReleaseTime,
  inReleaseCurve,

  inDecay0,
  inDecay1,

  inHold0,
  inHold1,

  inLevel0,
  inLevel1,

  inCurve0,
  inCurve1,
};

struct NoteInfo {
  int32_t id;
  float velocity;
  float noteRatio;
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

  PolyLoopEnvelope<float> envelope;

  ExpSmootherLocal<float> interpRate;

  ExpSmoother<float> interpGain;
  ExpSmoother<float> interpReleaseTime;
  ExpSmoother<float> interpReleaseCurve;

  std::array<ExpSmoother<float>, 2> interpDecayTime;
  std::array<ExpSmoother<float>, 2> interpHoldTime;
  std::array<ExpSmoother<float>, 2> interpLevel;
  std::array<ExpSmoother<float>, 2> interpCurve;
};
