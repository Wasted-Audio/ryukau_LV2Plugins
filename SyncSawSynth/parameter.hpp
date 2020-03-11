// (c) 2019-2020 Takamitsu Endo
//
// This file is part of SyncSawSynth.
//
// SyncSawSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SyncSawSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SyncSawSynth.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <vector>

#include "../common/dsp/scale.hpp"
#include "../common/value.hpp"

#ifdef TEST_BUILD
static const uint32_t kParameterIsAutomable = 0x01;
static const uint32_t kParameterIsBoolean = 0x02;
static const uint32_t kParameterIsInteger = 0x04;
static const uint32_t kParameterIsLogarithmic = 0x08;
#endif

namespace ParameterID {
enum ID {
  bypass,

  unison,

  osc1Gain,
  osc1Semi,
  osc1Cent,
  osc1Sync,
  osc1SyncType,
  osc1PTROrder,
  osc1Phase,
  osc1PhaseLock,

  osc2Gain,
  osc2Semi,
  osc2Cent,
  osc2Sync,
  osc2SyncType,
  osc2PTROrder,
  osc2Phase,
  osc2PhaseLock,
  osc2Invert,

  fmOsc1ToSync1,
  fmOsc1ToFreq2,
  fmOsc2ToSync1,

  gain,
  gainA,
  gainD,
  gainS,
  gainR,
  gainEnvelopeCurve,

  filterCutoff,
  filterResonance,
  filterFeedback,
  filterSaturation,
  filterDirty,
  filterType,
  filterShaper,
  filterA,
  filterD,
  filterS,
  filterR,
  filterCutoffAmount,
  filterResonanceAmount,
  filterKeyToCutoff,
  filterKeyToFeedback,

  modEnvelopeA,
  modEnvelopeCurve,
  modEnvelopeToFreq1,
  modEnvelopeToSync1,
  modEnvelopeToFreq2,
  modEnvelopeToSync2,
  modLFOFrequency,
  modLFONoiseMix,
  modLFOToFreq1,
  modLFOToSync1,
  modLFOToFreq2,
  modLFOToSync2,

  pitchBend,
  nVoice,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::LogScale<double> oscGain;
  static SomeDSP::LinearScale<double> semi;
  static SomeDSP::LinearScale<double> cent;
  static SomeDSP::LinearScale<double> sync;
  static SomeDSP::IntScale<double> syncType;
  static SomeDSP::IntScale<double> ptrOrder;

  static SomeDSP::LogScale<double> fmToSync;
  static SomeDSP::LogScale<double> fmToFreq;

  static SomeDSP::LogScale<double> gain;

  static SomeDSP::LogScale<double> envelopeA;
  static SomeDSP::LogScale<double> envelopeD;
  static SomeDSP::LogScale<double> envelopeS;
  static SomeDSP::LogScale<double> envelopeR;

  static SomeDSP::LogScale<double> filterCutoff;
  static SomeDSP::LogScale<double> filterResonance;
  static SomeDSP::LogScale<double> filterFeedback;
  static SomeDSP::LogScale<double> filterSaturation;
  static SomeDSP::IntScale<double> filterType;
  static SomeDSP::IntScale<double> filterShaper;
  static SomeDSP::LinearScale<double> filterCutoffAmount;
  static SomeDSP::LinearScale<double> filterKeyMod;

  static SomeDSP::LogScale<double> modEnvelopeA;
  static SomeDSP::LogScale<double> modEnvelopeCurve;
  static SomeDSP::LogScale<double> modLFOFrequency;
  static SomeDSP::LogScale<double> modToFreq;
  static SomeDSP::LogScale<double> modToSync;

  static SomeDSP::IntScale<double> nVoice;
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
      0.0, Scales::boolScale, "Bypass", kParameterIsAutomable | kParameterIsBoolean);

    value[ID::unison] = std::make_unique<IntValue>(
      0.0, Scales::boolScale, "Unison", kParameterIsAutomable | kParameterIsBoolean);

