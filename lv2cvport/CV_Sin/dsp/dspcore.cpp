// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_Sin.
//
// CV_Sin is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_Sin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_Sin.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

inline float midiNoteToFrequency(float pitch, float tuning)
{
  return 440.0f * powf(2.0f, ((pitch - 69.0f) * 100.0f + tuning) / 1200.0f);
}

inline float bendToPitch(float bend)
{
  return powf(2.0f, (bend - 0.5f) * 400.0f / 1200.0f);
}

void DSPCore::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  noteStack.reserve(128);
  noteStack.resize(0);

  reset();
  startup();
}

void DSPCore::reset()
{
  noteStack.resize(0);
  interpGain.reset(
    param.value[ParameterID::gain]->getFloat()
    * param.value[ParameterID::boost]->getFloat());
  interpFrequency.reset(0);
  startup();
}

void DSPCore::startup() { phase = 0; }

void DSPCore::setParameters()
{
  using ID = ParameterID::ID;

  if (!noteStack.empty()) {
    lastFreq = noteStack.back().frequency;
  }
  interpFrequency.push(
    lastFreq * bendToPitch(param.value[ParameterID::pitchBend]->getFloat()));
  interpGain.push(param.value[ID::gain]->getFloat() * param.value[ID::boost]->getFloat());
}

void DSPCore::process(
  const size_t length,
  const float *inGain,
  const float *inPitch,
  const float *inPhase,
  float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);
    SmootherCommon<float>::setBufferIndex(i);

    const float freq = interpFrequency.process() * powf(2.0f, inPitch[i] * 32.0f / 12.0f);
    const float gain = interpGain.process() + inGain[i];

    phase = fmodf(phase + float(twopi) * freq / sampleRate + inPhase[i], float(twopi));
    out0[i] = gain * sinf(phase);
  }
}

void DSPCore::noteOn(int32_t noteId, int16_t pitch, float tuning, float velocity)
{
  NoteInfo info;
  info.id = noteId;
  info.frequency = midiNoteToFrequency(pitch, tuning);
  info.velocity = velocity;
  noteStack.push_back(info);
}

void DSPCore::noteOff(int32_t noteId)
{
  auto it = std::find_if(noteStack.begin(), noteStack.end(), [&](const NoteInfo &info) {
    return info.id == noteId;
  });
  if (it == noteStack.end()) return;
  noteStack.erase(it);
}
