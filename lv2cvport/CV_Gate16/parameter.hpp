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
  type,

  masterGain,

  gain1,
  gain2,
  gain3,
  gain4,
  gain5,
  gain6,
  gain7,
  gain8,
  gain9,
  gain10,
  gain11,
  gain12,
  gain13,
  gain14,
  gain15,
  gain16,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> type;
  static SomeDSP::LinearScale<double> defaultScale;
  static SomeDSP::LinearScale<double> masterGain;
};

struct GlobalParameter {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    // using LogValue = FloatValue<SomeDSP::LogScale<double>>;

    value[ID::type]
      = std::make_unique<IntValue>(1, Scales::type, "type", kParameterIsAutomable);
    value[ID::masterGain] = std::make_unique<LinearValue>(
      1.0, Scales::masterGain, "masterGain", kParameterIsAutomable);

    value[ID::gain1] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain1", kParameterIsAutomable);
    value[ID::gain2] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain2", kParameterIsAutomable);
    value[ID::gain3] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain3", kParameterIsAutomable);
    value[ID::gain4] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain4", kParameterIsAutomable);
    value[ID::gain5] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain5", kParameterIsAutomable);
    value[ID::gain6] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain6", kParameterIsAutomable);
    value[ID::gain7] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain7", kParameterIsAutomable);
    value[ID::gain8] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain8", kParameterIsAutomable);
    value[ID::gain9] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain9", kParameterIsAutomable);
    value[ID::gain10] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain10", kParameterIsAutomable);
    value[ID::gain11] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain11", kParameterIsAutomable);
    value[ID::gain12] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain12", kParameterIsAutomable);
    value[ID::gain13] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain13", kParameterIsAutomable);
    value[ID::gain14] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain14", kParameterIsAutomable);
    value[ID::gain15] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain15", kParameterIsAutomable);
    value[ID::gain16] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "gain16", kParameterIsAutomable);
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
