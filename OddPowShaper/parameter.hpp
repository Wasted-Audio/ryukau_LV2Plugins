// (c) 2019-2020 Takamitsu Endo
//
// This file is part of OddPowShaper.
//
// OddPowShaper is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OddPowShaper is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OddPowShaper.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../common/dsp/constants.hpp"
#include "../common/dsp/scale.hpp"
#include "../common/parameterinterface.hpp"
#include "../common/value.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifdef TEST_BUILD
static const uint32_t kParameterIsAutomable = 0x01;
static const uint32_t kParameterIsBoolean = 0x02;
static const uint32_t kParameterIsInteger = 0x04;
static const uint32_t kParameterIsLogarithmic = 0x08;
#endif

namespace ParameterID {
enum ID {
  bypass,

  drive,
  boost,
  outputGain,

  order,
  flip,
  inverse,

  oversample,

  smoothness,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::LogScale<double> drive;
  static SomeDSP::LinearScale<double> boost;
  static SomeDSP::LogScale<double> outputGain;
  static SomeDSP::IntScale<double> order;

  static SomeDSP::LogScale<double> smoothness;
};

struct GlobalParameter : public ParameterInterface {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    using LogValue = FloatValue<SomeDSP::LogScale<double>>;
    // using SPolyValue = FloatValue<SomeDSP::SPolyScale<double>>;
    // using DecibelValue = FloatValue<SomeDSP::DecibelScale<double>>;

    value[ID::bypass] = std::make_unique<IntValue>(
      0, Scales::boolScale, "bypass", kParameterIsAutomable | kParameterIsBoolean);

    value[ID::drive]
      = std::make_unique<LogValue>(0.5, Scales::drive, "drive", kParameterIsAutomable);
    value[ID::boost]
      = std::make_unique<LinearValue>(0.0, Scales::boost, "boost", kParameterIsAutomable);
    value[ID::outputGain] = std::make_unique<LogValue>(
      0.5, Scales::outputGain, "outputGain", kParameterIsAutomable);

    value[ID::order] = std::make_unique<IntValue>(
      0, Scales::order, "order", kParameterIsAutomable | kParameterIsInteger);
    value[ID::flip] = std::make_unique<IntValue>(
      true, Scales::boolScale, "flip", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::inverse] = std::make_unique<IntValue>(
      true, Scales::boolScale, "inverse", kParameterIsAutomable | kParameterIsBoolean);

    value[ID::oversample] = std::make_unique<IntValue>(
      true, Scales::boolScale, "oversample", kParameterIsAutomable | kParameterIsBoolean);

    value[ID::smoothness] = std::make_unique<LogValue>(
      0.1, Scales::smoothness, "smoothness", kParameterIsAutomable);
  }

#ifndef TEST_BUILD
  void initParameter(uint32_t index, Parameter &parameter)
  {
    if (index >= value.size()) return;
    value[index]->setParameterRange(parameter);
  }
#endif

  size_t idLength() override { return value.size(); }

  void resetParameter()
  {
    for (auto &val : value) val->setFromNormalized(val->getDefaultNormalized());
  }

  double getNormalized(uint32_t index) const override
  {
    if (index >= value.size()) return 0.0;
    return value[index]->getNormalized();
  }

  double getDefaultNormalized(uint32_t index) const override
  {
    if (index >= value.size()) return 0.0;
    return value[index]->getDefaultNormalized();
  }

  double getFloat(uint32_t index) const override
  {
    if (index >= value.size()) return 0.0;
    return value[index]->getFloat();
  }

  double getInt(uint32_t index) const override
  {
    if (index >= value.size()) return 0.0;
    return value[index]->getInt();
  }

  void setParameterValue(uint32_t index, float raw)
  {
    if (index >= value.size()) return;
    value[index]->setFromFloat(raw);
  }

  double parameterChanged(uint32_t index, float raw) override
  {
    if (index >= value.size()) return 0.0;
    value[index]->setFromFloat(raw);
    return value[index]->getNormalized();
  }

  double updateValue(uint32_t index, float normalized) override
  {
    if (index >= value.size()) return 0.0;
    value[index]->setFromNormalized(normalized);
    return value[index]->getFloat();
  }

  enum Preset {
    presetDefault,

    Preset_ENUM_LENGTH,
  };

  std::array<const char *, 1> programName{
    "Default",
  };

#ifndef TEST_BUILD
  void initProgramName(uint32_t index, String &programName)
  {
    programName = this->programName[index];
  }

  void loadProgram(uint32_t index);
#endif

  void validate()
  {
    for (size_t i = 0; i < value.size(); ++i) {
      if (value[i] == nullptr) {
        std::cout << "PluginError: GlobalParameter::value[" << std::to_string(i)
                  << "] is nullptr. Forgetting initialization?\n";
        std::exit(EXIT_FAILURE);
      }
    }
  }
};
