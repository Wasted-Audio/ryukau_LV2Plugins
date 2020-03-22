// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_ParabolicADEnvelope.
//
// CV_ParabolicADEnvelope is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_ParabolicADEnvelope is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_ParabolicADEnvelope.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

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

  interpGain.push(param.value[ID::gain]->getFloat());
}

void DSPCore::process(const size_t length, float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);
    SmootherCommon<float>::setBufferIndex(i);

    // out0[i] = interpGain.process() * envelope.process();
    out0[i] = envelope.process();
  }
}

void DSPCore::noteOn(
  int32_t noteId, int16_t /* pitch */, float /* tuning */, float /* velocity */)
{
  NoteInfo info;
  info.id = noteId;
  noteStack.push_back(info);

  envelope.reset(
    param.value[ParameterID::attackTime]->getFloat(),
    param.value[ParameterID::attackCurve]->getFloat(),
    param.value[ParameterID::decayTime]->getFloat(),
    param.value[ParameterID::decayCurve]->getFloat());
}

void DSPCore::noteOff(int32_t noteId)
{
  auto it = std::find_if(noteStack.begin(), noteStack.end(), [&](const NoteInfo &info) {
    return info.id == noteId;
  });
  if (it == noteStack.end()) return;
  noteStack.erase(it);
}
