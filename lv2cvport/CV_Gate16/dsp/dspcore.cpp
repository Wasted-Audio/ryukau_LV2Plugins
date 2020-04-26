// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_Gate16.
//
// CV_Gate16 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_Gate16 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_Gate16.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

void DSPCore::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  noteStack.reserve(128);
  noteStack.resize(0);

  reset();
}

void DSPCore::reset()
{
  noteStack.resize(0);
  for (auto &gate : gates) gate.reset();
}

void DSPCore::setParameters()
{
  using ID = ParameterID::ID;

  bool isNoteOn = noteStack.size() != 0;
  for (size_t idx = 0; idx < nGate; ++idx) {
    gates[idx].gain.push(param.value[ID::gain1 + idx]->getFloat());
    gates[idx].setType(param.value[ID::type1 + idx]->getInt(), isNoteOn);
  }

  interpMasterGain.push(param.value[ID::masterGain]->getFloat());
}

void DSPCore::process(const size_t length, float **outputs)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);
    SmootherCommon<float>::setBufferIndex(i);

    const float gain = interpMasterGain.process();

    for (size_t idx = 0; idx < nGate; ++idx)
      outputs[idx][i] = gain * gates[idx].process();
  }
}

void DSPCore::noteOn(
  int32_t noteId, int16_t /* pitch */, float /* tuning */, float /* velocity */)
{
  NoteInfo info;
  info.id = noteId;
  noteStack.push_back(info);

  auto delayMul = param.value[ParameterID::delayMultiply]->getFloat();
  for (size_t idx = 0; idx < gates.size(); ++idx)
    gates[idx].trigger(
      sampleRate, delayMul * param.value[ParameterID::delay1 + idx]->getFloat());
}

void DSPCore::noteOff(int32_t noteId)
{
  auto it = std::find_if(noteStack.begin(), noteStack.end(), [&](const NoteInfo &info) {
    return info.id == noteId;
  });
  if (it == noteStack.end()) return;
  noteStack.erase(it);

  if (noteStack.size() == 0)
    for (auto &gate : gates) gate.release();
}
