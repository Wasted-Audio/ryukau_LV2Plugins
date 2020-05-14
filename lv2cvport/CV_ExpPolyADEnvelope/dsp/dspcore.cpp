// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_ExpPolyADEnvelope.
//
// CV_ExpPolyADEnvelope is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_ExpPolyADEnvelope is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_ExpPolyADEnvelope.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

void DSPCore::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  reset();
}

void DSPCore::reset()
{
  interpGain.reset(param.value[ParameterID::gain]->getFloat());
  envelope.terminate();
}

void DSPCore::setParameters()
{
  interpGain.push(param.value[ParameterID::gain]->getFloat());
}

void DSPCore::process(const size_t length, float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);

    out0[i] = interpGain.process() * envelope.process();
  }
}

void DSPCore::noteOn(
  int32_t /* noteId */, int16_t /* pitch */, float /* tuning */, float /* velocity */)
{
  envelope.reset(
    sampleRate, param.value[ParameterID::attack]->getFloat(),
    param.value[ParameterID::curve]->getFloat());
}

void DSPCore::noteOff(int32_t /* noteId */) {}
