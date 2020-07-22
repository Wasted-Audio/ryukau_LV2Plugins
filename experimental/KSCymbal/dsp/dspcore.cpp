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

#include "dspcore.hpp"

#if INSTRSET >= 10
  #define NOTE_NAME Note_AVX512
  #define DSPCORE_NAME DSPCore_AVX512
#elif INSTRSET >= 8
  #define NOTE_NAME Note_AVX2
  #define DSPCORE_NAME DSPCore_AVX2
#elif INSTRSET >= 5
  #define NOTE_NAME Note_SSE41
  #define DSPCORE_NAME DSPCore_SSE41
#elif INSTRSET >= 2
  #define NOTE_NAME Note_SSE2
  #define DSPCORE_NAME DSPCore_SSE2
#else
  #error Unsupported instruction set
#endif

inline float calcMasterPitch(
  int32_t octave, int32_t semi, int32_t milli, float bend, float equalTemperament)
{
  return equalTemperament * octave + semi + milli / 1000.0f + (bend - 0.5f) * 4.0f;
}

inline float
notePitchToFrequency(float notePitch, float equalTemperament = 12.0f, float a4Hz = 440.0f)
{
  return a4Hz * powf(2.0f, (notePitch - 69.0f) / equalTemperament);
}

inline float calcNotePitch(float notePitch, float equalTemperament = 12.0f)
{
  return powf(2.0f, (notePitch - 69.0f) / equalTemperament);
}

// Fast approximation of PController::cutoffToP().
// x is normalized frequency. Range is [0, 0.5).
template<typename T> inline T cutoffToPApprox(T x)
{
  return (T(-0.0004930424721520979) + T(2.9650003823571467) * x
          + T(1.8250079928630534) * x * x)
    / (T(0.4649282844668299) + T(1.8754712250208077) * x + T(3.7307819672604023) * x * x)
    + T(0.0010604699447733293);
}

void NOTE_NAME::setup(float sampleRate)
{
  cymbalLowpassEnvelope.setup(sampleRate);
  cymbal.setup(sampleRate);
}

void NOTE_NAME::noteOn(
  int32_t noteId,
  float notePitch,
  float velocity,
  float pan,
  float sampleRate,
  NoteProcessInfo &info,
  GlobalParameter &param)
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  state = NoteState::active;
  id = noteId;

  this->velocity = velocity;
  this->pan = pan;
  gain = 1.0f;

  noiseCounter = int32_t(pv[ID::attack]->getFloat() * sampleRate);
  gate.reset(sampleRate, pv[ID::attack]->getFloat());

  releaseLength = 0.01f * sampleRate;
  releaseCounter = int32_t(releaseLength);

  for (size_t idx = 0; idx < nComb; ++idx) {
    const auto combTime = pv[ID::combTime0 + idx]->getFloat();
    const auto spread = combTime * pv[ID::randomComb]->getFloat();
    std::uniform_real_distribution<float> distCombTime(
      combTime - spread, combTime + spread);
    comb[idx].setTime(sampleRate, distCombTime(info.rngComb));
  }

  const auto eqTemp = pv[ID::equalTemperament]->getFloat() + 1;
  const auto semitone = pv[ID::semitone]->getInt() - 120;
  const auto octave = eqTemp * (int32_t(pv[ID::octave]->getInt()) - 12);
  const auto milli = 0.001f * (pv[ID::milli]->getInt() - 1000);
  const auto pitch = calcNotePitch(octave + semitone + milli + notePitch, eqTemp);
  for (size_t idx = 0; idx < nDelay; ++idx) {
    const auto freq = pitch * pv[ID::frequency0 + idx]->getFloat();
    const auto spread
      = (freq - Scales::frequency.getMin()) * pv[ID::randomFrequency]->getFloat();
    std::uniform_real_distribution<float> distFreq(freq - spread, freq + spread);
    cymbal.string[idx].delay.setTime(sampleRate, 1.0f / distFreq(info.rngCymbal));
  }
  cymbal.b1
    = OnePoleHighpass<float>::setCutoff(sampleRate, pv[ID::highpassCutoffHz]->getFloat());
  cymbal.kp = 0;
  cymbal.trigger(pv[ID::distance]->getFloat());

  cymbalLowpassEnvelope.reset(
    pv[ID::lowpassA]->getFloat(), pv[ID::lowpassD]->getFloat(),
    pv[ID::lowpassS]->getFloat(), pv[ID::lowpassR]->getFloat());

  dcKiller.reset();
}

