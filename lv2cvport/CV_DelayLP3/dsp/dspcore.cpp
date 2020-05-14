// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_DelayLP3.
//
// CV_DelayLP3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_DelayLP3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_DelayLP3.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

// Approximation of `440 * np.power(2, (x * 130 - 69) / 12)`
// where x is in [0, 1].
inline float mapCutoffHz(float x)
{
  x = std::clamp(x, 0.0f, 1.0f);

  // clang-format off

  return (
      float(7.83898781259972)
    + float(65.32852274310886)  * x
    + float(-75.90021042753838) * x * x
    + float(522.6375210708683)  * x * x * x
  ) / (
      float(0.9266048955405587)
    + float(-2.011103603009115) * x
    + float(1.5092698784262415) * x * x
    + float(-0.38992385750216)  * x * x * x
  );

  // clang-format on
}

void DSPCore::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  delay.setup(sampleRate, Scales::time.getMax());

  reset();
}

void DSPCore::reset()
{
  using ID = ParameterID::ID;

  delay.reset();

  interpTime.reset(param.value[ID::time]->getFloat());
  interpFeedback.reset(param.value[ID::feedback]->getFloat());
  interpLowpassHz.reset(param.value[ID::lowpassHz]->getFloat());
  interpResonance.reset(param.value[ID::resonance]->getFloat());
  interpHighpassHz.reset(param.value[ID::highpassHz]->getFloat());
}

void DSPCore::startup() {}

void DSPCore::setParameters()
{
  using ID = ParameterID::ID;

  interpTime.push(param.value[ID::time]->getFloat());
  interpFeedback.push(param.value[ID::feedback]->getFloat());
  interpLowpassHz.push(param.value[ID::lowpassHz]->getFloat());
  interpResonance.push(param.value[ID::resonance]->getFloat());
  interpHighpassHz.push(param.value[ID::highpassHz]->getFloat());
}

void DSPCore::process(
  const size_t length,
  const float *in0,
  const float *inTime,
  const float *inFeedback,
  const float *inLowpass,
  const float *inHighpass,
  float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    auto lowpassHz = interpLowpassHz.process() + mapCutoffHz(inLowpass[i]);
    auto highpassHz = interpHighpassHz.process() + mapCutoffHz(inHighpass[i]);

    out0[i] = delay.process(
      in0[i], sampleRate,
      std::clamp(interpTime.process() + inTime[i], 0.0f, float(Scales::time.getMax())),
      std::clamp(interpFeedback.process() + inFeedback[i], 0.0f, 1.0f),
      std::clamp(lowpassHz, 0.0f, float(Scales::cutoff.getMax())),
      interpResonance.process(),
      std::clamp(highpassHz, 0.0f, float(Scales::cutoff.getMax())));
  }
}
