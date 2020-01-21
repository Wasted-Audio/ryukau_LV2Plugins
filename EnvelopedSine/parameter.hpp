// (c) 2019-2020 Takamitsu Endo
//
// This file is part of EnvelopedSine.
//
// EnvelopedSine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// EnvelopedSine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with EnvelopedSine.  If not, see <https://www.gnu.org/licenses/>.

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

  // 64 for each.
  attack0,
  decay0 = 65,
  overtone0 = 129,
  saturation0 = 193,

  // Do not add parameter here.

  gain = 257,
  gainBoost,

  aliasing,
  masterOctave,
  pitchMultiply,
  pitchModulo,

  seed,
  randomRetrigger,
  randomGain,
  randomFrequency,
  randomAttack,
  randomDecay,
  randomSaturation,
  randomPhase,

  overtoneExpand,
  overtoneShift,

  attackMultiplier,
  decayMultiplier,
  declick,
  gainPower,
  saturationMix,

  phaserMix,
  phaserFrequency,
  phaserFeedback,
  phaserRange,
  phaserMin,
  phaserPhase,
  phaserOffset,
  phaserStage,

  unison,
  nVoice,
  smoothness,

  pitchBend,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::LogScale<double> envelopeA;
  static SomeDSP::LogScale<double> envelopeD;
  static SomeDSP::DecibelScale<double> gainDecibel;
  static SomeDSP::LogScale<double> saturation;

  static SomeDSP::LogScale<double> gain;
  static SomeDSP::LinearScale<double> gainBoost;

  static SomeDSP::LinearScale<double> masterOctave;
  static SomeDSP::LinearScale<double> equalTemperament;
  static SomeDSP::LinearScale<double> pitchMultiply;
  static SomeDSP::LinearScale<double> pitchModulo;

  static SomeDSP::IntScale<double> seed;
  static SomeDSP::LinearScale<double> randomGain;
  static SomeDSP::LogScale<double> randomFrequency;
  static SomeDSP::LinearScale<double> randomAttack;
  static SomeDSP::LinearScale<double> randomDecay;
  static SomeDSP::LinearScale<double> randomSaturation;
  static SomeDSP::LinearScale<double> randomPhase;

  static SomeDSP::LogScale<double> overtoneExpand;
  static SomeDSP::LinearScale<double> overtoneShift;

  static SomeDSP::LogScale<double> envelopeMultiplier;
  static SomeDSP::LinearScale<double> gainPower;

  static SomeDSP::LogScale<double> phaserFrequency;
  static SomeDSP::LinearScale<double> phaserFeedback;
  static SomeDSP::LogScale<double> phaserRange;
  static SomeDSP::LinearScale<double> phaserPhase;
  static SomeDSP::IntScale<double> phaserStage;

  static SomeDSP::IntScale<double> nVoice;
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
    using DecibelValue = FloatValue<SomeDSP::DecibelScale<double>>;

    value[ID::bypass] = std::make_unique<IntValue>(
      0, Scales::boolScale, "bypass", kParameterIsAutomable | kParameterIsBoolean);

    std::string attackLabel("attack");
    std::string curveLabel("curve");
    std::string overtoneLabel("overtone");
    std::string saturationLabel("saturation");
    for (size_t i = 0; i < nOvertone; ++i) {
      auto indexStr = std::to_string(i);
      value[ID::attack0 + i] = std::make_unique<LogValue>(
        0.0, Scales::envelopeA, (attackLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::decay0 + i] = std::make_unique<LogValue>(
        0.5, Scales::envelopeD, (curveLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::overtone0 + i] = std::make_unique<DecibelValue>(
        Scales::gainDecibel.invmap(1.0 / (i + 1)), Scales::gainDecibel,
        (overtoneLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::saturation0 + i] = std::make_unique<LogValue>(
        0.0, Scales::saturation, (saturationLabel + indexStr).c_str(),
        kParameterIsAutomable);
    }

    value[ID::gain]
      = std::make_unique<LogValue>(0.5, Scales::gain, "gain", kParameterIsAutomable);
    value[ID::gainBoost] = std::make_unique<LinearValue>(
      0.0, Scales::gainBoost, "gainBoost", kParameterIsAutomable);

    value[ID::aliasing] = std::make_unique<IntValue>(
      0, Scales::boolScale, "aliasing", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::masterOctave] = std::make_unique<LinearValue>(
      0.5, Scales::masterOctave, "masterOctave",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::pitchMultiply] = std::make_unique<LinearValue>(
      Scales::pitchMultiply.invmap(1.0), Scales::pitchMultiply, "pitchMultiply",
      kParameterIsAutomable);
    value[ID::pitchModulo] = std::make_unique<LinearValue>(
      0.0, Scales::pitchModulo, "pitchModulo", kParameterIsAutomable);

    value[ID::seed] = std::make_unique<IntValue>(
      0, Scales::seed, "seed", kParameterIsAutomable | kParameterIsInteger);
    value[ID::randomRetrigger] = std::make_unique<IntValue>(
      0, Scales::boolScale, "randomRetrigger",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::randomGain] = std::make_unique<LinearValue>(
      0.0, Scales::randomGain, "randomGain", kParameterIsAutomable);
    value[ID::randomFrequency] = std::make_unique<LogValue>(
      0.0, Scales::randomFrequency, "randomFrequency", kParameterIsAutomable);
    value[ID::randomAttack] = std::make_unique<LinearValue>(
      0.0, Scales::randomAttack, "randomAttack", kParameterIsAutomable);
    value[ID::randomDecay] = std::make_unique<LinearValue>(
      0.0, Scales::randomDecay, "randomDecay", kParameterIsAutomable);
    value[ID::randomSaturation] = std::make_unique<LinearValue>(
      0.0, Scales::randomSaturation, "randomSaturation", kParameterIsAutomable);
    value[ID::randomPhase] = std::make_unique<LinearValue>(
      0.0, Scales::randomPhase, "randomPhase", kParameterIsAutomable);

    value[ID::overtoneExpand] = std::make_unique<LogValue>(
      Scales::overtoneExpand.invmap(1.0), Scales::overtoneExpand, "overtoneExpand",
      kParameterIsAutomable);
    value[ID::overtoneShift] = std::make_unique<LinearValue>(
      0, Scales::overtoneShift, "overtoneShift", kParameterIsAutomable);

    value[ID::attackMultiplier] = std::make_unique<LogValue>(
      Scales::envelopeMultiplier.invmap(1.0), Scales::envelopeMultiplier,
      "attackMultiplier", kParameterIsAutomable);
    value[ID::decayMultiplier] = std::make_unique<LogValue>(
      Scales::envelopeMultiplier.invmap(1.0), Scales::envelopeMultiplier,
      "decayMultiplier", kParameterIsAutomable);
    value[ID::declick] = std::make_unique<IntValue>(
      1, Scales::boolScale, "declick", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::gainPower] = std::make_unique<LinearValue>(
      Scales::gainPower.invmap(1.0), Scales::gainPower, "gainPower",
      kParameterIsAutomable);
    value[ID::saturationMix] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "saturationMix", kParameterIsAutomable);

    value[ID::phaserMix] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "phaserMix", kParameterIsAutomable);
    value[ID::phaserFrequency] = std::make_unique<LogValue>(
      0.5, Scales::phaserFrequency, "phaserFrequency", kParameterIsAutomable);
    value[ID::phaserFeedback] = std::make_unique<LinearValue>(
      0.5, Scales::phaserFeedback, "phaserFeedback", kParameterIsAutomable);
    value[ID::phaserRange] = std::make_unique<LogValue>(
      1.0, Scales::phaserRange, "phaserRange", kParameterIsAutomable);
    value[ID::phaserMin] = std::make_unique<LogValue>(
      0.0, Scales::phaserRange, "phaserMin", kParameterIsAutomable);
    value[ID::phaserPhase] = std::make_unique<LinearValue>(
      0.0, Scales::phaserPhase, "phaserPhase", kParameterIsAutomable);
    value[ID::phaserOffset] = std::make_unique<LinearValue>(
      0.5, Scales::phaserPhase, "phaserOffset", kParameterIsAutomable);
    value[ID::phaserStage] = std::make_unique<IntValue>(
      15, Scales::phaserStage, "phaserStage",
      kParameterIsAutomable | kParameterIsInteger);

    value[ID::unison] = std::make_unique<IntValue>(
      0, Scales::boolScale, "unison", kParameterIsAutomable | kParameterIsBoolean);
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
    preset3and7,
    preset5n,
    presetAquatic,
    presetAround9,
    presetAtCs5,
    presetBlur,
    presetGlassCup,
    presetHighFrequency,
    presetLongDecay,
    presetMaxAttack,
    presetMetallicContact,
    presetModuloKey,
    presetNotHappening,
    presetNotSoMuch,
    presetOutOfTune,
    presetPercussive,
    presetPermuteSaturation,
    presetPhasedBass,
    presetPhaserFeedback,
    presetPluck,
    presetPong,
    presetRandom,
    presetRandomDecay,
    presetRubberBand,
    presetSaturation1,
    presetSparse,
    presetStrangeBass,
    presetSubCluster,
    presetWhenItIsReady,
    presetWhereIsThisGoing,

    Preset_ENUM_LENGTH,
  };

  std::array<const char *, 31> programName{
    "Default",
    "3and7",
    "5n",
    "Aquatic",
    "Around9",
    "AtCs5",
    "Blur",
    "GlassCup",
    "HighFrequency",
    "LongDecay",
    "MaxAttack",
    "MetallicContact",
    "ModuloKey",
    "NotHappening",
    "NotSoMuch",
    "OutOfTune",
    "Percussive",
    "PermuteSaturation",
    "PhasedBass",
    "PhaserFeedback",
    "Pluck",
    "Pong",
    "Random",
    "RandomDecay",
    "RubberBand",
    "Saturation1",
    "Sparse",
    "StrangeBass",
    "SubCluster",
    "WhenItIsReady",
    "WhereIsThisGoing",
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
