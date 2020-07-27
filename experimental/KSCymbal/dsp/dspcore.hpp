// (c) 2020 Takamitsu Endo
//
// This file is part of KSCymbal.
//
// KSCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KSCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KSCymbal.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../../../common/dsp/somemath.hpp"
#include "../parameter.hpp"
#include "delay.hpp"
#include "envelope.hpp"
#include "iir.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <random>

using namespace SomeDSP;

constexpr uint64_t rngOffset = 16777216;

enum class NoteState { active, release, rest };

#define NOTE_PROCESS_INFO_SMOOTHER(METHOD)                                               \
  lowpassCutoff.METHOD(pv[ID::lowpassCutoff]->getFloat());                               \
  lowpassEnvelopeOffset.METHOD(pv[ID::lowpassEnvelopeOffset]->getFloat());               \
  highpassCutoff.METHOD(pv[ID::highpassCutoff]->getFloat());                             \
  highpassReleaseAmount.METHOD(pv[ID::highpassReleaseAmount]->getFloat());               \
  noiseGain.METHOD(pv[ID::exciterGain]->getFloat());

struct NoteProcessInfo {
  std::minstd_rand rngNoise{0};
  std::minstd_rand rngComb{rngOffset};
  std::minstd_rand rngCymbal{2 * rngOffset};
  std::minstd_rand rngUnison{3 * rngOffset};

  ExpSmoother<float> lowpassCutoff;
  ExpSmoother<float> lowpassEnvelopeOffset;
  ExpSmoother<float> highpassCutoff;
  ExpSmoother<float> highpassReleaseAmount;
  ExpSmoother<float> noiseGain;

  void reset(GlobalParameter &param)
  {
    using ID = ParameterID::ID;
    auto &pv = param.value;

    rngNoise.seed(pv[ID::seed]->getInt());
    rngComb.seed(pv[ID::seed]->getInt() + rngOffset);
    rngCymbal.seed(pv[ID::seed]->getInt() + 2 * rngOffset);
    rngUnison.seed(pv[ID::seed]->getInt() + 3 * rngOffset);

    NOTE_PROCESS_INFO_SMOOTHER(reset);
  }

  void setParameters(GlobalParameter &param)
  {
    using ID = ParameterID::ID;
    auto &pv = param.value;

    NOTE_PROCESS_INFO_SMOOTHER(push);
  }

  void process()
  {
    lowpassCutoff.process();
    lowpassEnvelopeOffset.process();
    highpassCutoff.process();
    highpassReleaseAmount.process();
    noiseGain.process();
  }
};

#define NOTE_CLASS(INSTRSET)                                                             \
  class Note_##INSTRSET {                                                                \
  public:                                                                                \
    NoteState state = NoteState::rest;                                                   \
                                                                                         \
    int32_t id = -1;                                                                     \
    float velocity = 0;                                                                  \
    float noteFreq = 1;                                                                  \
    float pan = 0.5f;                                                                    \
    float gain = 0;                                                                      \
                                                                                         \
    int32_t noiseCounter = 0;                                                            \
    int32_t releaseCounter = 0;                                                          \
    float releaseLength = 0;                                                             \
                                                                                         \
    PController<float> exciterLowpass;                                                   \
    AttackGate<float> gate;                                                              \
    std::array<ShortComb<float>, nComb> comb;                                            \
    KsHat<float, nDelay> cymbal;                                                         \
    ExpADSREnvelopeP<float> cymbalLowpassEnvelope;                                       \
    HighpassReleaseGate<float> cymbalHighpassEnvelope;                                   \
    DCKiller<float> dcKiller;                                                            \
    EasyCompressor<float> compressor;                                                    \
                                                                                         \
    void setup(float sampleRate);                                                        \
    void noteOn(                                                                         \
      int32_t noteId,                                                                    \
      float notePitch,                                                                   \
      float velocity,                                                                    \
      float pan,                                                                         \
      float sampleRate,                                                                  \
      NoteProcessInfo &info,                                                             \
      GlobalParameter &param);                                                           \
    void release(float sampleRate);                                                      \
    void rest();                                                                         \
    bool isAttacking();                                                                  \
    float getGain();                                                                     \
    std::array<float, 2> process(float sampleRate, NoteProcessInfo &info);               \
  };

NOTE_CLASS(AVX512)
NOTE_CLASS(AVX2)
NOTE_CLASS(SSE41)
NOTE_CLASS(SSE2)

class DSPInterface {
public:
  virtual ~DSPInterface(){};

  constexpr static uint8_t maxVoice = 16;
  GlobalParameter param;

