// (c) 2019-2020 Takamitsu Endo
//
// This file is part of FDNCymbal.
//
// FDNCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// FDNCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with FDNCymbal.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <vector>

#include "value.hpp"

#ifdef TEST_BUILD
static const uint32_t kParameterIsAutomable = 0x01;
static const uint32_t kParameterIsBoolean = 0x02;
static const uint32_t kParameterIsInteger = 0x04;
static const uint32_t kParameterIsLogarithmic = 0x08;
#endif

namespace ParameterID {
enum ID {
  bypass,

  seed,
  retriggerTime,
  retriggerStick,
  retriggerTremolo,
  fdn,
  fdnTime,
  fdnFeedback,
  fdnCascadeMix,
  allpassMix,
  allpass1Saturation,
  allpass1Time,
  allpass1Feedback,
  allpass1HighpassCutoff,
  allpass2Time,
  allpass2Feedback,
  allpass2HighpassCutoff,
  tremoloMix,
  tremoloDepth,
  tremoloFrequency,
  tremoloDelayTime,
  randomTremoloDepth,
  randomTremoloFrequency,
  randomTremoloDelayTime,
  stick,
  stickDecay,
  stickToneMix,
  stickPulseMix,
  stickVelvetMix,

  smoothness,

  gain,
  pitchBend,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::IntScale<double> seed;
  static SomeDSP::LogScale<double> fdnTime;
  static SomeDSP::LogScale<double> fdnFeedback;
  static SomeDSP::LogScale<double> fdnCascadeMix;
  static SomeDSP::LogScale<double> allpassTime;
  static SomeDSP::LogScale<double> allpassFeedback;
  static SomeDSP::LogScale<double> allpassHighpassCutoff;
  static SomeDSP::LogScale<double> tremoloFrequency;
  static SomeDSP::LogScale<double> tremoloDelayTime;
  static SomeDSP::LogScale<double> stickDecay;
  static SomeDSP::LogScale<double> stickToneMix;

  static SomeDSP::LogScale<double> smoothness;

  static SomeDSP::LogScale<double> gain;
};

struct GlobalParameter {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    using LogValue = FloatValue<SomeDSP::LogScale<double>>;

    value[ID::bypass] = std::make_unique<IntValue>(
      false, Scales::boolScale, "bypass", kParameterIsAutomable | kParameterIsBoolean);

    value[ID::seed] = std::make_unique<IntValue>(
      6583421, Scales::seed, "seed", kParameterIsAutomable | kParameterIsInteger);
    value[ID::retriggerTime] = std::make_unique<IntValue>(
      true, Scales::boolScale, "retriggerTime",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::retriggerStick] = std::make_unique<IntValue>(
      false, Scales::boolScale, "retriggerStick",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::retriggerTremolo] = std::make_unique<IntValue>(
      false, Scales::boolScale, "retriggerTremolo",
      kParameterIsAutomable | kParameterIsBoolean);

    value[ID::fdn] = std::make_unique<IntValue>(
      true, Scales::boolScale, "fdn", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::fdnTime] = std::make_unique<LogValue>(
      0.2, Scales::fdnTime, "fdnTime", kParameterIsAutomable);
    value[ID::fdnFeedback] = std::make_unique<LogValue>(
      0.5, Scales::fdnFeedback, "fdnFeedback", kParameterIsAutomable);
    value[ID::fdnCascadeMix] = std::make_unique<LogValue>(
      0.5, Scales::fdnCascadeMix, "fdnCascadeMix", kParameterIsAutomable);

