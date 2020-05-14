// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_HoldFilter.
//
// CV_HoldFilter is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_HoldFilter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_HoldFilter.  If not, see <https://www.gnu.org/licenses/>.

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
  dcSuppressor.reset();
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
  interpPulseWidth.push(param.value[ID::pulseWidth]->getFloat());
  interpEdge.push(param.value[ID::edge]->getFloat());

  filter.type = param.value[ID::filterType]->getInt();
  highpass = param.value[ID::highpass]->getInt();
}

uint32_t DSPCore::getLatency()
{
  if (overSample == 1) {
    if (filter.type == 0) // Naive.
      return 0;
    else if (filter.type == 1) // 4 Point PolyBLEP Residual.
      return 1;
    else if (filter.type == 2) // 6 Point PolyBLEP Residual.
      return 1;
    else if (filter.type == 3) // 8 Point PolyBLEP Residual.
      return 2;
  } else { // overSample == 4.
    if (filter.type == 0)
      return 0;
    else if (filter.type == 1)
      return 4;
    else if (filter.type == 2)
      return 6;
    else if (filter.type == 3)
      return 8;
  }
  return 0;
}

void DSPCore::process(
  const size_t length,
  const float *in0,
  const float *inCutoff,
  const float *inResonance,
  const float *inPulseWidth,
  float *out0)
{
  SmootherCommon<float>::setBufferSize(length * overSample);

  for (size_t i = 0; i < length; ++i) {
    inputInterp.push(in0[i]);

    for (size_t j = 0; j < overSample; ++j) {
      float cutoff = interpCutoff.process();
      float resonance = interpResonance.process();
      float pulseWidth = interpPulseWidth.process();
      float edge = interpEdge.process();

      cutoff = std::clamp<float>(
        cutoff + 440.0f * powf(2.0f, (fabsf(inCutoff[i]) * 130.0f - 69.0f) / 12.0f), 0.0f,
        48000.0f);
      resonance = std::clamp<float>(resonance + inResonance[i], 0.0f, 1.0f);
      pulseWidth = std::clamp<float>(pulseWidth + inPulseWidth[i], 0.1f, 1.9f);

      filter.set(sampleRate * overSample, cutoff, resonance, pulseWidth, edge);
      output = interpGain.process()
        * filter.process(inputInterp.process(float(j) / overSample));

      decimationLowpass.push(output);
    }
    output = overSample == 1 ? output : decimationLowpass.output();
    out0[i] = highpass ? dcSuppressor.process(output) : output;
  }
}
