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

constexpr uint16_t nDelay = 24;
constexpr uint16_t nComb = 8;

namespace ParameterID {
enum ID {
  bypass,

  frequency0 = 1,

  combTime0 = 1 + nDelay,

  gain = 1 + nDelay + nComb,
  boost,

  exciterGain,
  exciterAttack,
  exciterDecay,
  exciterNoiseMix,
  exciterLowpassCutoff,

  lowpassCutoff,
  highpassCutoff,

  lowpassA,
  lowpassD,
  lowpassS,
  lowpassR,

  distance,

  randomComb,
  randomFrequency,

  seedNoise,
  seedComb,
  seedString,
  seedUnison,

  retriggerNoise,
  retriggerComb,
  retriggerString,
  retriggerUnison,

  compressor,
  compressorTime,
  compressorThreshold,

  nVoice,
  nUnison,
  unisonDetune,
  unisonSpread,
  unisonGainRandom,
  unisonDetuneRandom,

  octave,
  semitone,
  milli,
  equalTemperament,
  pitchA4Hz,
  pitchBend,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::LogScale<double> gain;
  static SomeDSP::LogScale<double> boost;

  static SomeDSP::LogScale<double> exciterGain;
  static SomeDSP::LogScale<double> exciterAttack;
  static SomeDSP::LogScale<double> exciterDecay;
  static SomeDSP::LogScale<double> exciterLowpassCutoff;

  static SomeDSP::LinearScale<double> combTime;
  static SomeDSP::LogScale<double> frequency;
  static SomeDSP::LogScale<double> lowpassCutoff;
  static SomeDSP::LogScale<double> highpassCutoff;

  static SomeDSP::LogScale<double> envelopeA;
  static SomeDSP::LogScale<double> envelopeD;
  static SomeDSP::LogScale<double> envelopeS;
  static SomeDSP::LogScale<double> envelopeR;

  static SomeDSP::LogScale<double> distance;
  static SomeDSP::IntScale<double> seed;

  static SomeDSP::LogScale<double> randomFrequency;

  static SomeDSP::LogScale<double> compressorTime;
  static SomeDSP::LogScale<double> compressorThreshold;

  static SomeDSP::IntScale<double> nVoice;
  static SomeDSP::IntScale<double> nUnison;
  static SomeDSP::LogScale<double> unisonDetune;

  static SomeDSP::IntScale<double> octave;
  static SomeDSP::IntScale<double> semitone;
  static SomeDSP::IntScale<double> milli;
  static SomeDSP::IntScale<double> equalTemperament;
  static SomeDSP::IntScale<double> pitchA4Hz;
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
        Scales::frequency.invmap(100.0 + 20.0 * idx), Scales::frequency,
        (frequencyLabel + indexStr).c_str(), kParameterIsAutomable);
    }

    std::string combTimeLabel("combTime");
    for (size_t idx = 0; idx < nComb; ++idx) {
      auto indexStr = std::to_string(idx);
      value[ID::combTime0 + idx] = std::make_unique<LinearValue>(
        0.5, Scales::combTime, (combTimeLabel + indexStr).c_str(), kParameterIsAutomable);
    }

    value[ID::gain] = std::make_unique<LogValue>(
      0.5, Scales::gain, "gain", kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::boost] = std::make_unique<LogValue>(
      Scales::boost.invmap(1.0), Scales::boost, "boost",
      kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::exciterGain] = std::make_unique<LogValue>(
      0.5, Scales::exciterGain, "exciterGain",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::exciterAttack] = std::make_unique<LogValue>(
      0.0, Scales::exciterAttack, "exciterAttack",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::exciterDecay] = std::make_unique<LogValue>(
      0.0, Scales::exciterDecay, "exciterDecay",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::exciterNoiseMix] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "exciterNoiseMix", kParameterIsAutomable);
    value[ID::exciterLowpassCutoff] = std::make_unique<LogValue>(
      0.5, Scales::exciterLowpassCutoff, "exciterLowpassCutoff",
      kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::lowpassCutoff] = std::make_unique<LogValue>(
      Scales::lowpassCutoff.invmap(12000.0), Scales::lowpassCutoff, "lowpassCutoff",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::highpassCutoff] = std::make_unique<LogValue>(
      0.5, Scales::highpassCutoff, "highpassCutoff",
      kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::lowpassA] = std::make_unique<LogValue>(
      0.0, Scales::envelopeA, "lowpassA",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::lowpassD] = std::make_unique<LogValue>(
      0.5, Scales::envelopeD, "lowpassD",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::lowpassS] = std::make_unique<LogValue>(
      0.5, Scales::envelopeS, "lowpassS",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::lowpassR] = std::make_unique<LogValue>(
      0.5, Scales::envelopeR, "lowpassR",
      kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::distance] = std::make_unique<LogValue>(
      0.5, Scales::distance, "distance", kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::randomComb] = std::make_unique<LinearValue>(
      0.01, Scales::defaultScale, "randomComb", kParameterIsAutomable);
    value[ID::randomFrequency] = std::make_unique<LogValue>(
      0.0, Scales::randomFrequency, "randomFrequency", kParameterIsAutomable);

    value[ID::seedNoise] = std::make_unique<IntValue>(
      11467559, Scales::seed, "seedNoise", kParameterIsAutomable | kParameterIsInteger);
    value[ID::seedComb] = std::make_unique<IntValue>(
      14700349, Scales::seed, "seedComb", kParameterIsAutomable | kParameterIsInteger);
    value[ID::seedString] = std::make_unique<IntValue>(
      3937273, Scales::seed, "seedString", kParameterIsAutomable | kParameterIsInteger);
    value[ID::seedUnison] = std::make_unique<IntValue>(
      2060287, Scales::seed, "seedUnison", kParameterIsAutomable | kParameterIsInteger);

    value[ID::retriggerNoise] = std::make_unique<IntValue>(
      false, Scales::boolScale, "retriggerNoise",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::retriggerComb] = std::make_unique<IntValue>(
      false, Scales::boolScale, "retriggerComb",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::retriggerString] = std::make_unique<IntValue>(
      true, Scales::boolScale, "retriggerString",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::retriggerUnison] = std::make_unique<IntValue>(
      false, Scales::boolScale, "retriggerUnison",
      kParameterIsAutomable | kParameterIsBoolean);

    value[ID::compressor] = std::make_unique<IntValue>(
      1, Scales::boolScale, "compressor", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::compressorTime] = std::make_unique<LogValue>(
      Scales::compressorTime.invmap(0.1), Scales::compressorTime, "compressorTime",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::compressorThreshold] = std::make_unique<LogValue>(
      Scales::compressorThreshold.invmap(0.5), Scales::compressorThreshold,
      "compressorThreshold", kParameterIsAutomable | kParameterIsLogarithmic);

    value[ID::nVoice] = std::make_unique<IntValue>(
      7, Scales::nVoice, "nVoice", kParameterIsAutomable | kParameterIsInteger);
    value[ID::nUnison] = std::make_unique<IntValue>(
      1, Scales::nUnison, "nUnison", kParameterIsAutomable | kParameterIsInteger);
    value[ID::unisonDetune] = std::make_unique<LogValue>(
      0.25, Scales::unisonDetune, "unisonDetune", kParameterIsAutomable);
    value[ID::unisonSpread] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "unisonSpread", kParameterIsAutomable);
    value[ID::unisonGainRandom] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "unisonGainRandom", kParameterIsAutomable);
    value[ID::unisonDetuneRandom] = std::make_unique<IntValue>(
      1, Scales::boolScale, "unisonDetuneRandom",
      kParameterIsAutomable | kParameterIsBoolean);

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