    value[ID::allpassMix] = std::make_unique<LinearValue>(
      0.75, Scales::defaultScale, "allpassMix", kParameterIsAutomable);
    value[ID::allpass1Saturation] = std::make_unique<IntValue>(
      true, Scales::boolScale, "allpass1Saturation",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::allpass1Time] = std::make_unique<LogValue>(
      0.5, Scales::allpassTime, "allpass1Time", kParameterIsAutomable);
    value[ID::allpass1Feedback] = std::make_unique<LogValue>(
      0.75, Scales::allpassFeedback, "allpass1Feedback", kParameterIsAutomable);
    value[ID::allpass1HighpassCutoff] = std::make_unique<LogValue>(
      0.5, Scales::allpassHighpassCutoff, "allpass1HighpassCutoff",
      kParameterIsAutomable);
    value[ID::allpass2Time] = std::make_unique<LogValue>(
      0.5, Scales::allpassTime, "allpass2Time", kParameterIsAutomable);
    value[ID::allpass2Feedback] = std::make_unique<LogValue>(
      0.5, Scales::allpassFeedback, "allpass2Feedback", kParameterIsAutomable);
    value[ID::allpass2HighpassCutoff] = std::make_unique<LogValue>(
      0.5, Scales::allpassHighpassCutoff, "allpass2HighpassCutoff",
      kParameterIsAutomable);

    value[ID::tremoloMix] = std::make_unique<LinearValue>(
      0.2, Scales::defaultScale, "tremoloMix", kParameterIsAutomable);
    value[ID::tremoloDepth] = std::make_unique<LinearValue>(
      0.8, Scales::defaultScale, "tremoloDepth", kParameterIsAutomable);
    value[ID::tremoloFrequency] = std::make_unique<LogValue>(
      0.5, Scales::tremoloFrequency, "tremoloFrequency", kParameterIsAutomable);
    value[ID::tremoloDelayTime] = std::make_unique<LogValue>(
      0.25, Scales::tremoloDelayTime, "tremoloDelayTime", kParameterIsAutomable);
    value[ID::randomTremoloDepth] = std::make_unique<LinearValue>(
      0.35, Scales::defaultScale, "randomTremoloDepth", kParameterIsAutomable);
    value[ID::randomTremoloFrequency] = std::make_unique<LinearValue>(
      0.35, Scales::defaultScale, "randomTremoloFrequency", kParameterIsAutomable);
    value[ID::randomTremoloDelayTime] = std::make_unique<LinearValue>(
      0.35, Scales::defaultScale, "randomTremoloDelayTime", kParameterIsAutomable);

    value[ID::stick] = std::make_unique<IntValue>(
      true, Scales::boolScale, "stick", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::stickDecay] = std::make_unique<LogValue>(
      0.5, Scales::stickDecay, "stickDecay", kParameterIsAutomable);
    value[ID::stickToneMix] = std::make_unique<LogValue>(
      0.5, Scales::stickToneMix, "stickToneMix", kParameterIsAutomable);
    value[ID::stickPulseMix] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "stickPulseMix", kParameterIsAutomable);
    value[ID::stickVelvetMix] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "stickVelvetMix", kParameterIsAutomable);

    value[ID::smoothness] = std::make_unique<LogValue>(
      0.1, Scales::smoothness, "smoothness", kParameterIsAutomable);

    value[ID::gain]
      = std::make_unique<LogValue>(0.5, Scales::gain, "gain", kParameterIsAutomable);
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
    preset0,
    preset11415258,
    preset5711006,
    presetFDNTweak,
    presetGlassy,
    presetLongExcitation,
    presetRandomShortTime,
    presetRetriggerStickOscillator,
    presetSmooth,
    presetTooMuchDelayTime,
    presetTurnRight,

    Preset_ENUM_LENGTH,
  };

  std::array<const char *, 12> programName{
    "Default",
    "0",
    "11415258",
    "5711006",
    "FDNTweak",
    "Glassy",
    "LongExcitation",
    "RandomShortTime",
    "RetriggerStickOscillator",
    "Smooth",
    "TooMuchDelayTime",
    "TurnRight",
  };

  void initProgramName(uint32_t index, String &programName)
  {
    programName = this->programName[index];
  }

  void loadProgram(uint32_t index);
};
