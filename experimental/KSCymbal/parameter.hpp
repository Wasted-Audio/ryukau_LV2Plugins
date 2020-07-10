// (c) 2020 Takamitsu Endo
//
// This file is part of KSCymbal.
//
// KSCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KSCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KSCymbal.  If not, see <https://www.gnu.org/licenses/>.

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

constexpr uint16_t nOscillator = 16;

namespace ParameterID {
enum ID {
  bypass,

  octave,
  semitone,
  milli,
  equalTemperament,
  pitchA4Hz,
  pitchBend,

  gain,
  boost,

  attack,
  decay,

  minFrequency,
  maxFrequency,

  distance,
  stack,
  seed,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::IntScale<double> octave;
  static SomeDSP::IntScale<double> semitone;
  static SomeDSP::IntScale<double> milli;
  static SomeDSP::IntScale<double> equalTemperament;
  static SomeDSP::IntScale<double> pitchA4Hz;

  static SomeDSP::LogScale<double> gain;
  static SomeDSP::LinearScale<double> boost;

  static SomeDSP::LogScale<double> attack;
  static SomeDSP::LogScale<double> decay;

  static SomeDSP::LogScale<double> frequency;

  static SomeDSP::LogScale<double> distance;
  static SomeDSP::IntScale<double> stack;
  static SomeDSP::IntScale<double> seed;
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

    value[ID::octave] = std::make_unique<IntValue>(
      12, Scales::octave, "octave", kParameterIsAutomable | kParameterIsInteger);
    value[ID::semitone] = std::make_unique<IntValue>(
      120, Scales::semitone, "semitone", kParameterIsAutomable | kParameterIsInteger);
    value[ID::milli] = std::make_unique<IntValue>(
      1000, Scales::milli, "milli", kParameterIsAutomable | kParameterIsInteger);
    value[ID::equalTemperament] = std::make_unique<IntValue>(
      11, Scales::equalTemperament, "equalTemperament",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::pitchA4Hz] = std::make_unique<IntValue>(
      340, Scales::pitchA4Hz, "pitchA4Hz", kParameterIsAutomable | kParameterIsInteger);
    value[ID::pitchBend] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "pitchBend", kParameterIsAutomable);

    value[ID::gain] = std::make_unique<LogValue>(
      0.5, Scales::gain, "gain", kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::boost] = std::make_unique<LinearValue>(
      Scales::boost.invmap(1.0), Scales::boost, "boost", kParameterIsAutomable);

    value[ID::attack] = std::make_unique<LogValue>(
      0.0, Scales::attack, "attack", kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::decay] = std::make_unique<LogValue>(
      0.5, Scales::decay, "decay", kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::minFrequency] = std::make_unique<LogValue>(
      0.0, Scales::frequency, "minFrequency",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::maxFrequency] = std::make_unique<LogValue>(
      Scales::frequency.invmap(200.0), Scales::frequency, "maxFrequency",
      kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::distance] = std::make_unique<LogValue>(
      0.5, Scales::distance, "distance", kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::stack] = std::make_unique<IntValue>(
      22, Scales::stack, "stack", kParameterIsAutomable | kParameterIsInteger);
    value[ID::seed] = std::make_unique<IntValue>(
      0, Scales::seed, "seed", kParameterIsAutomable | kParameterIsInteger);
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
