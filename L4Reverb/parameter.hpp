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
  innerFeed0 = nDepth1,                      // 256
  d1Feed0 = 2 * nDepth1,                     // 512
  d2Feed0 = 3 * nDepth1,                     // 768
  d3Feed0 = 3 * nDepth1 + nDepth2,           // 832
  d4Feed0 = 3 * nDepth1 + nDepth2 + nDepth3, // 848

  timeMultiply = 3 * nDepth1 + nDepth2 + nDepth3 + nDepth4, // 852
  innerFeedMultiply,
  d1FeedMultiply,
  d2FeedMultiply,
  d3FeedMultiply,
  d4FeedMultiply,

  timeOffsetRange,
  innerFeedOffsetRange,
  d1FeedOffsetRange,
  d2FeedOffsetRange,
  d3FeedOffsetRange,
  d4FeedOffsetRange,

  timeModulation,
  innerFeedModulation,
  d1FeedModulation,
  d2FeedModulation,
  d3FeedModulation,
  d4FeedModulation,

  seed,

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
  static SomeDSP::LogScale<double> timeOffsetRange;
  static SomeDSP::IntScale<double> seed;
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

    value[ID::timeMultiply] = std::make_unique<LogValue>(
      1.0, Scales::timeMultiply, "timeMultiply", kParameterIsAutomable);
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

    value[ID::timeOffsetRange] = std::make_unique<LogValue>(
      Scales::timeOffsetRange.invmap(0.05), Scales::timeOffsetRange, "timeOffsetRange",
      kParameterIsAutomable);
    value[ID::innerFeedOffsetRange] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "innerFeedOffsetRange", kParameterIsAutomable);
    value[ID::d1FeedOffsetRange] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "d1FeedOffsetRange", kParameterIsAutomable);
    value[ID::d2FeedOffsetRange] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "d2FeedOffsetRange", kParameterIsAutomable);
    value[ID::d3FeedOffsetRange] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "d3FeedOffsetRange", kParameterIsAutomable);
    value[ID::d4FeedOffsetRange] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "d4FeedOffsetRange", kParameterIsAutomable);

    value[ID::timeModulation] = std::make_unique<IntValue>(
      0, Scales::boolScale, "timeModulation", kParameterIsAutomable);
    value[ID::innerFeedModulation] = std::make_unique<IntValue>(
      0, Scales::boolScale, "innerFeedModulation", kParameterIsAutomable);
    value[ID::d1FeedModulation] = std::make_unique<IntValue>(
      0, Scales::boolScale, "d1FeedModulation", kParameterIsAutomable);
    value[ID::d2FeedModulation] = std::make_unique<IntValue>(
      0, Scales::boolScale, "d2FeedModulation", kParameterIsAutomable);
    value[ID::d3FeedModulation] = std::make_unique<IntValue>(
      0, Scales::boolScale, "d3FeedModulation", kParameterIsAutomable);
    value[ID::d4FeedModulation] = std::make_unique<IntValue>(
      0, Scales::boolScale, "d4FeedModulation", kParameterIsAutomable);

    value[ID::seed]
      = std::make_unique<IntValue>(0, Scales::seed, "seed", kParameterIsAutomable);

    value[ID::stereoCross] = std::make_unique<LinearValue>(
      Scales::stereoCross.invmap(0.0), Scales::stereoCross, "stereoCross",
      kParameterIsAutomable);
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
    presetAbility,
    presetAdaptability,
    presetAptness,
    presetCapability,
    presetColdHardReverb,
    presetCompetency,
    presetCost,
    presetCreativity,
    presetEfficiency,
    presetExpertise,
    presetLatePeak,
    presetLiability,
    presetMerit,
    presetNarrowTube,
    presetPotential,
    presetProductivity,
    presetProficiency,
    presetProfitability,
    presetResonance,
    presetResponsibility,
    presetSkill,
    presetSusceptibility,
    presetSustainability,
    presetTalent,
    presetTinCan,
    presetViability,
    presetWobblyShort,
    presetYamabiko,

    Preset_ENUM_LENGTH,
  };

  std::array<const char *, 29> programName{
    "Default",        "Ability",      "Adaptability",   "Aptness",        "Capability",
    "ColdHardReverb", "Competency",   "Cost",           "Creativity",     "Efficiency",
    "Expertise",      "LatePeak",     "Liability",      "Merit",          "NarrowTube",
    "Potential",      "Productivity", "Proficiency",    "Profitability",  "Resonance",
    "Responsibility", "Skill",        "Susceptibility", "Sustainability", "Talent",
    "TinCan",         "Viability",    "WobblyShort",    "Yamabiko",
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