  virtual void setup(double sampleRate) = 0;
  virtual void reset() = 0;   // Stop sounds.
  virtual void startup() = 0; // Reset phase, random seed etc.
  virtual void setParameters(float tempo) = 0;
  virtual void process(const size_t length, float *out0, float *out1) = 0;
  virtual void noteOn(int32_t noteId, int16_t pitch, float tuning, float velocity) = 0;
  virtual void noteOff(int32_t noteId) = 0;

  struct MidiNote {
    bool isNoteOn;
    uint32_t frame;
    int32_t id;
    int16_t pitch;
    float tuning;
    float velocity;
  };

  std::vector<MidiNote> midiNotes;

  virtual void pushMidiNote(
    bool isNoteOn,
    uint32_t frame,
    int32_t noteId,
    int16_t pitch,
    float tuning,
    float velocity)
    = 0;
  virtual void processMidiNote(uint32_t frame) = 0;
};

#define DSPCORE_CLASS(INSTRSET)                                                          \
  class DSPCore_##INSTRSET final : public DSPInterface {                                 \
  public:                                                                                \
    DSPCore_##INSTRSET();                                                                \
                                                                                         \
    void setup(double sampleRate);                                                       \
    void reset();                                                                        \
    void startup();                                                                      \
    void setParameters(float tempo);                                                     \
    void process(const size_t length, float *out0, float *out1);                         \
    void noteOn(int32_t noteId, int16_t pitch, float tuning, float velocity);            \
    void noteOff(int32_t noteId);                                                        \
    void fillTransitionBuffer(size_t noteIndex);                                         \
                                                                                         \
    struct MidiNote {                                                                    \
      bool isNoteOn;                                                                     \
      uint32_t frame;                                                                    \
      int32_t id;                                                                        \
      int16_t pitch;                                                                     \
      float tuning;                                                                      \
      float velocity;                                                                    \
    };                                                                                   \
                                                                                         \
    std::vector<MidiNote> midiNotes;                                                     \
                                                                                         \
    void pushMidiNote(                                                                   \
      bool isNoteOn,                                                                     \
      uint32_t frame,                                                                    \
      int32_t noteId,                                                                    \
      int16_t pitch,                                                                     \
      float tuning,                                                                      \
      float velocity)                                                                    \
    {                                                                                    \
      MidiNote note;                                                                     \
      note.isNoteOn = isNoteOn;                                                          \
      note.frame = frame;                                                                \
      note.id = noteId;                                                                  \
      note.pitch = pitch;                                                                \
      note.tuning = tuning;                                                              \
      note.velocity = velocity;                                                          \
      midiNotes.push_back(note);                                                         \
    }                                                                                    \
                                                                                         \
    void processMidiNote(uint32_t frame)                                                 \
    {                                                                                    \
      while (true) {                                                                     \
        auto it                                                                          \
          = std::find_if(midiNotes.begin(), midiNotes.end(), [&](const MidiNote &nt) {   \
              return nt.frame == frame;                                                  \
            });                                                                          \
        if (it == std::end(midiNotes)) return;                                           \
        if (it->isNoteOn)                                                                \
          noteOn(it->id, it->pitch, it->tuning, it->velocity);                           \
        else                                                                             \
          noteOff(it->id);                                                               \
        midiNotes.erase(it);                                                             \
      }                                                                                  \
    }                                                                                    \
                                                                                         \
  private:                                                                               \
    void setUnisonPan(size_t nUnison);                                                   \
                                                                                         \
    float sampleRate = 44100.0f;                                                         \
    float velocity = 0.0f;                                                               \
    DecibelScale<float> velocityMap{-30, 0, true};                                       \
                                                                                         \
    uint8_t nVoice = 8;                                                                  \
    int32_t panCounter = 0;                                                              \
    std::vector<size_t> noteIndices;                                                     \
    std::vector<size_t> voiceIndices;                                                    \
    std::vector<float> unisonPan;                                                        \
    std::array<Note_##INSTRSET, maxVoice> notes;                                         \
                                                                                         \
    NoteProcessInfo info;                                                                \
    ExpSmoother<float> interpMasterGain;                                                 \
                                                                                         \
    std::vector<std::array<float, 2>> transitionBuffer{};                                \
    bool isTransitioning = false;                                                        \
    size_t trIndex = 0;                                                                  \
    size_t trStop = 0;                                                                   \
  };

DSPCORE_CLASS(AVX512)
DSPCORE_CLASS(AVX2)
DSPCORE_CLASS(SSE41)
DSPCORE_CLASS(SSE2)