void NOTE_NAME::release()
{
  if (state == NoteState::rest) return;
  state = NoteState::release;
  cymbalLowpassEnvelope.release();
}

void NOTE_NAME::rest() { state = NoteState::rest; }

bool NOTE_NAME::isAttacking() { return cymbalLowpassEnvelope.isAttacking(); }

float NOTE_NAME::getGain() { return gain; }

std::array<float, 2> NOTE_NAME::process(float sampleRate, NoteProcessInfo &info)
{
  std::uniform_real_distribution<float> dist(-0.5f, 0.5f);

  float sig;
  if (noiseCounter > 0) {
    sig = dist(info.rngNoise);
    --noiseCounter;
  } else {
    sig = 0;
  }
  for (auto &cmb : comb) sig -= cmb.process(sig);

  float lpEnv = cymbalLowpassEnvelope.process();
  gain = velocity * lpEnv; // Used to determin most quiet note.

  cymbal.kp = cutoffToPApprox(0.5f * (1 + lpEnv) * info.lowpassCutoffHz / sampleRate);

  sig = velocity * dcKiller.process(cymbal.process(sig * gate.process()));

  if (cymbalLowpassEnvelope.isTerminated()) {
    --releaseCounter;
    sig *= releaseCounter / releaseLength;
    if (releaseCounter <= 0) state = NoteState::rest;
  }

  return {(1.0f - pan) * sig, pan * sig};
}

void DSPCORE_NAME::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  // 10 msec + 1 sample transition time.
  transitionBuffer.resize(1 + size_t(sampleRate * 0.005), {0.0f, 0.0f});

  for (auto &note : notes) note.setup(sampleRate);

  reset();
}

DSPCORE_NAME::DSPCORE_NAME()
{
  unisonPan.reserve(maxVoice);
  noteIndices.reserve(maxVoice);
  voiceIndices.reserve(maxVoice);
}

void DSPCORE_NAME::reset()
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  info.rngNoise.seed(pv[ID::seed]->getInt());
  info.rngComb.seed(pv[ID::seed]->getInt() + rngOffset);
  info.rngCymbal.seed(pv[ID::seed]->getInt() + 2 * rngOffset);
  info.rngUnison.seed(pv[ID::seed]->getInt() + 3 * rngOffset);

  for (auto &note : notes) note.rest();

  interpMasterGain.reset(pv[ID::gain]->getFloat() * pv[ID::boost]->getFloat());
}

void DSPCORE_NAME::startup() {}

void DSPCORE_NAME::setParameters(float tempo)
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  interpMasterGain.push(pv[ID::gain]->getFloat() * pv[ID::boost]->getFloat());

  info.lowpassCutoffHz = pv[ID::lowpassCutoffHz]->getFloat();
}

void DSPCORE_NAME::process(const size_t length, float *out0, float *out1)
{
  SmootherCommon<float>::setBufferSize(length);

  std::array<float, 2> frame{};
  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);

    frame.fill(0.0f);

    for (auto &note : notes) {
      if (note.state == NoteState::rest) continue;
      auto sig = note.process(sampleRate, info);
      frame[0] += sig[0];
      frame[1] += sig[1];
    }

    if (isTransitioning) {
      frame[0] += transitionBuffer[trIndex][0];
      frame[1] += transitionBuffer[trIndex][1];
      transitionBuffer[trIndex].fill(0.0f);
      trIndex = (trIndex + 1) % transitionBuffer.size();
      if (trIndex == trStop) isTransitioning = false;
    }

    const auto masterGain = interpMasterGain.process();
    out0[i] = masterGain * frame[0];
    out1[i] = masterGain * frame[1];
  }
}

void DSPCORE_NAME::setUnisonPan(size_t nUnison)
{
  using ID = ParameterID::ID;

  unisonPan.resize(nUnison);

  // TODO: Add unisonPan parameter.
  // const auto unisonPanRange = param.value[ID::unisonPan]->getFloat();
  const float unisonPanRange = 1;
  const float panRange = unisonPanRange / float(nUnison - 1);
  const float panOffset = 0.5f - 0.5f * unisonPanRange;

  for (size_t idx = 0; idx < unisonPan.size(); ++idx)
    unisonPan[idx] = panRange * idx + panOffset;
}

