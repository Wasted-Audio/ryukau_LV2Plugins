// (c) 2020 Takamitsu Endo
//
// This file is part of BubbleSynth.
//
// BubbleSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BubbleSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BubbleSynth.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include "../../common/parameterinterface.hpp"
#include "../../common/value.hpp"

#ifdef TEST_BUILD
static const uint32_t kParameterIsAutomable = 0x01;
static const uint32_t kParameterIsBoolean = 0x02;
static const uint32_t kParameterIsInteger = 0x04;
static const uint32_t kParameterIsLogarithmic = 0x08;
#endif

constexpr uint32_t nOscillator = 16;

namespace ParameterID {
enum ID {
  radius0 = 0,
  gain0 = nOscillator,
  xi0 = 2 * nOscillator,
  attack0 = 3 * nOscillator,
  decay0 = 4 * nOscillator,
  delay0 = 5 * nOscillator,

  bypass = 6 * nOscillator,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;
  static SomeDSP::LogScale<double> radius;
  static SomeDSP::LogScale<double> gain;
  static SomeDSP::LinearScale<double> xi;
  static SomeDSP::LogScale<double> attack;
  static SomeDSP::LogScale<double> decay;
  static SomeDSP::LogScale<double> delay;
};

struct GlobalParameter : public ParameterInterface {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    using LogValue = FloatValue<SomeDSP::LogScale<double>>;

    std::string radiusLabel("radius");
    std::string gainLabel("gain");
    std::string xiLabel("xi");
    std::string attackLabel("attack");
    std::string decayLabel("decay");
    std::string delayLabel("delay");
    for (size_t idx = 0; idx < nOscillator; ++idx) {
      auto indexStr = std::to_string(idx);
      value[ID::radius0 + idx] = std::make_unique<LogValue>(
        0.5, Scales::radius, (radiusLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::gain0 + idx] = std::make_unique<LogValue>(
        0.5, Scales::gain, (gainLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::xi0 + idx] = std::make_unique<LinearValue>(
        0.0, Scales::xi, (xiLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::attack0 + idx] = std::make_unique<LogValue>(
        0.5, Scales::attack, (attackLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::decay0 + idx] = std::make_unique<LogValue>(
        0.5, Scales::decay, (decayLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::delay0 + idx] = std::make_unique<LogValue>(
        0.0, Scales::delay, (delayLabel + indexStr).c_str(), kParameterIsAutomable);
    }

    value[ID::bypass] = std::make_unique<IntValue>(
      false, Scales::boolScale, "bypass", kParameterIsAutomable | kParameterIsBoolean);
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

  void validate()
  {
    for (size_t i = 0; i < value.size(); ++i) {
      if (value[i] == nullptr) {
        std::cerr << "PluginError: GlobalParameter::value[" << std::to_string(i)
                  << "] is nullptr. Forgetting initialization?\n";
        std::exit(EXIT_FAILURE);
      }
    }
  }
};
