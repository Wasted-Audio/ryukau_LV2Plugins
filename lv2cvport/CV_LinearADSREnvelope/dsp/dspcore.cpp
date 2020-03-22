// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_LinearADSREnvelope.
//
// CV_LinearADSREnvelope is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_LinearADSREnvelope is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_LinearADSREnvelope.  If not, see <https://www.gnu.org/licenses/>.

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

  envelope.set(
    param.value[ParameterID::attack]->getFloat(),
    param.value[ParameterID::decay]->getFloat(),
    param.value[ParameterID::sustain]->getFloat(),
    param.value[ParameterID::release]->getFloat(), 20000.0f);
}

void DSPCore::process(const size_t length, float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);
    SmootherCommon<float>::setBufferIndex(i);

    out0[i] = interpGain.process() * envelope.process();
  }
}

void DSPCore::noteOn(
  int32_t noteId, int16_t /* pitch */, float /* tuning */, float /* velocity */)
{
  NoteInfo info;
  info.id = noteId;
  noteStack.push_back(info);

  envelope.trigger(
    param.value[ParameterID::attack]->getFloat(),
    param.value[ParameterID::decay]->getFloat(),
    param.value[ParameterID::sustain]->getFloat(),
    param.value[ParameterID::release]->getFloat(), 20000.0f);
}

void DSPCore::noteOff(int32_t noteId)
{
  auto it = std::find_if(noteStack.begin(), noteStack.end(), [&](const NoteInfo &info) {
    return info.id == noteId;
  });
  if (it == noteStack.end()) return;
  noteStack.erase(it);

  if (noteStack.size() == 0) envelope.release();
}
