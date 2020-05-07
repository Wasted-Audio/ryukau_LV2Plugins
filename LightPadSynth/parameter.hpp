// (c) 2019-2020 Takamitsu Endo
//
// This file is part of LightPadSynth.
//
// LightPadSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LightPadSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LightPadSynth.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <iostream>

#include "../common/dsp/constants.hpp"
#include "../common/dsp/scale.hpp"
#include "../common/value.hpp"

#ifdef TEST_BUILD
static const uint32_t kParameterIsAutomable = 0x01;
static const uint32_t kParameterIsBoolean = 0x02;
static const uint32_t kParameterIsInteger = 0x04;
static const uint32_t kParameterIsLogarithmic = 0x08;
#endif

constexpr int32_t nOvertone = 360;
constexpr int32_t nLFOWavetable = 64;
constexpr int32_t tableSize = 262144;
constexpr int32_t lfoTableSize = 1023;
constexpr int32_t spectrumSize = tableSize / 2 + 1;

namespace ParameterID {
enum ID {
  bypass,

  overtoneGain0,
  overtoneWidth0 = 361,
  overtonePitch0 = 721,
  overtonePhase0 = 1081,

  lfoWavetable0 = 1441,

  // Do not add parameter here.

  tableBaseFrequency = 1505,
  tableBufferSize,
  padSynthSeed,
  overtoneGainPower,
  overtoneWidthMultiply,
  overtonePitchMultiply,
  overtonePitchModulo,
  spectrumExpand,
  spectrumRotate,
  profileComb,
  profileShape,
  uniformPhaseProfile,

  gain,
  gainA,
  gainD,
  gainS,
  gainR,
  gainCurve,

  filterCutoff,
  filterResonance,
  filterA,
  filterD,
  filterS,
  filterR,
  filterAmount,
  filterKeyFollow,

  delayMix,
  delayDetuneSemi,
  delayDetuneMilli,
  delayFeedback,
  delayAttack,

  oscOctave,
  oscSemi,
  oscMilli,
  equalTemperament,
  pitchA4Hz,

  lfoWavetableType,
  lfoTempoNumerator,
  lfoTempoDenominator,
  lfoFrequencyMultiplier,
  lfoDelayAmount,
  lfoLowpass,

  oscInitialPhase,
  oscPhaseReset,
  oscPhaseRandom,

  nUnison,
  unisonDetune,
  unisonPan,
  unisonPhase,
  unisonGainRandom,
  unisonDetuneRandom,
  unisonPanType,

  nVoice,
  smoothness,
  seed,

  pitchBend,

  refreshLFO,
  refreshTable,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::DecibelScale<double> overtoneGain;
  static SomeDSP::LinearScale<double> overtoneWidth;
  static SomeDSP::LogScale<double> overtonePitch;
  static SomeDSP::LinearScale<double> overtonePhase;

  static SomeDSP::LogScale<double> tableBaseFrequency;
  static SomeDSP::IntScale<double> tableBufferSize;
  static SomeDSP::LogScale<double> overtoneGainPower;
  static SomeDSP::LogScale<double> overtoneWidthMultiply;
  static SomeDSP::LogScale<double> overtonePitchMultiply;
  static SomeDSP::LinearScale<double> overtonePitchModulo;
  static SomeDSP::LogScale<double> spectrumExpand;
  static SomeDSP::IntScale<double> profileComb;
  static SomeDSP::LogScale<double> profileShape;
  static SomeDSP::IntScale<double> seed;

  static SomeDSP::LogScale<double> gain;
  static SomeDSP::LogScale<double> envelopeA;
  static SomeDSP::LogScale<double> envelopeD;
  static SomeDSP::LogScale<double> envelopeS;
  static SomeDSP::LogScale<double> envelopeR;

  static SomeDSP::LogScale<double> filterCutoff;
  static SomeDSP::LinearScale<double> filterResonance;

  static SomeDSP::IntScale<double> delayDetuneSemi;
  static SomeDSP::LinearScale<double> delayFeedback;

