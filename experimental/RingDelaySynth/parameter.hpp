// (c) 2020 Takamitsu Endo
//
// This file is part of RingDelaySynth.
//
// RingDelaySynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RingDelaySynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RingDelaySynth.  If not, see <https://www.gnu.org/licenses/>.

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

constexpr int16_t nDelay = 16;

namespace ParameterID {
enum ID {
  bypass,

  frequency0 = 1,

  gain = 17,
  smooth,

  noiseAttack,
  noiseDecay,
  noiseGain,

  ringPropagation,
  ringLowpassCutoff,
  ringHighpassCutoff,

  pitchDecay,
  pitchDecayAmount,
  ksLowpassFrequency,
  ksLowpassDecay,
  ksLowpassDecayAmount,
  ksFeed,

  pitchBend,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::LogScale<double> frequency;

  static SomeDSP::LogScale<double> gain;
  static SomeDSP::LogScale<double> attack;
  static SomeDSP::LogScale<double> decay;
  static SomeDSP::LogScale<double> ringHighpassCutoff;
  static SomeDSP::LinearScale<double> feed;
  static SomeDSP::LogScale<double> smooth;
};

struct GlobalParameter : public ParameterInterface {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    using LogValue = FloatValue<SomeDSP::LogScale<double>>;

    value[ID::bypass] = std::make_unique<IntValue>(
      false, Scales::boolScale, "bypass", kParameterIsAutomable | kParameterIsBoolean);

    std::string frequencyLabel("frequency");
    for (size_t idx = 0; idx < nDelay; ++idx) {
      auto indexStr = std::to_string(idx);
      value[ID::frequency0 + idx] = std::make_unique<LogValue>(
        0.5, Scales::frequency, (frequencyLabel + indexStr).c_str(),
        kParameterIsAutomable);
    }

    value[ID::gain] = std::make_unique<LogValue>(
      0.5, Scales::gain, "gain", kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::smooth] = std::make_unique<LogValue>(
      0.5, Scales::smooth, "smooth", kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::noiseAttack] = std::make_unique<LogValue>(
      0.5, Scales::attack, "noiseAttack",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::noiseDecay] = std::make_unique<LogValue>(
      0.5, Scales::decay, "noiseDecay", kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::noiseGain] = std::make_unique<LogValue>(
      0.5, Scales::gain, "noiseGain", kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::ringPropagation] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "ringPropagation", kParameterIsAutomable);
    value[ID::ringLowpassCutoff] = std::make_unique<LogValue>(
      0.0, Scales::frequency, "ringLowpassCutoff",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::ringHighpassCutoff] = std::make_unique<LogValue>(
      0.0, Scales::ringHighpassCutoff, "ringHighpassCutoff",
      kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::pitchDecay] = std::make_unique<LogValue>(
      0.5, Scales::decay, "pitchDecay", kParameterIsAutomable);
    value[ID::pitchDecayAmount] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "pitchDecayAmount", kParameterIsAutomable);
    value[ID::ksLowpassFrequency] = std::make_unique<LogValue>(
      0.5, Scales::frequency, "ksLowpassFrequency", kParameterIsAutomable);
    value[ID::ksLowpassDecay] = std::make_unique<LogValue>(
      0.5, Scales::decay, "ksLowpassDecay", kParameterIsAutomable);
    value[ID::ksLowpassDecayAmount] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "ksLowpassDecayAmount", kParameterIsAutomable);
    value[ID::ksFeed]
      = std::make_unique<LinearValue>(0.5, Scales::feed, "ksFeed", kParameterIsAutomable);

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
