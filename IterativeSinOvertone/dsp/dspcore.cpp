// (c) 2019 Takamitsu Endo
//
// This file is part of IterativeSinOvertone.
//
// IterativeSinOvertone is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// IterativeSinOvertone is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with IterativeSinOvertone.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

#include <iostream>

#if INSTRSET >= 10
#define NOTE_NAME Note_AVX512
#define DSPCORE_NAME DSPCore_AVX512
#elif INSTRSET >= 8
#define NOTE_NAME Note_AVX2
#define DSPCORE_NAME DSPCore_AVX2
#elif INSTRSET >= 5
#define NOTE_NAME Note_SSE41
#define DSPCORE_NAME DSPCore_SSE41
#elif INSTRSET == 2
#define NOTE_NAME Note_SSE2
#define DSPCORE_NAME DSPCore_SSE2
#else
#error Unsupported instruction set
#endif

inline float clamp(float value, float min, float max)
{
  return (value < min) ? min : (value > max) ? max : value;
}

inline float midiNoteToFrequency(float pitch, float tuning)
{
  return 440.0f * powf(2.0f, ((pitch - 69.0f) * 100.0f + tuning) / 1200.0f);
}

// Using fmod because if equalTemperament == 1, this returns 2^121 which is too large.
inline float semiToPitch(float semi, float equalTemperament)
{
  return fmodf(powf(2.0f, semi / equalTemperament), 1e5f);
}

inline float paramMilliToPitch(float semi, float milli, float equalTemperament)
{
  return fmodf(
    powf(2.0f, (1000.0f * floorf(semi) + milli) / (equalTemperament * 1000.0f)), 1e5f);
}

// https://en.wikipedia.org/wiki/Cent_(music)#Piecewise_linear_approximation
inline float centApprox(float cent) { return 1.0f + 0.0005946f * cent; }

template<typename Sample> void NOTE_NAME<Sample>::setup(Sample sampleRate)
{
  this->sampleRate = sampleRate;

  oscillator.setup(sampleRate);
}

template<typename Sample>
void NOTE_NAME<Sample>::noteOn(
  int32_t noteId,
  Sample normalizedKey,
  Sample frequency,
  Sample velocity,
  GlobalParameter &param,
  White<float> &rng)
{
  using ID = ParameterID::ID;

  frequency
    *= somepow<Sample>(2, somefloor<Sample>(param.value[ID::masterOctave]->getFloat()));

  state = NoteState::active;
  id = noteId;
  this->normalizedKey = normalizedKey;
  this->frequency = frequency;
  this->velocity = velocity;

  const float nyquist = sampleRate / 2;
  const bool aliasing = param.value[ID::aliasing]->getInt();
  const Sample randGainAmt = 3 * param.value[ID::randomGainAmount]->getFloat();
  const Sample randFreqAmt = param.value[ID::randomFrequencyAmount]->getFloat();
  const Sample pitchMultiply = param.value[ID::pitchMultiply]->getFloat();
  const Sample pitchModulo = param.value[ID::pitchModulo]->getFloat();

  Vec16f overtonePitch(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

  size_t idxOffset = 0;
  for (size_t i = 0; i < oscillatorSize; ++i) {
    Vec16f rndPt = randFreqAmt
      * Vec16f(rng.process(), rng.process(), rng.process(), rng.process(), rng.process(),
               rng.process(), rng.process(), rng.process(), rng.process(), rng.process(),
               rng.process(), rng.process(), rng.process(), rng.process(), rng.process(),
               rng.process());
    Vec16f modPt = pitchMultiply * (rndPt + overtonePitch + rndPt * overtonePitch);
    if (pitchModulo != 0) // Modulo operation. modf isn't available in vcl.
      modPt = modPt - pitchModulo * floor(modPt / pitchModulo);
    oscillator.frequency[i] = frequency * modPt;

    overtonePitch += 16.0f;

    for (size_t j = 0; j < 16; ++j) {
      oscillator.phase[i].insert(j, param.value[ID::phase0 + idxOffset]->getFloat());
      oscillator.attack[i].insert(j, param.value[ID::attack0 + idxOffset]->getFloat());
      oscillator.decay[i].insert(j, param.value[ID::decay0 + idxOffset]->getFloat());
      oscillator.gain[i].insert(j, param.value[ID::overtone0 + idxOffset]->getFloat());
      idxOffset += 1;
    }

    Vec16f rndGn(
      rng.process(), rng.process(), rng.process(), rng.process(), rng.process(),
      rng.process(), rng.process(), rng.process(), rng.process(), rng.process(),
      rng.process(), rng.process(), rng.process(), rng.process(), rng.process(),
      rng.process());
    oscillator.gain[i] *= (1.0f + randGainAmt * rndGn);

    if (!aliasing) {
      oscillator.gain[i]
        = select(oscillator.frequency[i] >= nyquist, 0, oscillator.gain[i]);
    }
  }

  oscillator.setup(sampleRate);
}

template<typename Sample> void NOTE_NAME<Sample>::release()
{
  if (state == NoteState::rest) return;
  state = NoteState::release;
}

template<typename Sample> void NOTE_NAME<Sample>::rest() { state = NoteState::rest; }

template<typename Sample> Sample NOTE_NAME<Sample>::process()
{
  if (state == NoteState::rest) return 0;

  float sig = oscillator.process();
  if (oscillator.isTerminated()) rest();

  gain = velocity;

  return gain * sig;
}

void DSPCORE_NAME::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  LinearSmoother<float>::setSampleRate(sampleRate);
  LinearSmoother<float>::setTime(0.04f);

  for (auto &note : notes) note.setup(sampleRate);

  // 2 msec + 1 sample transition time.
  transitionBuffer.resize(1 + size_t(sampleRate * 0.005), 0.0f);

  startup();
}

