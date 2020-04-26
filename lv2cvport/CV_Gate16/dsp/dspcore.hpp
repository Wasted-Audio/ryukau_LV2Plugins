// (c) 2019-2020 Takamitsu Endo
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
#include "../parameter.hpp"

#include <algorithm>
#include <array>

using namespace SomeDSP;

enum GateType : uint32_t { typeTrigger, typeGate, typeDC };

template<typename Sample> class Gate {
public:
  bool open = false;
  uint32_t type = 0;
  uint32_t delay = 0;
  LinearSmoother<Sample> gain;

  void setType(uint32_t type)
  {
    this->type = type;
    open = open || type == typeDC;
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
  void process(const size_t length, float **outputs);
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

  std::array<Gate<float>, nGate> gates;

  LinearSmoother<float> interpMasterGain;
};
