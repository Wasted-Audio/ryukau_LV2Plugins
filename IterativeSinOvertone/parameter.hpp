// (c) 2019 Takamitsu Endo
//
// This file is part of IterativeSinOvertone.
//
// IterativeSinOvertone is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// IterativeSinOvertone is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with IterativeSinOvertone.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <iostream>

#include "dsp/scale.hpp"
#include "value.hpp"

#ifdef TEST_BUILD
static const uint32_t kParameterIsAutomable = 0x01;
static const uint32_t kParameterIsBoolean = 0x02;
static const uint32_t kParameterIsInteger = 0x04;
static const uint32_t kParameterIsLogarithmic = 0x08;
#endif

constexpr size_t nOvertone = 64;

namespace ParameterID {
enum ID {
  bypass,

  aliasing,
  masterOctave,
  pitchMultiply,
  pitchModulo,

  seed,
  randomRetrigger,
  randomGainAmount,
  randomFrequencyAmount,

  gain,
  gainBoost,

  // 64 for each.
  attack0,
  decay0 = 75,
  overtone0 = 139,
  phase0 = 203,

  // Do not add parameter here.

  nVoice = 267,
  smoothness,

  pitchBend,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::LinearScale<double> masterOctave;
  static SomeDSP::LinearScale<double> equalTemperament;
  static SomeDSP::LinearScale<double> pitchMultiply;
  static SomeDSP::LinearScale<double> pitchModulo;

  static SomeDSP::IntScale<double> seed;
  static SomeDSP::LogScale<double> randomAmount;

  static SomeDSP::LogScale<double> gain;
  static SomeDSP::LinearScale<double> gainBoost;

  static SomeDSP::LogScale<double> envelopeA;
  static SomeDSP::LogScale<double> envelopeD;
  static SomeDSP::DecibelScale<double> gainDecibel;
  static SomeDSP::LinearScale<double> phase;

  static SomeDSP::IntScale<double> nVoice;
  static SomeDSP::LogScale<double> smoothness;
};

struct GlobalParameter {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    using LogValue = FloatValue<SomeDSP::LogScale<double>>;
    // using SPolyValue = FloatValue<SomeDSP::SPolyScale<double>>;
    using DecibelValue = FloatValue<SomeDSP::DecibelScale<double>>;

    value[ID::bypass] = std::make_unique<IntValue>(
      0, Scales::boolScale, "bypass", kParameterIsAutomable | kParameterIsBoolean);

    value[ID::aliasing] = std::make_unique<IntValue>(
      0, Scales::boolScale, "aliasing", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::masterOctave] = std::make_unique<LinearValue>(
      0.5, Scales::masterOctave, "masterOctave",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::pitchMultiply] = std::make_unique<LinearValue>(
      0.25, Scales::pitchMultiply, "pitchMultiply", kParameterIsAutomable);
    value[ID::pitchModulo] = std::make_unique<LinearValue>(
      0.0, Scales::pitchModulo, "pitchModulo", kParameterIsAutomable);

    value[ID::seed] = std::make_unique<IntValue>(
      0, Scales::seed, "seed", kParameterIsAutomable | kParameterIsInteger);
    value[ID::randomRetrigger] = std::make_unique<IntValue>(
      0, Scales::boolScale, "randomRetrigger",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::randomGainAmount] = std::make_unique<LogValue>(
      0.0, Scales::randomAmount, "randomGainAmount", kParameterIsAutomable);
    value[ID::randomFrequencyAmount] = std::make_unique<LogValue>(
      0.0, Scales::randomAmount, "randomFrequencyAmount", kParameterIsAutomable);

    value[ID::gain]
      = std::make_unique<LogValue>(0.5, Scales::gain, "gain", kParameterIsAutomable);
    value[ID::gainBoost] = std::make_unique<LinearValue>(
      0.0, Scales::gainBoost, "gainBoost", kParameterIsAutomable);

    std::string attackLabel("attack");
    std::string curveLabel("curve");
    std::string overtoneLabel("overtone");
    std::string phaseLabel("phase");
    for (size_t i = 0; i < nOvertone; ++i) {
      auto indexStr = std::to_string(i);
      value[ID::attack0 + i] = std::make_unique<LogValue>(
        0.0, Scales::envelopeA, (attackLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::decay0 + i] = std::make_unique<LogValue>(
        0.5, Scales::envelopeD, (curveLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::overtone0 + i] = std::make_unique<DecibelValue>(
        Scales::gainDecibel.invmap(1.0 / (i + 1)), Scales::gainDecibel,
        (overtoneLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::phase0 + i] = std::make_unique<LinearValue>(
        0.0, Scales::phase, (phaseLabel + indexStr).c_str(), kParameterIsAutomable);
    }

    value[ID::nVoice] = std::make_unique<IntValue>(
      5, Scales::nVoice, "nVoice", kParameterIsAutomable | kParameterIsInteger);
    value[ID::smoothness] = std::make_unique<LogValue>(
      0.1, Scales::smoothness, "smoothness", kParameterIsAutomable);

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

  enum Preset {
    presetDefault,

    Preset_ENUM_LENGTH,
  };

  std::array<const char *, 35> programName{
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
