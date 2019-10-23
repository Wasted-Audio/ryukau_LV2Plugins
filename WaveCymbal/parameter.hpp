// (c) 2019 Takamitsu Endo
//
// This file is part of WaveCymbal.
//
// WaveCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// WaveCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with WaveCymbal.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <vector>

#include "value.hpp"

#ifdef TEST_BUILD
static const uint32_t kParameterIsAutomable = 0x01;
static const uint32_t kParameterIsBoolean = 0x02;
static const uint32_t kParameterIsInteger = 0x04;
static const uint32_t kParameterIsLogarithmic = 0x08;
#endif

namespace ParameterID {
enum ID {
  bypass,

  seed,
  randomAmount,
  nCymbal,
  stack,
  decay,
  distance,
  minFrequency,
  maxFrequency,
  bandpassQ,
  damping,
  pulsePosition,
  pulseWidth,
  pickCombFeedback,
  pickCombTime,
  retrigger,
  cutoffMap,
  excitation,
  collision,
  oscType,
  smoothness,

  gain,
  pitchBend,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::IntScale<double> seed;
  static SomeDSP::IntScale<double> nCymbal;
  static SomeDSP::LogScale<double> decay;
  static SomeDSP::LogScale<double> damping;
  static SomeDSP::LogScale<double> minFrequency;
  static SomeDSP::LogScale<double> maxFrequency;
  static SomeDSP::LinearScale<double> bandpassQ;
  static SomeDSP::LogScale<double> distance;
  static SomeDSP::IntScale<double> stack;
  static SomeDSP::LogScale<double> pickCombFeedback;
  static SomeDSP::LogScale<double> pickCombTime;
  static SomeDSP::IntScale<double> oscType;
  static SomeDSP::LogScale<double> smoothness;

  static SomeDSP::LogScale<double> gain;
};

struct GlobalParameter {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    using LogValue = FloatValue<SomeDSP::LogScale<double>>;

    value[ID::bypass] = std::make_unique<IntValue>(
      0, Scales::boolScale, "bypass", kParameterIsAutomable | kParameterIsBoolean);

    value[ID::seed] = std::make_unique<IntValue>(
      6583421, Scales::seed, "seed", kParameterIsAutomable | kParameterIsInteger);
    value[ID::randomAmount] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "randomAmount", kParameterIsAutomable);
    value[ID::nCymbal] = std::make_unique<IntValue>(
      3, Scales::nCymbal, "nCymbal", kParameterIsAutomable | kParameterIsInteger);
    value[ID::stack] = std::make_unique<IntValue>(
      24, Scales::stack, "stack", kParameterIsAutomable | kParameterIsInteger);
    value[ID::decay]
      = std::make_unique<LogValue>(0.5, Scales::decay, "decay", kParameterIsAutomable);
    value[ID::distance] = std::make_unique<LogValue>(
      0.5, Scales::distance, "distance", kParameterIsAutomable);
    value[ID::minFrequency] = std::make_unique<LogValue>(
      0.5, Scales::minFrequency, "minFrequency", kParameterIsAutomable);
    value[ID::maxFrequency] = std::make_unique<LogValue>(
      0.5, Scales::maxFrequency, "maxFrequency", kParameterIsAutomable);
    value[ID::bandpassQ] = std::make_unique<LinearValue>(
      0.5, Scales::bandpassQ, "bandpassQ", kParameterIsAutomable);
    value[ID::damping] = std::make_unique<LogValue>(
      0.5, Scales::damping, "damping", kParameterIsAutomable);
    value[ID::pulsePosition] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "pulsePosition", kParameterIsAutomable);
    value[ID::pulseWidth] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "pulseWidth", kParameterIsAutomable);
    value[ID::pickCombFeedback] = std::make_unique<LogValue>(
      0.5, Scales::pickCombFeedback, "pickCombFeedback", kParameterIsAutomable);
    value[ID::pickCombTime] = std::make_unique<LogValue>(
      0.25, Scales::pickCombTime, "pickCombTime", kParameterIsAutomable);
    value[ID::retrigger] = std::make_unique<IntValue>(
      0, Scales::boolScale, "retrigger", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::cutoffMap] = std::make_unique<IntValue>(
      0, Scales::boolScale, "cutoffMap", kParameterIsAutomable | kParameterIsInteger);
    value[ID::excitation] = std::make_unique<IntValue>(
      1, Scales::boolScale, "excitation", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::collision] = std::make_unique<IntValue>(
      1, Scales::boolScale, "collision", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::oscType] = std::make_unique<IntValue>(
      2, Scales::oscType, "oscType", kParameterIsAutomable | kParameterIsInteger);
    value[ID::smoothness] = std::make_unique<LogValue>(
      0.7, Scales::smoothness, "smoothness", kParameterIsAutomable);

    value[ID::gain]
      = std::make_unique<LogValue>(0.4, Scales::gain, "gain", kParameterIsAutomable);
    value[ID::pitchBend] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "pitchBend", kParameterIsAutomable);
  }

#ifndef TEST_BUILD
  void initParameter(uint32_t index, Parameter &parameter)
  {
    if (index >= value.size()) return;
    value[index]->setParameterRange(parameter);
  }
#endif

  double getParameterValue(uint32_t index) const
  {
    if (index >= value.size()) return 0.0;
    return value[index]->getFloat();
  }

  void setParameterValue(uint32_t index, float raw)
  {
    if (index >= value.size()) return;
    value[index]->setFromFloat(raw);
  }

  double parameterChanged(uint32_t index, float raw)
  {
    if (index >= value.size()) return 0.0;
    value[index]->setFromFloat(raw);
    return value[index]->getNormalized();
  }

  double updateValue(uint32_t index, float normalized)
  {
    if (index >= value.size()) return 0.0;
    value[index]->setFromNormalized(normalized);
    return value[index]->getFloat();
  }
};
