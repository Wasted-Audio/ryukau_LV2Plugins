// (c) 2020 Takamitsu Endo
//
// This file is part of NoiseTester.
//
// NoiseTester is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// NoiseTester is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with NoiseTester.  If not, see <https://www.gnu.org/licenses/>.

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

  reset();
}

void DSPCORE_NAME::reset()
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  noteStack.resize(0);
}

void DSPCORE_NAME::startup() {}

void DSPCORE_NAME::setParameters(float tempo)
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  SmootherCommon<float>::setTime(pv[ID::smooth]->getFloat());

  smoothGain.push(pv[ID::gain]->getFloat());

  hardclip = pv[ID::hardclip]->getInt();
  noiseType = static_cast<NoiseGenerator::Type>(pv[ID::type]->getInt());

  rateLimiter.setRate(
    1.0f / pv[ID::attack]->getFloat(), -1.0f / pv[ID::release]->getFloat());
}

void DSPCORE_NAME::process(const size_t length, float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  std::array<float, 16> delayTime;
  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);

    float sig
      = rateLimiter.process(gate) * smoothGain.process() * noiseOsc.process(noiseType);
    out0[i] = hardclip ? std::clamp<float>(sig, -1, 1) : sig;
  }
}

void DSPCORE_NAME::noteOn(int32_t noteId, int16_t pitch, float tuning, float velocity)
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  NoteInfo info;
  info.id = noteId;
  noteStack.push_back(info);

  gate = velocityMap.map(velocity);
  noiseOsc.setSeed(pv[ID::seed]->getInt());
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
