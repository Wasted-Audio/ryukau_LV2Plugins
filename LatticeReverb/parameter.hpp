// (c) 2019-2020 Takamitsu Endo
//
// This file is part of LatticeReverb.
//
// LatticeReverb is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LatticeReverb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LatticeReverb.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../common/parameterinterface.hpp"
#include "../common/value.hpp"

#include <iostream>
#include <memory>
#include <vector>

#ifdef TEST_BUILD
static const uint32_t kParameterIsAutomable = 0x01;
static const uint32_t kParameterIsBoolean = 0x02;
static const uint32_t kParameterIsInteger = 0x04;
static const uint32_t kParameterIsLogarithmic = 0x08;
#endif

constexpr size_t nestingDepth = 16;

namespace ParameterID {
enum ID {
  time0 = 0,
  outerFeed0 = 16,
  innerFeed0 = 32,

  timeOffset0 = 48,
  outerFeedOffset0 = 64,
  innerFeedOffset0 = 80,

  timeLfoAmount0 = 96,
  lowpassCutoff0 = 112,

  timeMultiply = 128,
  outerFeedMultiply,
  innerFeedMultiply,

  timeOffsetMultiply,
  outerFeedOffsetMultiply,
  innerFeedOffsetMultiply,

  timeLfoLowpass,

  stereoCross,
  stereoSpread,

  dry,
  wet,

  smoothness,
  bypass,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::LinearScale<double> defaultScale;
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> multiply;
  static SomeDSP::LogScale<double> time;
  static SomeDSP::LinearScale<double> feed;
  static SomeDSP::LinearScale<double> timeOffset;
  static SomeDSP::LinearScale<double> feedOffset;
  static SomeDSP::LogScale<double> timeLfoLowpas;
  static SomeDSP::LinearScale<double> stereoCross;
  static SomeDSP::LogScale<double> gain;
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

    std::string timeLabel("time");
    std::string outerFeedLabel("outerFeed");
    std::string innerFeedLabel("innerFeed");

    std::string timeOffsetLabel("timeOffset");
    std::string outerFeedOffsetLabel("outerFeedOffset");
    std::string innerFeedOffsetLabel("innerFeedOffset");

    std::string timeLfoAmountLabel("timeLfoAmount");
    std::string lowpassCutoffLabel("lowpassCutoff");

    for (size_t idx = 0; idx < nestingDepth; ++idx) {
      auto indexStr = std::to_string(idx);
      value[ID::time0 + idx] = std::make_unique<LogValue>(
        Scales::time.invmap(0.1), Scales::time, (timeLabel + indexStr).c_str(),
        kParameterIsAutomable | kParameterIsLogarithmic);
      value[ID::outerFeed0 + idx] = std::make_unique<LinearValue>(
        0.5, Scales::feed, (outerFeedLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::innerFeed0 + idx] = std::make_unique<LinearValue>(
        0.5, Scales::feed, (innerFeedLabel + indexStr).c_str(), kParameterIsAutomable);

      value[ID::timeOffset0 + idx] = std::make_unique<LinearValue>(
        0.5, Scales::timeOffset, (timeOffsetLabel + indexStr).c_str(),
        kParameterIsAutomable);
      value[ID::outerFeedOffset0 + idx] = std::make_unique<LinearValue>(
        0.5, Scales::feedOffset, (outerFeedOffsetLabel + indexStr).c_str(),
        kParameterIsAutomable);
      value[ID::innerFeedOffset0 + idx] = std::make_unique<LinearValue>(
        0.5, Scales::feedOffset, (innerFeedOffsetLabel + indexStr).c_str(),
        kParameterIsAutomable);

      value[ID::timeLfoAmount0 + idx] = std::make_unique<LogValue>(
        0.0, Scales::time, (timeLfoAmountLabel + indexStr).c_str(),
        kParameterIsAutomable);

      value[ID::lowpassCutoff0 + idx] = std::make_unique<LinearValue>(
        1.0, Scales::defaultScale, (lowpassCutoffLabel + indexStr).c_str(),
        kParameterIsAutomable);
    }

    value[ID::timeMultiply] = std::make_unique<LinearValue>(
      1.0, Scales::multiply, "timeMultiply", kParameterIsAutomable);
    value[ID::outerFeedMultiply] = std::make_unique<LinearValue>(
      1.0, Scales::multiply, "outerFeedMultiply", kParameterIsAutomable);
    value[ID::innerFeedMultiply] = std::make_unique<LinearValue>(
      1.0, Scales::multiply, "innerFeedMultiply", kParameterIsAutomable);

    value[ID::timeOffsetMultiply] = std::make_unique<LinearValue>(
      0.05, Scales::multiply, "timeOffsetMultiply", kParameterIsAutomable);
    value[ID::outerFeedOffsetMultiply] = std::make_unique<LinearValue>(
      1.0, Scales::multiply, "outerFeedOffsetMultiply", kParameterIsAutomable);
    value[ID::innerFeedOffsetMultiply] = std::make_unique<LinearValue>(
      1.0, Scales::multiply, "innerFeedOffsetMultiply", kParameterIsAutomable);

    value[ID::timeLfoLowpass] = std::make_unique<LogValue>(
      Scales::timeLfoLowpas.invmap(0.01), Scales::timeLfoLowpas, "timeLfoLowpass",
      kParameterIsAutomable);

    value[ID::stereoCross] = std::make_unique<LinearValue>(
      0.0, Scales::stereoCross, "stereoCross", kParameterIsAutomable);
    value[ID::stereoSpread] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "stereoSpread", kParameterIsAutomable);

    value[ID::dry]
      = std::make_unique<LogValue>(0.5, Scales::gain, "dry", kParameterIsAutomable);
    value[ID::wet]
      = std::make_unique<LogValue>(0.5, Scales::gain, "wet", kParameterIsAutomable);

    value[ID::smoothness] = std::make_unique<LogValue>(
      0.5, Scales::smoothness, "smoothness", kParameterIsAutomable);
    value[ID::bypass] = std::make_unique<IntValue>(
      0, Scales::boolScale, "bypass", kParameterIsAutomable | kParameterIsBoolean);
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
    presetBadPhase,
    presetBarrel,
    presetBoing,
    presetColdPlace,
    presetDamped,
    presetHappyLittleTrees,
    presetInterleave,
    presetLateToParty,
    presetLongLongReverb,
    presetMessingUp,
    presetModulateTimeBaseMultiplier,
    presetNoisyModulation,
    presetPerhapsHall,
    presetPipe,
    presetR2L,
    presetRightBehind,
    presetSadMic,
    presetSober,
    presetWindy,

    Preset_ENUM_LENGTH,
  };

  std::array<const char *, 20> programName{
    "Default",
    "BadPhase",
    "Barrel",
    "Boing",
    "ColdPlace",
    "Damped",
    "HappyLittleTrees",
    "Interleave",
    "LateToParty",
    "LongLongReverb",
    "MessingUp",
    "ModulateTimeBaseMultiplier",
    "NoisyModulation",
    "PerhapsHall",
    "Pipe",
    "R2L",
    "RightBehind",
    "SadMic",
    "Sober",
    "Windy",
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