  static SomeDSP::IntScale<double> oscOctave;
  static SomeDSP::IntScale<double> oscSemi;
  static SomeDSP::IntScale<double> oscMilli;
  static SomeDSP::IntScale<double> equalTemperament;
  static SomeDSP::IntScale<double> pitchA4Hz;

  static SomeDSP::LinearScale<double> lfoWavetable;
  static SomeDSP::IntScale<double> lfoWavetableType;
  static SomeDSP::IntScale<double> lfoTempoNumerator;
  static SomeDSP::IntScale<double> lfoTempoDenominator;
  static SomeDSP::LogScale<double> lfoFrequencyMultiplier;
  static SomeDSP::LogScale<double> lfoDelayAmount;

  static SomeDSP::IntScale<double> nUnison;
  static SomeDSP::LogScale<double> unisonDetune;
  static SomeDSP::IntScale<double> unisonPanType;

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

    std::string gainLabel("gain");
    std::string widthLabel("width");
    std::string pitchLabel("pitch");
    std::string phaseLabel("phase");
    for (size_t idx = 0; idx < nOvertone; ++idx) {
      auto indexStr = std::to_string(idx);
      value[ID::overtoneGain0 + idx] = std::make_unique<DecibelValue>(
        Scales::overtoneGain.invmap(1.0 / (idx + 1)), Scales::overtoneGain,
        (gainLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::overtoneWidth0 + idx] = std::make_unique<LinearValue>(
        0.5, Scales::overtoneWidth, (widthLabel + indexStr).c_str(),
        kParameterIsAutomable);
      value[ID::overtonePitch0 + idx] = std::make_unique<LogValue>(
        Scales::overtonePitch.invmap(1.0), Scales::overtonePitch,
        (pitchLabel + indexStr).c_str(), kParameterIsAutomable);
      value[ID::overtonePhase0 + idx] = std::make_unique<LinearValue>(
        1.0, Scales::overtonePhase, (phaseLabel + indexStr).c_str(),
        kParameterIsAutomable);
    }

    std::string lfoWavetableLabel("lfoWavetable");
    for (size_t idx = 0; idx < nLFOWavetable; ++idx) {
      auto indexStr = std::to_string(idx);
      value[ID::lfoWavetable0 + idx] = std::make_unique<LinearValue>(
        Scales::lfoWavetable.invmap(sin(SomeDSP::twopi * idx / double(nLFOWavetable))),
        Scales::lfoWavetable, (lfoWavetableLabel + indexStr).c_str(),
        kParameterIsAutomable);
    }

    value[ID::tableBaseFrequency] = std::make_unique<LogValue>(
      Scales::tableBaseFrequency.invmap(10.0), Scales::tableBaseFrequency,
      "tableBaseFrequency", kParameterIsAutomable);
    value[ID::tableBufferSize] = std::make_unique<IntValue>(
      8, Scales::tableBufferSize, "tableBufferSize",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::padSynthSeed] = std::make_unique<IntValue>(
      0, Scales::seed, "padSynthSeed", kParameterIsAutomable | kParameterIsInteger);
    value[ID::overtoneGainPower] = std::make_unique<LogValue>(
      0.5, Scales::overtoneGainPower, "overtoneGainPower", kParameterIsAutomable);
    value[ID::overtoneWidthMultiply] = std::make_unique<LogValue>(
      0.5, Scales::overtoneWidthMultiply, "overtoneWidthMultiply", kParameterIsAutomable);
    value[ID::overtonePitchMultiply] = std::make_unique<LogValue>(
      Scales::overtonePitchMultiply.invmap(1.0), Scales::overtonePitchMultiply,
      "overtonePitchMultiply", kParameterIsAutomable);
    value[ID::overtonePitchModulo] = std::make_unique<LinearValue>(
      0.0, Scales::overtonePitchModulo, "overtonePitchModulo", kParameterIsAutomable);
    value[ID::spectrumExpand] = std::make_unique<LogValue>(
      Scales::spectrumExpand.invmap(1.0), Scales::spectrumExpand, "spectrumExpand",
      kParameterIsAutomable);
    value[ID::spectrumRotate] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "spectrumRotate", kParameterIsAutomable);
    value[ID::profileComb] = std::make_unique<IntValue>(
      0, Scales::profileComb, "profileComb", kParameterIsAutomable | kParameterIsInteger);
    value[ID::profileShape] = std::make_unique<LogValue>(
      Scales::profileShape.invmap(1.0), Scales::profileShape, "profileShape",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::uniformPhaseProfile] = std::make_unique<IntValue>(
      0, Scales::boolScale, "uniformPhaseProfile",
      kParameterIsAutomable | kParameterIsBoolean);