void DSPCORE_NAME::noteOn(int32_t noteId, int16_t pitch, float tuning, float velocity)
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  // TODO: Add nUnison.
  const size_t nUnison = 1 + pv[ID::nUnison]->getInt();

  noteIndices.resize(0);

  // Pick up note from resting one.
  for (size_t index = 0; index < maxVoice; ++index) {
    if (notes[index].id == noteId) noteIndices.push_back(index);
    if (notes[index].state == NoteState::rest) noteIndices.push_back(index);
    if (noteIndices.size() >= nUnison) break;
  }

  // If there aren't enought resting note, pick up from most quiet one.
  if (noteIndices.size() < nUnison) {
    voiceIndices.resize(maxVoice);
    std::iota(voiceIndices.begin(), voiceIndices.end(), 0);
    std::sort(voiceIndices.begin(), voiceIndices.end(), [&](size_t lhs, size_t rhs) {
      return !notes[lhs].isAttacking() && (notes[lhs].getGain() < notes[rhs].getGain());
    });

    for (auto &index : voiceIndices) {
      fillTransitionBuffer(index);
      noteIndices.push_back(index);
      if (noteIndices.size() >= nUnison) break;
    }
  }

  // Parameters must be set after transition buffer is filled.
  velocity = velocityMap.map(velocity);

  if (pv[ID::retriggerNoise]->getInt()) info.rngNoise.seed(pv[ID::seed]->getInt());
  if (pv[ID::retriggerComb]->getInt())
    info.rngComb.seed(pv[ID::seed]->getInt() + rngOffset);
  if (pv[ID::retriggerCymbal]->getInt())
    info.rngCymbal.seed(pv[ID::seed]->getInt() + 2 * rngOffset);
  if (pv[ID::retriggerCymbal]->getInt())
    info.rngCymbal.seed(pv[ID::seed]->getInt() + 3 * rngOffset);

  if (nUnison <= 1) {
    notes[noteIndices[0]].noteOn(
      noteId, float(pitch) + tuning, velocity, 0.5f, sampleRate, info, param);
    return;
  }

  setUnisonPan(nUnison);

  for (size_t i = 0; i < unisonPan.size(); ++i) {
  }

  // TODO: Add unison parameters.
  // const auto unisonDetune = param.value[ID::unisonDetune]->getFloat();
  // const auto unisonGainRandom = param.value[ID::unisonGainRandom]->getFloat();
  // const bool randomizeDetune = param.value[ID::unisonDetuneRandom]->getInt();
  // std::uniform_real_distribution<float> distDetune(0.0f, 1.0f);
  // std::uniform_real_distribution<float> distGain(1.0f - unisonGainRandom, 1.0f);
  for (size_t unison = 0; unison < nUnison; ++unison) {
    if (noteIndices.size() <= unison) break;
    // auto detune
    //   = unison * unisonDetune * (randomizeDetune ? distDetune(info.rngUnison) : 1.0f);
    // auto notePitch = (float(pitch) + tuning) * (1.0f + detune);
    // auto vel = distGain(info.rngUnison) * velocity;
    auto notePitch = float(pitch) + tuning;
    auto vel = velocity;
    notes[noteIndices[unison]].noteOn(
      noteId, notePitch, vel, unisonPan[unison], sampleRate, info, param);
  }
}

void DSPCORE_NAME::noteOff(int32_t noteId)
{
  for (size_t i = 0; i < notes.size(); ++i)
    if (notes[i].id == noteId) notes[i].release();
}

void DSPCORE_NAME::fillTransitionBuffer(size_t noteIndex)
{
  isTransitioning = true;

  // Beware the negative overflow. trStop is size_t.
  trStop = trIndex - 1;
  if (trStop >= transitionBuffer.size()) trStop += transitionBuffer.size();

  auto &note = notes[noteIndex];

  for (size_t bufIdx = 0; bufIdx < transitionBuffer.size(); ++bufIdx) {
    auto oscOut = note.process(sampleRate, info);
    auto idx = (trIndex + bufIdx) % transitionBuffer.size();
    auto interp = 1.0f - float(bufIdx) / transitionBuffer.size();

    transitionBuffer[idx][0] += oscOut[0] * interp;
    transitionBuffer[idx][1] += oscOut[1] * interp;
  }
}