    value[ID::osc1Gain] = std::make_unique<LogValue>(
      1.0, Scales::oscGain, "Osc1Gain", kParameterIsAutomable);
    value[ID::osc1Semi] = std::make_unique<LinearValue>(
      0.5, Scales::semi, "Osc1Semi", kParameterIsAutomable);
    value[ID::osc1Cent] = std::make_unique<LinearValue>(
      0.5, Scales::cent, "Osc1Cent", kParameterIsAutomable);
    value[ID::osc1Sync] = std::make_unique<LinearValue>(
      1.0 / 16.0, Scales::sync, "Osc1Sync", kParameterIsAutomable);
    value[ID::osc1SyncType] = std::make_unique<IntValue>(
      0, Scales::syncType, "Osc1SyncType", kParameterIsAutomable | kParameterIsInteger);
    value[ID::osc1PTROrder] = std::make_unique<IntValue>(
      16.0 / Scales::ptrOrder.getMax(), Scales::ptrOrder, "Osc1PTROrder",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::osc1Phase] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "Osc1Phase", kParameterIsAutomable);
    value[ID::osc1PhaseLock] = std::make_unique<IntValue>(
      false, Scales::boolScale, "Osc1PhaseLock",
      kParameterIsAutomable | kParameterIsBoolean);

    value[ID::osc2Gain] = std::make_unique<LogValue>(
      1.0, Scales::oscGain, "Osc2Gain", kParameterIsAutomable);
    value[ID::osc2Semi] = std::make_unique<LinearValue>(
      0.5, Scales::semi, "Osc2Semi", kParameterIsAutomable);
    value[ID::osc2Cent] = std::make_unique<LinearValue>(
      0.5, Scales::cent, "Osc2Cent", kParameterIsAutomable);
    value[ID::osc2Sync] = std::make_unique<LinearValue>(
      1.0 / 16.0, Scales::sync, "Osc2Sync", kParameterIsAutomable);
    value[ID::osc2SyncType] = std::make_unique<IntValue>(
      0, Scales::syncType, "Osc2SyncType", kParameterIsAutomable | kParameterIsInteger);
    value[ID::osc2PTROrder] = std::make_unique<IntValue>(
      16.0 / Scales::ptrOrder.getMax(), Scales::ptrOrder, "Osc2PTROrder",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::osc2Phase] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "Osc2Phase", kParameterIsAutomable);
    value[ID::osc2PhaseLock] = std::make_unique<IntValue>(
      false, Scales::boolScale, "Osc2PhaseLock",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::osc2Invert] = std::make_unique<IntValue>(
      false, Scales::boolScale, "Osc2Invert",
      kParameterIsAutomable | kParameterIsBoolean);

    value[ID::fmOsc1ToSync1] = std::make_unique<LogValue>(
      0.0, Scales::fmToSync, "FM_Osc1_to_Sync1", kParameterIsAutomable);
    value[ID::fmOsc1ToFreq2] = std::make_unique<LogValue>(
      0.0, Scales::fmToFreq, "FM_Osc1_to_Freq2", kParameterIsAutomable);
    value[ID::fmOsc2ToSync1] = std::make_unique<LogValue>(
      0.0, Scales::fmToSync, "FM_Osc2_to_Sync1", kParameterIsAutomable);

    value[ID::gain]
      = std::make_unique<LogValue>(0.5, Scales::gain, "Gain", kParameterIsAutomable);
    value[ID::gainA] = std::make_unique<LogValue>(
      0.05, Scales::envelopeA, "GainAttack", kParameterIsAutomable);
    value[ID::gainD] = std::make_unique<LogValue>(
      0.5, Scales::envelopeD, "GainDecay", kParameterIsAutomable);
    value[ID::gainS] = std::make_unique<LogValue>(
      0.5, Scales::envelopeS, "GainSustain", kParameterIsAutomable);
    value[ID::gainR] = std::make_unique<LogValue>(
      0.1, Scales::envelopeR, "GainRelease", kParameterIsAutomable);
    value[ID::gainEnvelopeCurve] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "GainEnvCurve", kParameterIsAutomable);

    value[ID::filterCutoff] = std::make_unique<LogValue>(
      1.0, Scales::filterCutoff, "Cutoff", kParameterIsAutomable);
    value[ID::filterResonance] = std::make_unique<LogValue>(
      0.5, Scales::filterResonance, "Resonance", kParameterIsAutomable);
    value[ID::filterFeedback] = std::make_unique<LogValue>(
      0.0, Scales::filterFeedback, "Feedback", kParameterIsAutomable);
    value[ID::filterSaturation] = std::make_unique<LogValue>(
      0.3, Scales::filterSaturation, "Saturation", kParameterIsAutomable);
    value[ID::filterDirty] = std::make_unique<IntValue>(
      false, Scales::boolScale, "DirtyBuffer",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::filterType] = std::make_unique<IntValue>(
      0, Scales::filterType, "FilterType", kParameterIsAutomable | kParameterIsInteger);
    value[ID::filterShaper] = std::make_unique<IntValue>(
      1.0 / Scales::filterShaper.getMax(), Scales::filterShaper, "FilterShaper",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::filterA] = std::make_unique<LogValue>(
      0.0, Scales::envelopeA, "FilterEnvAttack", kParameterIsAutomable);
    value[ID::filterD] = std::make_unique<LogValue>(
      0.5, Scales::envelopeD, "FilterEnvDecay", kParameterIsAutomable);
    value[ID::filterS] = std::make_unique<LogValue>(
      0.0, Scales::envelopeS, "FilterEnvSustain", kParameterIsAutomable);
    value[ID::filterR] = std::make_unique<LogValue>(
      0.5, Scales::envelopeR, "FilterEnvRelease", kParameterIsAutomable);
    value[ID::filterCutoffAmount] = std::make_unique<LinearValue>(
      0.5, Scales::filterCutoffAmount, "CutoffAmount", kParameterIsAutomable);
    value[ID::filterResonanceAmount] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "ResonanceAmount", kParameterIsAutomable);
    value[ID::filterKeyToCutoff] = std::make_unique<LinearValue>(
      0.5, Scales::filterKeyMod, "Key_to_Cutoff", kParameterIsAutomable);
    value[ID::filterKeyToFeedback] = std::make_unique<LinearValue>(
      0.5, Scales::filterKeyMod, "Key_to_Feedback", kParameterIsAutomable);

    value[ID::modEnvelopeA] = std::make_unique<LogValue>(
      0.0, Scales::modEnvelopeA, "ModEnvAttack", kParameterIsAutomable);
    value[ID::modEnvelopeCurve] = std::make_unique<LogValue>(
      0.5, Scales::modEnvelopeCurve, "ModEnvCurve", kParameterIsAutomable);
    value[ID::modEnvelopeToFreq1] = std::make_unique<LogValue>(
      0.0, Scales::modToFreq, "ModEnv_to_Freq1", kParameterIsAutomable);
    value[ID::modEnvelopeToSync1] = std::make_unique<LogValue>(
      0.0, Scales::modToSync, "ModEnv_to_Sync1", kParameterIsAutomable);
    value[ID::modEnvelopeToFreq2] = std::make_unique<LogValue>(
      0.0, Scales::modToFreq, "ModEnv_to_Freq2", kParameterIsAutomable);
    value[ID::modEnvelopeToSync2] = std::make_unique<LogValue>(
      0.0, Scales::modToSync, "ModEnv_to_Sync2", kParameterIsAutomable);
    value[ID::modLFOFrequency] = std::make_unique<LogValue>(
      0.5, Scales::modLFOFrequency, "LFO_Frequency", kParameterIsAutomable);
    value[ID::modLFONoiseMix] = std::make_unique<LinearValue>(
      0.01, Scales::defaultScale, "LFO_Noise_Mix", kParameterIsAutomable);
    value[ID::modLFOToFreq1] = std::make_unique<LogValue>(
      0.0, Scales::modToFreq, "LFO_to_Freq1", kParameterIsAutomable);
    value[ID::modLFOToSync1] = std::make_unique<LogValue>(
      0.0, Scales::modToSync, "LFO_to_Sync1", kParameterIsAutomable);
    value[ID::modLFOToFreq2] = std::make_unique<LogValue>(
      0.0, Scales::modToFreq, "LFO_to_Freq2", kParameterIsAutomable);
    value[ID::modLFOToSync2] = std::make_unique<LogValue>(
      0.0, Scales::modToSync, "LFO_to_Sync2", kParameterIsAutomable);

    value[ID::pitchBend] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "PitchBend", kParameterIsAutomable);
    value[ID::nVoice] = std::make_unique<IntValue>(
      5.0 / Scales::nVoice.getMax(), Scales::nVoice, "nVoice",
      kParameterIsAutomable | kParameterIsInteger);
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
    preset_2Octave,
    presetAlmost,
    presetBasicFilter,
    presetBasicSync,
    presetBreathing,
    presetBurningUpCPU,
    presetEdgyResonance,
    presetFaint,
    presetHmmm,
    presetIndistinguishable,
    presetMessBehindFilter,
    presetMod_HP,
    presetMotivation,
    presetNoisyMod,
    presetOutOfTuning,
    presetPitchDecay,
    presetPitchLFO,
    presetPulse,
    presetRunOutOfIdea,
    presetSingleOscSyncLFO,
    presetSomehowTonal,
    presetTeachMeEnglishCoolWords,
    presetThisIsntAModem,

    Preset_ENUM_LENGTH,
  };

  std::array<const char *, 24> programName{
    "Default",
    "-2Octave",
    "Almost",
    "BasicFilter",
    "BasicSync",
    "Breathing",
    "BurningUpCPU",
    "EdgyResonance",
    "Faint",
    "Hmmm",
    "Indistinguishable",
    "MessBehindFilter",
    "Mod+HP",
    "Motivation",
    "NoisyMod",
    "OutOfTuning",
    "PitchDecay",
    "PitchLFO",
    "Pulse",
    "RunOutOfIdea",
    "SingleOscSyncLFO",
    "SomehowTonal",
    "TeachMeEnglishCoolWords",
    "ThisIsntAModem",
  };

  void initProgramName(uint32_t index, String &programName)
  {
    programName = this->programName[index];
  }

  void loadProgram(uint32_t index);
};
