// (c) 2020 Takamitsu Endo
//
// This file is part of BubbleSynth.
//
// BubbleSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BubbleSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BubbleSynth.  If not, see <https://www.gnu.org/licenses/>.

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

void DSPCORE_NAME::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  noteStack.reserve(128);
  noteStack.resize(0);

  for (auto &osc : bubbleOsc) osc.setup(sampleRate);

  reset();
}

void DSPCORE_NAME::reset()
{
  noteStack.resize(0);

  for (auto &osc : bubbleOsc) osc.reset();
}

void DSPCORE_NAME::startup() {}

void DSPCORE_NAME::setParameters(float tempo) { using ID = ParameterID::ID; }

void DSPCORE_NAME::process(const size_t length, float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);

    float bubble = 0.0f;
    for (auto &osc : bubbleOsc) bubble += osc.process();

    out0[i] = bubble;
  }
}

void DSPCORE_NAME::noteOn(
  int32_t noteId, int16_t /* pitch */, float /* tuning */, float /* velocity */)
{
  using ID = ParameterID::ID;

  NoteInfo info;
  info.id = noteId;
  noteStack.push_back(info);

  for (uint32_t idx = 0; idx < nOscillator; ++idx) {
    bubbleOsc[idx].prepare(
      sampleRate, param.value[ID::gain0 + idx]->getFloat(),
      param.value[ID::radius0 + idx]->getFloat(), param.value[ID::xi0 + idx]->getFloat(),
      param.value[ID::attack0 + idx]->getFloat(),
      param.value[ID::decay0 + idx]->getFloat(),
      param.value[ID::delay0 + idx]->getFloat());
  }
}

void DSPCORE_NAME::noteOff(int32_t noteId)
{
  auto it = std::find_if(noteStack.begin(), noteStack.end(), [&](const NoteInfo &info) {
    return info.id == noteId;
  });
  if (it == noteStack.end()) return;
  noteStack.erase(it);
}