    value[ID::gain]
      = std::make_unique<LogValue>(0.5, Scales::gain, "gain", kParameterIsAutomable);
    value[ID::gainA] = std::make_unique<LogValue>(
      0.0, Scales::envelopeA, "gainA", kParameterIsAutomable);
    value[ID::gainD] = std::make_unique<LogValue>(
      0.5, Scales::envelopeD, "gainD", kParameterIsAutomable);
    value[ID::gainS] = std::make_unique<LogValue>(
      0.5, Scales::envelopeS, "gainS", kParameterIsAutomable);
    value[ID::gainR] = std::make_unique<LogValue>(
      0.0, Scales::envelopeR, "gainR", kParameterIsAutomable);
    value[ID::gainCurve] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "gainCurve", kParameterIsAutomable);

    value[ID::filterCutoff] = std::make_unique<LogValue>(
      1.0, Scales::filterCutoff, "filterCutoff", kParameterIsAutomable);
    value[ID::filterResonance] = std::make_unique<LinearValue>(
      0.0, Scales::filterResonance, "filterResonance", kParameterIsAutomable);
    value[ID::filterA] = std::make_unique<LogValue>(
      0.0, Scales::envelopeA, "filterA", kParameterIsAutomable);
    value[ID::filterD] = std::make_unique<LogValue>(
      0.5, Scales::envelopeD, "filterD", kParameterIsAutomable);
    value[ID::filterS] = std::make_unique<LogValue>(
      0.5, Scales::envelopeS, "filterS", kParameterIsAutomable);
    value[ID::filterR] = std::make_unique<LogValue>(
      1.0, Scales::envelopeR, "filterR", kParameterIsAutomable);
    value[ID::filterAmount] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "filterAmount", kParameterIsAutomable);
    value[ID::filterKeyFollow] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "filterKeyFollow", kParameterIsAutomable);

    value[ID::delayMix] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "delayMix", kParameterIsAutomable);
    value[ID::delayDetuneSemi] = std::make_unique<IntValue>(
      120, Scales::delayDetuneSemi, "delayDetuneSemi",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::delayDetuneMilli] = std::make_unique<IntValue>(
      1000, Scales::oscMilli, "delayDetuneMilli",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::delayFeedback] = std::make_unique<LinearValue>(
      0.5, Scales::delayFeedback, "delayFeedback", kParameterIsAutomable);
    value[ID::delayAttack] = std::make_unique<LogValue>(
      0.0, Scales::envelopeA, "delayAttack", kParameterIsAutomable);

    value[ID::oscOctave] = std::make_unique<IntValue>(
      12, Scales::oscOctave, "oscOctave", kParameterIsAutomable | kParameterIsInteger);
    value[ID::oscSemi] = std::make_unique<IntValue>(
      120, Scales::oscSemi, "oscSemi", kParameterIsAutomable | kParameterIsInteger);
    value[ID::oscMilli] = std::make_unique<IntValue>(
      1000, Scales::oscMilli, "oscMilli", kParameterIsAutomable | kParameterIsInteger);
    value[ID::equalTemperament] = std::make_unique<IntValue>(
      11, Scales::equalTemperament, "equalTemperament",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::pitchA4Hz] = std::make_unique<IntValue>(
      340, Scales::pitchA4Hz, "pitchA4Hz", kParameterIsAutomable | kParameterIsInteger);

    value[ID::lfoWavetableType] = std::make_unique<IntValue>(
      2, Scales::lfoWavetableType, "lfoWavetableType",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::lfoTempoNumerator] = std::make_unique<IntValue>(
      0, Scales::lfoTempoNumerator, "lfoTempoNumerator",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::lfoTempoDenominator] = std::make_unique<IntValue>(
      0, Scales::lfoTempoDenominator, "lfoTempoDenominator",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::lfoFrequencyMultiplier] = std::make_unique<LogValue>(
      Scales::lfoFrequencyMultiplier.invmap(1.0), Scales::lfoFrequencyMultiplier,
      "lfoFrequencyMultiplier", kParameterIsAutomable);
    value[ID::lfoDelayAmount] = std::make_unique<LogValue>(
      0.5, Scales::lfoDelayAmount, "lfoDelayAmount", kParameterIsAutomable);
    value[ID::lfoLowpass] = std::make_unique<LogValue>(
      1.0, Scales::filterCutoff, "lfoLowpass", kParameterIsAutomable);

    value[ID::oscInitialPhase] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "oscInitialPhase", kParameterIsAutomable);
    value[ID::oscPhaseReset] = std::make_unique<IntValue>(
      true, Scales::boolScale, "oscPhaseReset",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::oscPhaseRandom] = std::make_unique<IntValue>(
      true, Scales::boolScale, "oscPhaseRandom",
      kParameterIsAutomable | kParameterIsBoolean);

    value[ID::nUnison] = std::make_unique<IntValue>(
      0, Scales::nUnison, "nUnison", kParameterIsAutomable | kParameterIsInteger);
    value[ID::unisonDetune] = std::make_unique<LogValue>(
      0.2, Scales::unisonDetune, "unisonDetune", kParameterIsAutomable);
    value[ID::unisonPan] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "unisonPan", kParameterIsAutomable);
    value[ID::unisonPhase] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "unisonPhase", kParameterIsAutomable);
    value[ID::unisonGainRandom] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "unisonGainRandom", kParameterIsAutomable);
    value[ID::unisonDetuneRandom] = std::make_unique<IntValue>(
      1, Scales::boolScale, "unisonDetuneRandom",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::unisonPanType] = std::make_unique<IntValue>(
      0, Scales::unisonPanType, "unisonPanType",
      kParameterIsAutomable | kParameterIsInteger);

    value[ID::nVoice] = std::make_unique<IntValue>(
      1, Scales::nVoice, "nVoice", kParameterIsAutomable | kParameterIsInteger);
    value[ID::smoothness] = std::make_unique<LogValue>(
      0.1, Scales::smoothness, "smoothness", kParameterIsAutomable);
    value[ID::seed] = std::make_unique<IntValue>(
      0, Scales::seed, "seed", kParameterIsAutomable | kParameterIsInteger);

    value[ID::pitchBend] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "pitchBend", kParameterIsAutomable);

    value[ID::refreshLFO] = std::make_unique<IntValue>(
      0, Scales::boolScale, "refreshLFO", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::refreshTable] = std::make_unique<IntValue>(
      0, Scales::boolScale, "refreshTable", kParameterIsAutomable | kParameterIsBoolean);
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
    presetAlienTalk,
    presetBells,
    presetBelong,
    presetBoilFor10Hours,
    presetBuyAndSell,
    presetCeremonial,
    presetCold,
    presetCommercialTransaction,
    presetConcerned,
    presetCourteous,
    presetEnvironmentallyFriendly,
    presetExhibition,
    presetInhale,
    presetOnceUponATime,
    presetPleasant,
    presetPreliminaryDesign,
    presetProposal,
    presetRetrospective,
    presetShading,
    presetSituations,
    presetSpeakingToYou,
    presetSword,
    presetTabVsSpace,
    presetYawning,

    Preset_ENUM_LENGTH,
  };

  std::array<const char *, 25> programName{
    "Default",
    "AlienTalk",
    "Bells",
    "Belong",
    "BoilFor10Hours",
    "BuyAndSell",
    "Ceremonial",
    "Cold",
    "CommercialTransaction",
    "Concerned",
    "Courteous",
    "EnvironmentallyFriendly",
    "Exhibition",
    "Inhale",
    "OnceUponATime",
    "Pleasant",
    "PreliminaryDesign",
    "Proposal",
    "Retrospective",
    "Shading",
    "Situations",
    "SpeakingToYou",
    "Sword",
    "TabVsSpace",
    "Yawning",
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