void DSPCORE_NAME::reset()
{
  for (auto &note : notes) note.rest();
  lastNoteFreq = 1.0f;

  startup();
}

void DSPCORE_NAME::startup() { rng.seed = param.value[ParameterID::seed]->getInt(); }

void DSPCORE_NAME::setParameters()
{
  using ID = ParameterID::ID;

  LinearSmoother<float>::setTime(param.value[ID::smoothness]->getFloat());

  interpMasterGain.push(
    param.value[ID::gain]->getFloat() * param.value[ID::gainBoost]->getFloat());

  nVoice = 1 << param.value[ID::nVoice]->getInt();
  if (nVoice > notes.size()) nVoice = notes.size();
}

void DSPCORE_NAME::process(const size_t length, float *out0, float *out1)
{
  LinearSmoother<float>::setBufferSize(length);

  float sample;
  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);

    sample = 0.0f;

    for (auto &note : notes) {
      if (note.state == NoteState::rest) continue;
      sample += note.process();
    }

    if (isTransitioning) {
      sample += transitionBuffer[trIndex];
      transitionBuffer[trIndex] = 0.0f;
      trIndex = (trIndex + 1) % transitionBuffer.size();
      if (trIndex == trStop) isTransitioning = false;
    }

    const auto masterGain = interpMasterGain.process();
    out0[i] = masterGain * sample;
    out1[i] = masterGain * sample;
  }
}

void DSPCORE_NAME::noteOn(int32_t identifier, int16_t pitch, float tuning, float velocity)
{
  size_t noteIdx = 0;
  size_t mostSilent = 0;
  float gain = 16 * oscillatorSize;
  for (; noteIdx < nVoice; ++noteIdx) {
    if (notes[noteIdx].id == identifier) break;
    if (notes[noteIdx].state == NoteState::rest) break;
    if (notes[noteIdx].oscillator.getDecayGain() < gain) {
      gain = notes[noteIdx].oscillator.getDecayGain();
      mostSilent = noteIdx;
    }
  }
  if (noteIdx >= nVoice) {
    isTransitioning = true;

    noteIdx = mostSilent;

    // Beware the negative overflow. trStop is size_t.
    trStop = trIndex - 1;
    if (trStop >= transitionBuffer.size()) trStop += transitionBuffer.size();

    for (size_t bufIdx = 0; bufIdx < transitionBuffer.size(); ++bufIdx) {
      if (notes[noteIdx].state == NoteState::rest) {
        trStop = trIndex + bufIdx;
        if (trStop >= transitionBuffer.size()) trStop -= transitionBuffer.size();
        break;
      }

      auto sample = notes[noteIdx].process();
      auto idx = (trIndex + bufIdx) % transitionBuffer.size();
      auto interp = 1.0f - float(bufIdx) / transitionBuffer.size();

      transitionBuffer[idx] += sample * interp;
    }
  }

  if (param.value[ParameterID::randomRetrigger]->getInt())
    rng.seed = param.value[ParameterID::seed]->getInt();

  auto normalizedKey = float(pitch) / 127.0f;
  lastNoteFreq = midiNoteToFrequency(pitch, tuning);
  notes[noteIdx].noteOn(identifier, normalizedKey, lastNoteFreq, velocity, param, rng);
}

void DSPCORE_NAME::noteOff(int32_t noteId)
{
  size_t i = 0;
  for (; i < notes.size(); ++i) {
    if (notes[i].id == noteId) break;
  }
  if (i >= notes.size()) return;

  notes[i].release();
}
