// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_PTRTrapezoid.
//
// CV_PTRTrapezoid is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_PTRTrapezoid is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_PTRTrapezoid.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

inline float midiNoteToFrequency(float pitch, float tuning)
{
  return 440.0f * powf(2.0f, ((pitch - 69.0f) * 100.0f + tuning) / 1200.0f);
}

inline float paramToPitch(float semi, float milli, float bend)
{
  return powf(2.0f, semi / 12.0f + milli / 12000.0f + (bend - 0.5f) / 3.0f);
}

void DSPCore::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  oscillator.setup(sampleRate);

  noteStack.reserve(128);
  noteStack.resize(0);

  reset();
  startup();
}

void DSPCore::reset()
{
  noteStack.resize(0);
  interpFrequency.reset(0);
  interpGain.reset(
    param.value[ParameterID::gain]->getFloat()
    * param.value[ParameterID::boost]->getFloat());
  interpPulseWidth.reset(param.value[ParameterID::pulseWidth]->getFloat());
  interpSlope.reset(param.value[ParameterID::slope]->getFloat());
  interpSlopeMul.reset(0);
  startup();
}

void DSPCore::startup() { oscillator.reset(); }

void DSPCore::setParameters()
{
  using ID = ParameterID::ID;

  if (!noteStack.empty()) lastFreq = noteStack.back().frequency;
  interpFrequency.push(
    lastFreq
    * paramToPitch(
      param.value[ParameterID::oscSemi]->getFloat() - 60,
      param.value[ParameterID::oscMilli]->getFloat(),
      param.value[ParameterID::pitchBend]->getFloat()));
  interpGain.push(param.value[ID::gain]->getFloat() * param.value[ID::boost]->getFloat());
  interpPulseWidth.push(param.value[ParameterID::pulseWidth]->getFloat());
  interpSlope.push(param.value[ParameterID::slope]->getFloat());
  interpSlopeMul.push(param.value[ID::slopeMultiply]->getFloat());
}

void DSPCore::process(
  const size_t length,
  const float *inGain,
  const float *inOscPitch,
  const float *inPhaseMod,
  const float *inPulseWidth,
  const float *inOscSlope,
  float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);
    SmootherCommon<float>::setBufferIndex(i);

    oscillator.setFreq(
      interpFrequency.process() * powf(2.0f, inOscPitch[i] * 32.0f / 12.0f));
    oscillator.addPhase(inPhaseMod[i]);
    oscillator.setPulseWidth(interpPulseWidth.process() + inPulseWidth[i]);
    oscillator.setSlope(interpSlope.process() + interpSlopeMul.process() * inOscSlope[i]);
    out0[i] = (interpGain.process() + inGain[i]) * oscillator.process();
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
