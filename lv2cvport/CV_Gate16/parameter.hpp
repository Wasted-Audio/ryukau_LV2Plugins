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

constexpr size_t nGate = 16;

namespace ParameterID {
enum ID {
  gain1 = 0,
  delay1 = 16,
  type1 = 32,

  masterGain = 48,
  delayMultiply,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::LinearScale<double> gain;
  static SomeDSP::LinearScale<double> delay;
  static SomeDSP::IntScale<double> type;
  static SomeDSP::LinearScale<double> masterGain;
  static SomeDSP::LogScale<double> delayMultiply;
};

struct GlobalParameter {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    using LogValue = FloatValue<SomeDSP::LogScale<double>>;

    std::string gainLabel("gain");
    std::string delayLabel("delay");
    std::string typeLabel("type");
    for (size_t idx = 0; idx < nGate; ++idx) {
      auto indexStr = std::to_string(idx + 1);
      value[ID::gain1 + idx] = std::make_unique<LinearValue>(
        1.0, Scales::gain, (gainLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::delay1 + idx] = std::make_unique<LinearValue>(
        0.0, Scales::delay, (delayLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::type1 + idx] = std::make_unique<IntValue>(
        1, Scales::type, (typeLabel + indexStr).c_str(),
        kParameterIsAutomable | kParameterIsInteger);
    }

    value[ID::masterGain] = std::make_unique<LinearValue>(
      1.0, Scales::masterGain, "masterGain", kParameterIsAutomable);
    value[ID::delayMultiply] = std::make_unique<LogValue>(
      Scales::delayMultiply.invmap(1.0), Scales::delayMultiply, "delayMultiply",
      kParameterIsAutomable);
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
