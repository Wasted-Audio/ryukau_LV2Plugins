// (c) 2020 Takamitsu Endo
//
// This file is part of CV_PolyLoopEnvelope2.
//
// CV_PolyLoopEnvelope2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_PolyLoopEnvelope2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_PolyLoopEnvelope2.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <vector>

#include "../../common/value.hpp"

#ifdef TEST_BUILD
static const uint32_t kParameterIsAutomable = 0x01;
static const uint32_t kParameterIsBoolean = 0x02;
static const uint32_t kParameterIsInteger = 0x04;
static const uint32_t kParameterIsLogarithmic = 0x08;
#endif

namespace ParameterID {
enum ID {
  gain,

  rate,
  rateKeyFollow,
  rateSlideTime,

  loopStart,
  loopEnd,

  releaseTime,
  releaseCurve,

  s0DecayTime,
  s1DecayTime,

  s0HoldTime,
  s1HoldTime,

  s0Level,
  s1Level,

  s0Curve,
  s1Curve,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::LogScale<double> rate;
  static SomeDSP::LogScale<double> rateSlideTime;
  static SomeDSP::IntScale<double> section;
  static SomeDSP::LinearScale<double> level;
  static SomeDSP::LogScale<double> decay;
  static SomeDSP::LinearScale<double> curve;
};

struct GlobalParameter {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    using LogValue = FloatValue<SomeDSP::LogScale<double>>;

    value[ID::gain]
      = std::make_unique<LinearValue>(1.0, Scales::level, "gain", kParameterIsAutomable);

    value[ID::rate] = std::make_unique<LogValue>(
      Scales::rate.invmap(1.0), Scales::rate, "rate",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::rateKeyFollow] = std::make_unique<IntValue>(
      true, Scales::boolScale, "rateKeyFollow",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::rateSlideTime] = std::make_unique<LogValue>(
      0.0, Scales::rateSlideTime, "rateSlideTime",
      kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::releaseTime] = std::make_unique<LogValue>(
      Scales::decay.invmap(0.01), Scales::decay, "releaseTime",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::releaseCurve] = std::make_unique<LinearValue>(
      0.5, Scales::curve, "releaseCurve", kParameterIsAutomable);

    value[ID::loopStart] = std::make_unique<IntValue>(
      0, Scales::section, "loopStart", kParameterIsAutomable | kParameterIsInteger);
    value[ID::loopEnd] = std::make_unique<IntValue>(
      1, Scales::section, "loopEnd", kParameterIsAutomable | kParameterIsInteger);

    value[ID::s0DecayTime] = std::make_unique<LogValue>(
      Scales::decay.invmap(0.01), Scales::decay, "s0DecayTime",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::s1DecayTime] = std::make_unique<LogValue>(
      Scales::decay.invmap(0.01), Scales::decay, "s1DecayTime",
      kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::s0HoldTime] = std::make_unique<LogValue>(
      Scales::decay.invmap(0.01), Scales::decay, "s0HoldTime",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::s1HoldTime] = std::make_unique<LogValue>(
      Scales::decay.invmap(0.01), Scales::decay, "s1HoldTime",
      kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::s0Level] = std::make_unique<LinearValue>(
      Scales::level.invmap(1.0), Scales::level, "s0Level", kParameterIsAutomable);
    value[ID::s1Level] = std::make_unique<LinearValue>(
      Scales::level.invmap(0.0), Scales::level, "s1Level", kParameterIsAutomable);

    value[ID::s0Curve] = std::make_unique<LinearValue>(
      0.5, Scales::curve, "s0Curve", kParameterIsAutomable);
    value[ID::s1Curve] = std::make_unique<LinearValue>(
      0.5, Scales::curve, "s1Curve", kParameterIsAutomable);
  }

#ifndef TEST_BUILD
  void initParameter(uint32_t index, Parameter &parameter)
  {
    if (index >= value.size()) return;
    value[index]->setParameterRange(parameter);
  }
#endif

  void resetParameter()
  {
    for (auto &val : value) val->setFromNormalized(val->getDefaultNormalized());
  }

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

  enum Preset { presetDefault, Preset_ENUM_LENGTH };
  std::array<const char *, 12> programName{"Default"};

  void initProgramName(uint32_t index, String &programName)
  {
    programName = this->programName[index];
  }

  void loadProgram(uint32_t index)
  {
    switch (index) {
      default:
        resetParameter();
        break;
    }
  }
};
