// (c) 2020 Takamitsu Endo
//
// This file is part of RingDelaySynth.
//
// RingDelaySynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RingDelaySynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RingDelaySynth.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

#include <random>

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

inline float
notePitchToFrequency(float notePitch, float equalTemperament = 12.0f, float a4Hz = 440.0f)
{
  return a4Hz * powf(2.0f, (notePitch - 69.0f) / equalTemperament);
}

void DSPCORE_NAME::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.1f);

  noteStack.reserve(128);
  noteStack.resize(0);

  ringDelay.setup(sampleRate, 0.01f);
  ksDelay.setup(sampleRate);

  reset();
}

void DSPCORE_NAME::reset()
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  noteStack.resize(0);

  ringDelay.reset();
  ksDelay.reset(pv[ID::pitchDecayAmount]->getFloat(), pv[ID::ksFeed]->getFloat());

  Vec16f delayTime;
  for (int i = 0; i < nDelay; ++i)
    delayTime.insert(i, 1.0f / pv[ID::frequency0 + i]->getFloat());
  smoothDelayTime.reset(delayTime);

  smoothMasterGain.reset(pv[ID::gain]->getFloat());
  smoothPropagation.reset(pv[ID::ringPropagation]->getFloat());
}

void DSPCORE_NAME::startup() {}

void DSPCORE_NAME::setParameters(float tempo)
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  SmootherCommon<float>::setTime(pv[ID::smooth]->getFloat());

  Vec16f delayTime;
  for (int i = 0; i < nDelay; ++i)
    delayTime.insert(i, 1.0f / pv[ID::frequency0 + i]->getFloat());
  smoothDelayTime.push(delayTime);

  smoothMasterGain.push(pv[ID::gain]->getFloat());
  smoothNoiseGain.push(pv[ID::noiseGain]->getFloat());
  smoothPropagation.push(pv[ID::ringPropagation]->getFloat());

  ringDelay.push(
    sampleRate, pv[ID::ringLowpassCutoff]->getFloat(),
    pv[ID::ringHighpassCutoff]->getFloat());
  ksDelay.push(pv[ID::pitchDecayAmount]->getFloat(), pv[ID::ksFeed]->getFloat());
}

void DSPCORE_NAME::process(const size_t length, float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  std::array<float, 16> delayTime;
  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);

    float sig = gate * smoothNoiseGain.process() * noiseOsc.process();
    sig = ringDelay.process(
      sampleRate, sig, smoothDelayTime.process(), smoothPropagation.process());
    out0[i] = smoothMasterGain.process() * ksDelay.process(sampleRate, sig);
  }
}

void DSPCORE_NAME::noteOn(int32_t noteId, int16_t pitch, float tuning, float velocity)
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  NoteInfo info;
  info.id = noteId;
  noteStack.push_back(info);

  gate = velocity;
  noiseOsc.prepare(
    sampleRate, pv[ID::noiseAttack]->getFloat(), pv[ID::noiseDecay]->getFloat());

  ksDelay.prepare(
    sampleRate, notePitchToFrequency(pitch - 24 + tuning),
    mapCutoff(pv[ID::pitchDecayAmount]->getFloat()), pv[ID::pitchDecay]->getFloat(),
    pv[ID::ksLowpassFrequency]->getFloat(),
    mapCutoff(pv[ID::ksLowpassDecayAmount]->getFloat()),
    pv[ID::ksLowpassDecay]->getFloat());
}

void DSPCORE_NAME::noteOff(int32_t noteId)
{
  auto it = std::find_if(noteStack.begin(), noteStack.end(), [&](const NoteInfo &info) {
    return info.id == noteId;
  });
  if (it == noteStack.end()) return;
  noteStack.erase(it);

  if (noteStack.size() > 0) return;
  gate = 0.0f;
}
