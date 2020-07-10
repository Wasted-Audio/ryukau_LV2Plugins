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

void DSPCORE_NAME::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  noteStack.reserve(128);
  noteStack.resize(0);

  // 10 msec + 1 sample transition time.
  transitionBuffer.resize(1 + size_t(sampleRate * 0.01), 0.0f);

  reset();
}

void DSPCORE_NAME::reset()
{
  noteStack.resize(0);

  using ID = ParameterID::ID;
  auto &pv = param.value;

  rng.seed(pv[ID::seed]->getInt());
  cymbal.reset();

  smoothMasterGain.reset(pv[ID::gain]->getFloat() * pv[ID::boost]->getFloat());
}

void DSPCORE_NAME::startup() {}

void DSPCORE_NAME::setParameters(float tempo)
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  smoothMasterGain.push(pv[ID::gain]->getFloat() * pv[ID::boost]->getFloat());
}

void DSPCORE_NAME::process(const size_t length, float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  std::uniform_real_distribution<float> dist(-0.5f, 0.5f);

  float frame, sig;
  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);

    if (noiseCounter > 0) {
      sig = dist(rng);
      --noiseCounter;
    } else {
      sig = 0;
    }
    for (auto &cmb : comb) sig -= cmb.process(sig);
    frame = velocity * smoothMasterGain.process() * cymbal.process(sig * gate.process());

    if (isTransitioning) {
      frame += transitionBuffer[trIndex];
      transitionBuffer[trIndex] = 0.0f;
      trIndex = (trIndex + 1) % transitionBuffer.size();
      if (trIndex == trStop) isTransitioning = false;
    }

    out0[i] = frame;
  }
}

void DSPCORE_NAME::noteOn(int32_t noteId, int16_t pitch, float tuning, float velocity)
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  NoteInfo info;
  info.id = noteId;
  noteStack.push_back(info);

  fillTransitionBuffer();

  // TODO: Implement retrigger.
  // if (pv[ID::retrigger]->getInt()) rng.seed(pv[ID::seed]->getInt());

  this->velocity = velocityMap.map(velocity);

  noiseCounter = int32_t(pv[ID::attack]->getFloat() * sampleRate);
  gate.reset(sampleRate, pv[ID::attack]->getFloat());

  std::uniform_real_distribution<float> distCombTime(0.0f, 0.002f);
  for (auto &cmb : comb) cmb.setTime(sampleRate, distCombTime(rng));

  cymbal.trigger(
    rng, sampleRate, pv[ID::minFrequency]->getFloat(), pv[ID::maxFrequency]->getFloat(),
    pv[ID::distance]->getFloat());
}

void DSPCORE_NAME::noteOff(int32_t noteId)
{
  auto it = std::find_if(noteStack.begin(), noteStack.end(), [&](const NoteInfo &info) {
    return info.id == noteId;
  });
  if (it == noteStack.end()) return;
  noteStack.erase(it);
}

void DSPCORE_NAME::fillTransitionBuffer()
{
  isTransitioning = true;

  // Beware the negative overflow. trStop is size_t.
  trStop = trIndex - 1;
  if (trStop >= transitionBuffer.size()) trStop += transitionBuffer.size();

  float gain = velocity * smoothMasterGain.getValue();
  float sig;
  for (size_t bufIdx = 0; bufIdx < transitionBuffer.size(); ++bufIdx) {
    sig = 0;
    for (auto &cmb : comb) sig -= cmb.process(sig);
    sig = gain * cymbal.process(sig * gate.process());
    auto idx = (trIndex + bufIdx) % transitionBuffer.size();
    auto interp = 1.0f - float(bufIdx) / transitionBuffer.size();
    transitionBuffer[idx] += sig * interp;
  }
}
