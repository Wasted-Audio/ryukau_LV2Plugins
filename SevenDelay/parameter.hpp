// (c) 2019-2020 Takamitsu Endo
//
// This file is part of SevenDelay.
//
// SevenDelay is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SevenDelay is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SevenDelay.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <vector>

#include "../common/dsp/scale.hpp"
#include "../common/value.hpp"

constexpr double maxDelayTime = 8.0;
constexpr double maxToneFrequency = 20000.0;
constexpr double minDCKillFrequency = 5.0;

namespace ParameterID {
enum ID {
  bypass,
  time,
  feedback,
  offset,
  wetMix,
  dryMix,
  tempoSync,
  negativeFeedback,
  lfoTimeAmount,
  lfoToneAmount,
  lfoFrequency,
  lfoShape,
  lfoInitialPhase,
  lfoHold,
  smoothness,
  inSpread,
  inPan,
  outSpread,
  outPan,
  toneCutoff,
  toneQ,
  dckill,
  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;
  static SomeDSP::LogScale<double> time;
  static SomeDSP::SPolyScale<double> offset;
  static SomeDSP::LogScale<double> lfoTimeAmount;
  static SomeDSP::LogScale<double> lfoToneAmount;
  static SomeDSP::LogScale<double> lfoFrequency;
  static SomeDSP::LogScale<double> lfoShape;
  static SomeDSP::LinearScale<double> lfoInitialPhase;
  static SomeDSP::LogScale<double> smoothness;
  static SomeDSP::LogScale<double> toneCutoff;
  static SomeDSP::LogScale<double> toneQ;
  static SomeDSP::LogScale<double> toneMix; // internal
  static SomeDSP::LogScale<double> dckill;
  static SomeDSP::LogScale<double> dckillMix; // internal
};

struct GlobalParameter {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    using LogValue = FloatValue<SomeDSP::LogScale<double>>;
    using SPolyValue = FloatValue<SomeDSP::SPolyScale<double>>;
    // using DecibelValue = FloatValue<SomeDSP::DecibelScale<double>>;

    value[ID::bypass] = std::make_unique<IntValue>(
      0.0, Scales::boolScale, "bypass", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::time]
      = std::make_unique<LogValue>(0.5, Scales::time, "time", kParameterIsAutomable);
    value[ID::feedback] = std::make_unique<LinearValue>(
      0.625, Scales::defaultScale, "feedback", kParameterIsAutomable);
    value[ID::offset] = std::make_unique<SPolyValue>(
      0.5, Scales::offset, "offset", kParameterIsAutomable);
    value[ID::wetMix] = std::make_unique<LinearValue>(
      0.75, Scales::defaultScale, "wetMix", kParameterIsAutomable);
    value[ID::dryMix] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "dryMix", kParameterIsAutomable);
    value[ID::tempoSync] = std::make_unique<IntValue>(
      0.0, Scales::boolScale, "tempoSync", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::negativeFeedback] = std::make_unique<IntValue>(
      0.0, Scales::boolScale, "negativeFeedback",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::lfoTimeAmount] = std::make_unique<LogValue>(
      0.0, Scales::lfoTimeAmount, "lfoTimeAmount", kParameterIsAutomable);
    value[ID::lfoToneAmount] = std::make_unique<LogValue>(
      0.0, Scales::lfoToneAmount, "lfoToneAmount", kParameterIsAutomable);
    value[ID::lfoFrequency] = std::make_unique<LogValue>(
      0.5, Scales::lfoFrequency, "lfoFrequency", kParameterIsAutomable);
    value[ID::lfoShape] = std::make_unique<LogValue>(
      0.5, Scales::lfoShape, "lfoShape", kParameterIsAutomable);
    value[ID::lfoInitialPhase] = std::make_unique<LinearValue>(
      0.0, Scales::lfoInitialPhase, "lfoInitialPhase", kParameterIsAutomable);
    value[ID::lfoHold] = std::make_unique<IntValue>(
      0.0, Scales::boolScale, "lfoHold", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::smoothness] = std::make_unique<LogValue>(
      0.3, Scales::smoothness, "smoothness", kParameterIsAutomable);
    value[ID::inSpread] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "inSpread", kParameterIsAutomable);
    value[ID::inPan] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "inPan", kParameterIsAutomable);
    value[ID::outSpread] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "outSpread", kParameterIsAutomable);
    value[ID::outPan] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "outPan", kParameterIsAutomable);
    value[ID::toneCutoff] = std::make_unique<LogValue>(
      1.0, Scales::toneCutoff, "toneCutoff", kParameterIsAutomable);
    value[ID::toneQ]
      = std::make_unique<LogValue>(0.9, Scales::toneQ, "toneQ", kParameterIsAutomable);
    value[ID::dckill]
      = std::make_unique<LogValue>(0.0, Scales::dckill, "dckill", kParameterIsAutomable);
  }

  void initParameter(uint32_t index, Parameter &parameter)
  {
    if (index >= value.size()) return;
    value[index]->setParameterRange(parameter);
  }

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
    preset3_16Invert,
    preset3_16PingPong,
    presetBend,
    presetChorus,
    presetCloseToPhaser,
    presetFlapping,
    presetGhostVibrato,
    presetHigh,
    presetLean,
    presetMaxFeedback,
    presetModeratelyCentered,
    presetNoise,
    presetPower,
    presetResponse,
    presetSlowLFO,
    presetSomewhere,
    presetTail,
    presetTurnUpTimeKnob,
    presetWaitForScratch,
    presetWandering,

    Preset_ENUM_LENGTH,
  };

  std::array<const char *, 21> programName{
    "Default",   "3/16Invert",    "3/16PingPong",   "Bend",
    "Chorus",    "CloseToPhaser", "Flapping",       "GhostVibrato",
    "High",      "Lean",          "MaxFeedback",    "ModeratelyCentered",
    "Noise",     "Power",         "Response",       "SlowLFO",
    "Somewhere", "Tail",          "TurnUpTimeKnob", "WaitForScratch",
    "Wandering",
  };

  void initProgramName(uint32_t index, String &programName)
  {
    programName = this->programName[index];
  }

  void loadProgram(uint32_t index);
};
