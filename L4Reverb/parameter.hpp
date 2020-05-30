// (c) 2019-2020 Takamitsu Endo
//
// This file is part of L4Reverb.
//
// L4Reverb is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// L4Reverb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with L4Reverb.  If not, see <https://www.gnu.org/licenses/>.

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

constexpr uint16_t nDepth1 = 256;
constexpr uint16_t nDepth2 = 64;
constexpr uint16_t nDepth3 = 16;
constexpr uint16_t nDepth4 = 4;
constexpr uint16_t nDepth = 4;

namespace ParameterID {
enum ID {
  time0 = 0,
  innerFeed0 = nDepth1,
  d1Feed0 = 2 * nDepth1,
  d2Feed0 = 3 * nDepth1,
  d3Feed0 = 3 * nDepth1 + nDepth2,
  d4Feed0 = 3 * nDepth1 + nDepth2 + nDepth3,

  timeMultiply = 3 * nDepth1 + nDepth2 + nDepth3 + nDepth4,
  innerFeedMultiply,
  d1FeedMultiply,
  d2FeedMultiply,
  d3FeedMultiply,
  d4FeedMultiply,

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
  static SomeDSP::LogScale<double> time;
  static SomeDSP::LinearScale<double> feed;
  static SomeDSP::LogScale<double> timeMultiply;
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
    std::string innerFeedLabel("innerFeed");
    std::string d1FeedLabel("d1Feed");
    for (size_t idx = 0; idx < nDepth1; ++idx) {
      auto indexStr = std::to_string(idx);
      value[ID::time0 + idx] = std::make_unique<LogValue>(
        Scales::time.invmap(0.1), Scales::time, (timeLabel + indexStr).c_str(),
        kParameterIsAutomable | kParameterIsLogarithmic);
      value[ID::innerFeed0 + idx] = std::make_unique<LinearValue>(
        0.5, Scales::feed, (innerFeedLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::d1Feed0 + idx] = std::make_unique<LinearValue>(
        0.5, Scales::feed, (d1FeedLabel + indexStr).c_str(), kParameterIsAutomable);
    }

    std::string d2FeedLabel("d2Feed");
    for (size_t idx = 0; idx < nDepth2; ++idx) {
      auto indexStr = std::to_string(idx);
      value[ID::d2Feed0 + idx] = std::make_unique<LinearValue>(
        0.5, Scales::feed, (d2FeedLabel + indexStr).c_str(), kParameterIsAutomable);
    }

    std::string d3FeedLabel("d3Feed");
    for (size_t idx = 0; idx < nDepth3; ++idx) {
      auto indexStr = std::to_string(idx);
      value[ID::d3Feed0 + idx] = std::make_unique<LinearValue>(
        0.5, Scales::feed, (d3FeedLabel + indexStr).c_str(), kParameterIsAutomable);
    }

    std::string d4FeedLabel("d4Feed");
    for (size_t idx = 0; idx < nDepth4; ++idx) {
      auto indexStr = std::to_string(idx);
      value[ID::d4Feed0 + idx] = std::make_unique<LinearValue>(
        0.5, Scales::feed, (d4FeedLabel + indexStr).c_str(), kParameterIsAutomable);
    }

    value[ID::timeMultiply] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "timeMultiply", kParameterIsAutomable);
    value[ID::innerFeedMultiply] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "innerFeedMultiply", kParameterIsAutomable);
    value[ID::d1FeedMultiply] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "d1FeedMultiply", kParameterIsAutomable);
    value[ID::d2FeedMultiply] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "d2FeedMultiply", kParameterIsAutomable);
    value[ID::d3FeedMultiply] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "d3FeedMultiply", kParameterIsAutomable);
    value[ID::d4FeedMultiply] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "d4FeedMultiply", kParameterIsAutomable);

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

  void loadProgram(uint32_t index) override;
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
