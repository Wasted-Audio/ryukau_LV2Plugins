// (c) 2019-2020 Takamitsu Endo
//
// This file is part of EsPhaser.
//
// EsPhaser is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// EsPhaser is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with EsPhaser.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <iostream>

#include "../common/dsp/scale.hpp"
#include "../common/value.hpp"

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

  mix,
  frequency,
  freqSpread,
  feedback,
  range,
  min,
  phase,
  stereoOffset,
  cascadeOffset,
  stage,

  smoothness,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::LogScale<double> frequency;
  static SomeDSP::LinearScale<double> feedback;
  static SomeDSP::LogScale<double> range;
  static SomeDSP::LinearScale<double> phase;
  static SomeDSP::LinearScale<double> cascadeOffset;
  static SomeDSP::IntScale<double> stage;

  static SomeDSP::LogScale<double> smoothness;
};

struct GlobalParameter {
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

    value[ID::mix] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "mix", kParameterIsAutomable);
    value[ID::frequency] = std::make_unique<LogValue>(
      0.5, Scales::frequency, "frequency", kParameterIsAutomable);
    value[ID::freqSpread] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "freqSpread", kParameterIsAutomable);
    value[ID::feedback] = std::make_unique<LinearValue>(
      0.5, Scales::feedback, "feedback", kParameterIsAutomable);
    value[ID::range]
      = std::make_unique<LogValue>(1.0, Scales::range, "range", kParameterIsAutomable);
    value[ID::min]
      = std::make_unique<LogValue>(0.0, Scales::range, "min", kParameterIsAutomable);
    value[ID::phase]
      = std::make_unique<LinearValue>(0.0, Scales::phase, "phase", kParameterIsAutomable);
    value[ID::stereoOffset] = std::make_unique<LinearValue>(
      0.5, Scales::phase, "stereoOffset", kParameterIsAutomable);
    value[ID::cascadeOffset] = std::make_unique<LinearValue>(
      Scales::cascadeOffset.invmap(SomeDSP::twopi / 16.0), Scales::cascadeOffset,
      "cascadeOffset", kParameterIsAutomable);
    value[ID::stage] = std::make_unique<IntValue>(
      15, Scales::stage, "stage", kParameterIsAutomable | kParameterIsInteger);

    value[ID::smoothness] = std::make_unique<LogValue>(
      Scales::smoothness.invmap(0.35), Scales::smoothness, "smoothness",
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

  enum Preset {
    presetDefault,
    presetAutomateCasOffset,
    presetAutomateMin,
    presetFeedback,
    presetHallucinogen95,
    presetResonator,
    presetSharp,
    presetSlowBend,
    presetStage4096,
    presetSubtle,
    presetThick,
    presetThisPhaserIsTooResourceHungry,

    Preset_ENUM_LENGTH,
  };

  std::array<const char *, 12> programName{
    "Default",        "AutomateCasOffset",
    "AutomateMin",    "Feedback",
    "Hallucinogen95", "Resonator",
    "Sharp",          "SlowBend",
    "Stage4096",      "Subtle",
    "Thick",          "ThisPhaserIsTooResourceHungry",
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
