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

enum GateType { typeTrigger, typeGate, typeDC };

void DSPCore::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  noteStack.reserve(128);
  noteStack.resize(0);

  reset();
}

void DSPCore::reset() { noteStack.resize(0); }

void DSPCore::setParameters()
{
  using ID = ParameterID::ID;

  if (param.value[ID::type]->getInt() == typeDC) gate = 1.0f;

  interpMasterGain.push(param.value[ID::masterGain]->getFloat());

  interpGain1.push(param.value[ID::gain1]->getFloat());
  interpGain2.push(param.value[ID::gain2]->getFloat());
  interpGain3.push(param.value[ID::gain3]->getFloat());
  interpGain4.push(param.value[ID::gain4]->getFloat());
  interpGain5.push(param.value[ID::gain5]->getFloat());
  interpGain6.push(param.value[ID::gain6]->getFloat());
  interpGain7.push(param.value[ID::gain7]->getFloat());
  interpGain8.push(param.value[ID::gain8]->getFloat());
  interpGain9.push(param.value[ID::gain9]->getFloat());
  interpGain10.push(param.value[ID::gain10]->getFloat());
  interpGain11.push(param.value[ID::gain11]->getFloat());
  interpGain12.push(param.value[ID::gain12]->getFloat());
  interpGain13.push(param.value[ID::gain13]->getFloat());
  interpGain14.push(param.value[ID::gain14]->getFloat());
  interpGain15.push(param.value[ID::gain15]->getFloat());
  interpGain16.push(param.value[ID::gain16]->getFloat());
}

void DSPCore::process(const size_t length, float **outputs)
{
  SmootherCommon<float>::setBufferSize(length);

  bool trigger = param.value[ParameterID::type]->getInt() == typeTrigger;
  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);
    SmootherCommon<float>::setBufferIndex(i);

    float gain = gate * interpMasterGain.process();

    outputs[0][i] = gain * interpGain1.process();
    outputs[1][i] = gain * interpGain2.process();
    outputs[2][i] = gain * interpGain3.process();
    outputs[3][i] = gain * interpGain4.process();
    outputs[4][i] = gain * interpGain5.process();
    outputs[5][i] = gain * interpGain6.process();
    outputs[6][i] = gain * interpGain7.process();
    outputs[7][i] = gain * interpGain8.process();
    outputs[8][i] = gain * interpGain9.process();
    outputs[9][i] = gain * interpGain10.process();
    outputs[10][i] = gain * interpGain11.process();
    outputs[11][i] = gain * interpGain12.process();
    outputs[12][i] = gain * interpGain13.process();
    outputs[13][i] = gain * interpGain14.process();
    outputs[14][i] = gain * interpGain15.process();
    outputs[15][i] = gain * interpGain16.process();

    if (trigger && gate > 0.0f) gate = 0.0f;
  }
}

void DSPCore::noteOn(
  int32_t noteId, int16_t /* pitch */, float /* tuning */, float /* velocity */)
{
  NoteInfo info;
  info.id = noteId;
  noteStack.push_back(info);

  if (param.value[ParameterID::type]->getInt() != typeDC) gate = 1.0f;
}

void DSPCore::noteOff(int32_t noteId)
{
  auto it = std::find_if(noteStack.begin(), noteStack.end(), [&](const NoteInfo &info) {
    return info.id == noteId;
  });
  if (it == noteStack.end()) return;
  noteStack.erase(it);

  if (noteStack.size() == 0 && param.value[ParameterID::type]->getInt() == typeGate)
    gate = 0.0f;
}
