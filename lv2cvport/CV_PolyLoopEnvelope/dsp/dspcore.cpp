// (c) 2020 Takamitsu Endo
//
// This file is part of CV_PolyLoopEnvelope.
//
// CV_PolyLoopEnvelope is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_PolyLoopEnvelope is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_PolyLoopEnvelope.  If not, see <https://www.gnu.org/licenses/>.

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

  interpRate.setSampleRate(sampleRate);

  envelope.setup(sampleRate);

  noteStack.reserve(128);
  noteStack.resize(0);

  reset();
}

void DSPCore::reset()
{
  noteStack.resize(0);
  interpGain.reset(param.value[ParameterID::gain]->getFloat());
  interpRate.reset(0);
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
  float rateSlideTime = param.value[ParameterID::rateSlideTime]->getFloat();
  interpRate.push(param.value[ID::rate]->getFloat() * rateRatio);
  interpRate.setTime(rateSlideTime);

  interpReleaseTime.push(param.value[ID::releaseTime]->getFloat());
  interpReleaseCurve.push(param.value[ID::releaseCurve]->getFloat());

  for (size_t idx = 0; idx < envelope.nSections; ++idx) {
    interpDecayTime[idx].push(param.value[ID::s0DecayTime + idx]->getFloat());
    interpHoldTime[idx].push(param.value[ID::s0HoldTime + idx]->getFloat());
    interpLevel[idx].push(param.value[ID::s0Level + idx]->getFloat());
    interpCurve[idx].push(param.value[ID::s0Curve + idx]->getFloat());
  }
}

void DSPCore::process(const size_t length, const float **inputs, float *out0)
{
  constexpr float gateThreshold = 1e-5f;

  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);

    if (!isGateOpen && inputs[inGate][i] >= gateThreshold) {
      isGateOpen = true;
      envelope.trigger();
    } else if (isGateOpen && inputs[inGate][i] < gateThreshold) {
      isGateOpen = false;
      if (noteStack.size() == 0) envelope.release();
    }

    envelope.set(
      interpRate.process() + powf(2.0f, inputs[inRate][i] * 32.0f / 12.0f),
      interpReleaseTime.process() + fabsf(inputs[inReleaseTime][i]),
      interpReleaseCurve.process() + inputs[inReleaseCurve][i],
      {
        interpDecayTime[0].process() + fabsf(inputs[inDecay0][i]),
        interpDecayTime[1].process() + fabsf(inputs[inDecay1][i]),
        interpDecayTime[2].process() + fabsf(inputs[inDecay2][i]),
        interpDecayTime[3].process() + fabsf(inputs[inDecay3][i]),
        interpDecayTime[4].process() + fabsf(inputs[inDecay4][i]),
        interpDecayTime[5].process() + fabsf(inputs[inDecay5][i]),
        interpDecayTime[6].process() + fabsf(inputs[inDecay6][i]),
        interpDecayTime[7].process() + fabsf(inputs[inDecay7][i]),
      },
      {
        interpHoldTime[0].process() + fabsf(inputs[inHold0][i]),
        interpHoldTime[1].process() + fabsf(inputs[inHold1][i]),
        interpHoldTime[2].process() + fabsf(inputs[inHold2][i]),
        interpHoldTime[3].process() + fabsf(inputs[inHold3][i]),
        interpHoldTime[4].process() + fabsf(inputs[inHold4][i]),
        interpHoldTime[5].process() + fabsf(inputs[inHold5][i]),
        interpHoldTime[6].process() + fabsf(inputs[inHold6][i]),
        interpHoldTime[7].process() + fabsf(inputs[inHold7][i]),
      },
      {
        interpLevel[0].process() + inputs[inLevel0][i],
        interpLevel[1].process() + inputs[inLevel1][i],
        interpLevel[2].process() + inputs[inLevel2][i],
        interpLevel[3].process() + inputs[inLevel3][i],
        interpLevel[4].process() + inputs[inLevel4][i],
        interpLevel[5].process() + inputs[inLevel5][i],
        interpLevel[6].process() + inputs[inLevel6][i],
        interpLevel[7].process() + inputs[inLevel7][i],
      },
      {
        interpCurve[0].process() + inputs[inCurve0][i],
        interpCurve[1].process() + inputs[inCurve1][i],
        interpCurve[2].process() + inputs[inCurve2][i],
        interpCurve[3].process() + inputs[inCurve3][i],
        interpCurve[4].process() + inputs[inCurve4][i],
        interpCurve[5].process() + inputs[inCurve5][i],
        interpCurve[6].process() + inputs[inCurve6][i],
        interpCurve[7].process() + inputs[inCurve7][i],
      });

    out0[i] = (interpGain.process() + inputs[inGain][i]) * envelope.process();
  }
}

void DSPCore::noteOn(int32_t noteId, int16_t pitch, float tuning, float /* velocity */)
{
  using ID = ParameterID::ID;

  NoteInfo info;
  info.id = noteId;
  info.noteRatio = midiNoteToRatio(pitch, tuning, 0.5f);
  noteStack.push_back(info);

  if (param.value[ID::rateKeyFollow]->getInt()) {
    noteRatio = info.noteRatio;
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

  using ID = ParameterID::ID;
  if (param.value[ID::rateKeyFollow]->getInt() && !noteStack.empty()) {
    noteRatio = noteStack.back().noteRatio;
    interpRate.push(param.value[ID::rate]->getFloat() * noteRatio);
  }

  if (noteStack.size() == 0 && !isGateOpen) envelope.release();
}
