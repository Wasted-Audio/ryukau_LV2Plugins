// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_ExpPolyADEnvelope.
//
// CV_ExpPolyADEnvelope is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_ExpPolyADEnvelope is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_ExpPolyADEnvelope.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../../../common/dsp/somemath.hpp"
#include "../parameter.hpp"

#include <algorithm>
#include <array>
#include <cfloat>
#include <cmath>
#include <memory>

using namespace SomeDSP;

template<typename Sample> class ExpPolyEnvelope {
public:
  // attack is in seconds.
  // curve is arbitrary value.
  void reset(Sample sampleRate, Sample attack, Sample curve)
  {
    alpha = attack * curve;

    auto betaMin = alpha / getTerminationTime();
    if (curve < betaMin) curve = betaMin;

    peak = somepow<Sample>(alpha / curve, alpha) * someexp<Sample>(-alpha);
    gamma = someexp<Sample>(-curve / sampleRate);
    tick = 1.0 / sampleRate;

    time = 0.0;
    value = 1.0;
  }

  void terminate() { time = INFINITY; }
  bool isReleasing() { return time >= attack; }
  Sample getTerminationTime() { return somepow<Sample>(DBL_MAX, 1.0 / alpha); }

  Sample process()
  {
    auto output = somepow<Sample>(time, alpha) * value / peak;
    if (!std::isfinite(output)) return 0.0; // Just in case.
    time += tick;
    value *= gamma;
    return output;
  }

protected:
  Sample value = 0;
  Sample peak = 1;
  Sample gamma = 0;
  Sample attack = 0;
  Sample tick = 0;
  Sample alpha = 0;
  Sample time = 0;
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
  ExpPolyEnvelope<double> envelope;
  LinearSmoother<float> interpGain;
};
