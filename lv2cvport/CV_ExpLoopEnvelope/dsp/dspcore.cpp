// (c) 2020 Takamitsu Endo
//
// This file is part of CV_ExpLoopEnvelope.
//
// CV_ExpLoopEnvelope is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_ExpLoopEnvelope is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_ExpLoopEnvelope.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

inline float midiNoteToRatio(float pitch, float tuning, float bend)
{
  return powf(
    2.0f, ((pitch - 69.0f) * 100.0f + tuning + (bend - 0.5f) * 400.0f) / 1200.0f);
}

void DSPCore::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  envelope.setup(sampleRate);

  noteStack.reserve(128);
  noteStack.resize(0);

  reset();
}

void DSPCore::reset()
{
  noteStack.resize(0);
  interpGain.reset(param.value[ParameterID::gain]->getFloat());
  envelope.terminate();
}

void DSPCore::setParameters()
{
  using ID = ParameterID::ID;

  envelope.setLoop(
    param.value[ParameterID::loopStart]->getInt(),
    param.value[ParameterID::loopEnd]->getInt());

  interpGain.push(param.value[ID::gain]->getFloat());

  float rateRatio = param.value[ID::rateKeyFollow]->getInt() ? noteRatio : 1.0f;
  interpRate.push(param.value[ID::rate]->getFloat() * rateRatio);

  interpReleaseTime.push(param.value[ID::releaseTime]->getFloat());

  interpS0DecayTime.push(param.value[ID::s0DecayTime]->getFloat());
  interpS1DecayTime.push(param.value[ID::s1DecayTime]->getFloat());
  interpS2DecayTime.push(param.value[ID::s2DecayTime]->getFloat());
  interpS3DecayTime.push(param.value[ID::s3DecayTime]->getFloat());
  interpS4DecayTime.push(param.value[ID::s4DecayTime]->getFloat());
  interpS5DecayTime.push(param.value[ID::s5DecayTime]->getFloat());
  interpS6DecayTime.push(param.value[ID::s6DecayTime]->getFloat());
  interpS7DecayTime.push(param.value[ID::s7DecayTime]->getFloat());

  interpS0HoldTime.push(param.value[ID::s0HoldTime]->getFloat());
  interpS1HoldTime.push(param.value[ID::s1HoldTime]->getFloat());
  interpS2HoldTime.push(param.value[ID::s2HoldTime]->getFloat());
  interpS3HoldTime.push(param.value[ID::s3HoldTime]->getFloat());
  interpS4HoldTime.push(param.value[ID::s4HoldTime]->getFloat());
  interpS5HoldTime.push(param.value[ID::s5HoldTime]->getFloat());
  interpS6HoldTime.push(param.value[ID::s6HoldTime]->getFloat());
  interpS7HoldTime.push(param.value[ID::s7HoldTime]->getFloat());

  interpS0Level.push(param.value[ID::s0Level]->getFloat());
  interpS1Level.push(param.value[ID::s1Level]->getFloat());
  interpS2Level.push(param.value[ID::s2Level]->getFloat());
  interpS3Level.push(param.value[ID::s3Level]->getFloat());
  interpS4Level.push(param.value[ID::s4Level]->getFloat());
  interpS5Level.push(param.value[ID::s5Level]->getFloat());
  interpS6Level.push(param.value[ID::s6Level]->getFloat());
  interpS7Level.push(param.value[ID::s7Level]->getFloat());
}

void DSPCore::process(const size_t length, const float **inputs, float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);
    SmootherCommon<float>::setBufferIndex(i);

    if (!isGateOpen && inputs[inGate][i] >= 0.1f) {
      isGateOpen = true;
      envelope.trigger();
    } else if (isGateOpen && inputs[inGate][i] < 0.1f) {
      isGateOpen = false;
      if (noteStack.size() == 0) envelope.release();
    }

    envelope.set(
      interpRate.process() + powf(2.0f, inputs[inRate][i] * 32.0f / 12.0f),
      interpReleaseTime.process() + fabsf(inputs[inReleaseTime][i]));
    envelope.setDecayTime(
      interpS0DecayTime.process() + fabsf(inputs[inDecay0][i]),
      interpS1DecayTime.process() + fabsf(inputs[inDecay1][i]),
      interpS2DecayTime.process() + fabsf(inputs[inDecay2][i]),
      interpS3DecayTime.process() + fabsf(inputs[inDecay3][i]),
      interpS4DecayTime.process() + fabsf(inputs[inDecay4][i]),
      interpS5DecayTime.process() + fabsf(inputs[inDecay5][i]),
      interpS6DecayTime.process() + fabsf(inputs[inDecay6][i]),
      interpS7DecayTime.process() + fabsf(inputs[inDecay7][i]));
    envelope.setHoldTime(
      interpS0HoldTime.process() + fabsf(inputs[inHold0][i]),
      interpS1HoldTime.process() + fabsf(inputs[inHold1][i]),
      interpS2HoldTime.process() + fabsf(inputs[inHold2][i]),
      interpS3HoldTime.process() + fabsf(inputs[inHold3][i]),
      interpS4HoldTime.process() + fabsf(inputs[inHold4][i]),
      interpS5HoldTime.process() + fabsf(inputs[inHold5][i]),
      interpS6HoldTime.process() + fabsf(inputs[inHold6][i]),
      interpS7HoldTime.process() + fabsf(inputs[inHold7][i]));
    envelope.setLevel(
      interpS0Level.process() + inputs[inLevel0][i],
      interpS1Level.process() + inputs[inLevel1][i],
      interpS2Level.process() + inputs[inLevel2][i],
      interpS3Level.process() + inputs[inLevel3][i],
      interpS4Level.process() + inputs[inLevel4][i],
      interpS5Level.process() + inputs[inLevel5][i],
      interpS6Level.process() + inputs[inLevel6][i],
      interpS7Level.process() + inputs[inLevel7][i]);

    out0[i] = (interpGain.process() + inputs[inGain][i]) * envelope.process();
  }
}

void DSPCore::noteOn(int32_t noteId, int16_t pitch, float tuning, float /* velocity */)
{
  using ID = ParameterID::ID;

  NoteInfo info;
  info.id = noteId;
  noteStack.push_back(info);

  if (param.value[ID::rateKeyFollow]->getInt()) {
    noteRatio = midiNoteToRatio(pitch, tuning, 0.5f);
    interpRate.push(param.value[ID::rate]->getFloat() * noteRatio);
  }

  envelope.trigger();
}

void DSPCore::noteOff(int32_t noteId)
{
  auto it = std::find_if(noteStack.begin(), noteStack.end(), [&](const NoteInfo &info) {
    return info.id == noteId;
  });
  if (it == noteStack.end()) return;
  noteStack.erase(it);

  if (noteStack.size() == 0 && !isGateOpen) envelope.release();
}
