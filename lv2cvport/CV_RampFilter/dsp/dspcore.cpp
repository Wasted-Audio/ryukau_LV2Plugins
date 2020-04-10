// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_RampFilter.
//
// CV_RampFilter is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_RampFilter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_RampFilter.  If not, see <https://www.gnu.org/licenses/>.

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
  filter.reset();
  decimationLowpass.reset();
}

void DSPCore::startup() {}

void DSPCore::setParameters()
{
  using ID = ParameterID::ID;

  overSample = param.value[ID::overSampling]->getInt() ? 4 : 1;

  SmootherCommon<float>::setSampleRate(sampleRate * overSample);
  SmootherCommon<float>::setTime(0.01f);

  interpGain.push(param.value[ID::gain]->getFloat());
  interpCutoff.push(param.value[ID::cutoff]->getFloat());
  interpResonance.push(param.value[ID::resonance]->getFloat());
  interpRampLimit.push(param.value[ID::rampLimit]->getFloat());
  interpBias.push(param.value[ID::bias]->getFloat());
  interpBiasTuning.push(param.value[ID::biasTuning]->getFloat());

  filter.highpass = param.value[ID::highpass]->getInt();
}

void DSPCore::process(
  const size_t length,
  const float *in0,
  const float *inCutoff,
  const float *inResonance,
  float *out0)
{
  SmootherCommon<float>::setBufferSize(length * overSample);

  for (size_t i = 0; i < length; ++i) {
    inputInterp.push(in0[i]);

    for (size_t j = 0; j < overSample; ++j) {
      SmootherCommon<float>::setBufferIndex(overSample * i);

      float cutoff = interpCutoff.process();
      float resonance = interpResonance.process();
      float limit = interpRampLimit.process();
      float bias = interpBias.process();
      float biasTuning = interpBiasTuning.process();

      cutoff = std::clamp<float>(
        cutoff + 440.0f * powf(2.0f, (fabsf(inCutoff[i]) * 130.0f - 69.0f) / 12.0f), 0.0f,
        48000.0f);
      resonance = std::clamp<float>(resonance + inResonance[i], 0.0f, 1.0f);

      filter.set(sampleRate * overSample, cutoff, resonance, limit, bias, biasTuning);
      output = interpGain.process()
        * filter.process(inputInterp.process(float(j) / overSample));

      decimationLowpass.push(output);
    }

    out0[i] = overSample == 1 ? output : decimationLowpass.output();
  }
}
