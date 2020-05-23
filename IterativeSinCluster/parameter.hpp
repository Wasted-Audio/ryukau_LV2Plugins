// (c) 2019-2020 Takamitsu Endo
//
// This file is part of IterativeSinCluster.
//
// IterativeSinCluster is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// IterativeSinCluster is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with IterativeSinCluster.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../common/parameterinterface.hpp"
#include "../common/value.hpp"

#include <memory>
#include <vector>

#ifdef TEST_BUILD
static const uint32_t kParameterIsAutomable = 0x01;
static const uint32_t kParameterIsBoolean = 0x02;
static const uint32_t kParameterIsInteger = 0x04;
static const uint32_t kParameterIsLogarithmic = 0x08;
#endif

namespace ParameterID {
enum ID {
  bypass,

  aliasing,
  negativeSemi,
  masterOctave,
  equalTemperament,
  pitchMultiply,
  pitchModulo,

  seed,
  randomRetrigger,
  randomGainAmount,
  randomFrequencyAmount,

  chorusMix,
  chorusFrequency,
  chorusDepth,
  chorusDelayTimeRange0,
  chorusDelayTimeRange1,
  chorusDelayTimeRange2,
  chorusMinDelayTime0,
  chorusMinDelayTime1,
  chorusMinDelayTime2,
  chorusPhase,
  chorusOffset,
  chorusFeedback,
  chorusKeyFollow,

  gain,
  gainBoost,
  gainA,
  gainD,
  gainS,
  gainR,
  gainEnvelopeCurve,

  lowShelfPitch,
  lowShelfGain,
  highShelfPitch,
  highShelfGain,

  gain0,
  gain1,
  gain2,
  gain3,
  gain4,
  gain5,
  gain6,
  gain7,

  semi0,
  semi1,
  semi2,
  semi3,
  semi4,
  semi5,
  semi6,
  semi7,

  milli0,
  milli1,
  milli2,
  milli3,
  milli4,
  milli5,
  milli6,
  milli7,

  overtone0,
  overtone1,
  overtone2,
  overtone3,
  overtone4,
  overtone5,
  overtone6,
  overtone7,
  overtone8,
  overtone9,
  overtone10,
  overtone11,
  overtone12,
  overtone13,
  overtone14,
  overtone15,

  chordGain0,
  chordGain1,
  chordGain2,
  chordGain3,

  chordSemi0,
  chordSemi1,
  chordSemi2,
  chordSemi3,

  chordMilli0,
  chordMilli1,
  chordMilli2,
  chordMilli3,

  chordPan0,
  chordPan1,
  chordPan2,
  chordPan3,

  nVoice,
  smoothness,

  pitchBend,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::LinearScale<double> masterOctave;
  static SomeDSP::LinearScale<double> equalTemperament;
  static SomeDSP::LinearScale<double> pitchMultiply;
  static SomeDSP::LinearScale<double> pitchModulo;

  static SomeDSP::IntScale<double> seed;
  static SomeDSP::LogScale<double> randomAmount;

  static SomeDSP::SPolyScale<double> chorusFrequency;
  static SomeDSP::LogScale<double> chorusDelayTimeRange;
  static SomeDSP::LogScale<double> chorusMinDelayTime;
  static SomeDSP::LinearScale<double> chorusPhase;
  static SomeDSP::LinearScale<double> chorusOffset;

  static SomeDSP::IntScale<double> oscSemi;
  static SomeDSP::IntScale<double> oscMilli;
  static SomeDSP::IntScale<double> oscOctave;

  static SomeDSP::LogScale<double> gain;
  static SomeDSP::LinearScale<double> gainBoost;
  static SomeDSP::DecibelScale<double> gainDecibel;

  static SomeDSP::LinearScale<double> shelvingPitch;
  static SomeDSP::DecibelScale<double> shelvingGain;

  static SomeDSP::LogScale<double> envelopeA;
  static SomeDSP::LogScale<double> envelopeD;
  static SomeDSP::LogScale<double> envelopeS;
  static SomeDSP::LogScale<double> envelopeR;

  static SomeDSP::IntScale<double> nVoice;
  static SomeDSP::LogScale<double> smoothness;
};

struct GlobalParameter : public ParameterInterface {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    using LogValue = FloatValue<SomeDSP::LogScale<double>>;
    using SPolyValue = FloatValue<SomeDSP::SPolyScale<double>>;
    using DecibelValue = FloatValue<SomeDSP::DecibelScale<double>>;

    value[ID::bypass] = std::make_unique<IntValue>(
      0, Scales::boolScale, "bypass", kParameterIsAutomable | kParameterIsBoolean);

    value[ID::aliasing] = std::make_unique<IntValue>(
      0, Scales::boolScale, "aliasing", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::negativeSemi] = std::make_unique<IntValue>(
      0, Scales::boolScale, "negativeSemi", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::masterOctave] = std::make_unique<LinearValue>(
      0.5, Scales::masterOctave, "masterOctave",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::equalTemperament] = std::make_unique<LinearValue>(
      Scales::equalTemperament.invmap(12), Scales::equalTemperament, "equalTemperament",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::pitchMultiply] = std::make_unique<LinearValue>(
      0.25, Scales::pitchMultiply, "pitchMultiply", kParameterIsAutomable);
    value[ID::pitchModulo] = std::make_unique<LinearValue>(
      0.0, Scales::pitchModulo, "pitchModulo", kParameterIsAutomable);

    value[ID::seed] = std::make_unique<IntValue>(
      0, Scales::seed, "seed", kParameterIsAutomable | kParameterIsInteger);
    value[ID::randomRetrigger] = std::make_unique<IntValue>(
      0, Scales::boolScale, "randomRetrigger",
      kParameterIsAutomable | kParameterIsBoolean);
    value[ID::randomGainAmount] = std::make_unique<LogValue>(
      0.0, Scales::randomAmount, "randomGainAmount", kParameterIsAutomable);
    value[ID::randomFrequencyAmount] = std::make_unique<LogValue>(
      0.0, Scales::randomAmount, "randomFrequencyAmount", kParameterIsAutomable);

    value[ID::chorusMix] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "chorusMix", kParameterIsAutomable);
    value[ID::chorusFrequency] = std::make_unique<SPolyValue>(
      0.5, Scales::chorusFrequency, "chorusFrequency", kParameterIsAutomable);
    value[ID::chorusDepth] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "chorusDepth", kParameterIsAutomable);
    value[ID::chorusDelayTimeRange0] = std::make_unique<LogValue>(
      0.0, Scales::chorusDelayTimeRange, "chorusDelayTimeRange0", kParameterIsAutomable);
    value[ID::chorusDelayTimeRange1] = std::make_unique<LogValue>(
      0.0, Scales::chorusDelayTimeRange, "chorusDelayTimeRange1", kParameterIsAutomable);
    value[ID::chorusDelayTimeRange2] = std::make_unique<LogValue>(
      0.0, Scales::chorusDelayTimeRange, "chorusDelayTimeRange2", kParameterIsAutomable);
    value[ID::chorusMinDelayTime0] = std::make_unique<LogValue>(
      0.5, Scales::chorusMinDelayTime, "chorusMinDelayTime0", kParameterIsAutomable);
    value[ID::chorusMinDelayTime1] = std::make_unique<LogValue>(
      0.5, Scales::chorusMinDelayTime, "chorusMinDelayTime1", kParameterIsAutomable);
    value[ID::chorusMinDelayTime2] = std::make_unique<LogValue>(
      0.5, Scales::chorusMinDelayTime, "chorusMinDelayTime2", kParameterIsAutomable);
    value[ID::chorusPhase] = std::make_unique<LinearValue>(
      0.0, Scales::chorusPhase, "chorusPhase", kParameterIsAutomable);
    value[ID::chorusOffset] = std::make_unique<LinearValue>(
      0.0, Scales::chorusOffset, "chorusOffset", kParameterIsAutomable);
    value[ID::chorusFeedback] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "chorusFeedback", kParameterIsAutomable);
    value[ID::chorusKeyFollow] = std::make_unique<IntValue>(
      true, Scales::boolScale, "chorusKeyFollow",
      kParameterIsAutomable | kParameterIsBoolean);

    value[ID::gain0] = std::make_unique<DecibelValue>(
      1.0, Scales::gainDecibel, "gain0", kParameterIsAutomable);
    value[ID::gain1] = std::make_unique<DecibelValue>(
      1.0, Scales::gainDecibel, "gain1", kParameterIsAutomable);
    value[ID::gain2] = std::make_unique<DecibelValue>(
      1.0, Scales::gainDecibel, "gain2", kParameterIsAutomable);
    value[ID::gain3] = std::make_unique<DecibelValue>(
      1.0, Scales::gainDecibel, "gain3", kParameterIsAutomable);
    value[ID::gain4] = std::make_unique<DecibelValue>(
      1.0, Scales::gainDecibel, "gain4", kParameterIsAutomable);
    value[ID::gain5] = std::make_unique<DecibelValue>(
      1.0, Scales::gainDecibel, "gain5", kParameterIsAutomable);
    value[ID::gain6] = std::make_unique<DecibelValue>(
      1.0, Scales::gainDecibel, "gain6", kParameterIsAutomable);
    value[ID::gain7] = std::make_unique<DecibelValue>(
      1.0, Scales::gainDecibel, "gain7", kParameterIsAutomable);

    value[ID::semi0] = std::make_unique<IntValue>(
      0, Scales::oscSemi, "semi0", kParameterIsAutomable | kParameterIsInteger);
    value[ID::semi1] = std::make_unique<IntValue>(
      0, Scales::oscSemi, "semi1", kParameterIsAutomable | kParameterIsInteger);
    value[ID::semi2] = std::make_unique<IntValue>(
      0, Scales::oscSemi, "semi2", kParameterIsAutomable | kParameterIsInteger);
    value[ID::semi3] = std::make_unique<IntValue>(
      0, Scales::oscSemi, "semi3", kParameterIsAutomable | kParameterIsInteger);
    value[ID::semi4] = std::make_unique<IntValue>(
      0, Scales::oscSemi, "semi4", kParameterIsAutomable | kParameterIsInteger);
    value[ID::semi5] = std::make_unique<IntValue>(
      0, Scales::oscSemi, "semi5", kParameterIsAutomable | kParameterIsInteger);
    value[ID::semi6] = std::make_unique<IntValue>(
      0, Scales::oscSemi, "semi6", kParameterIsAutomable | kParameterIsInteger);
    value[ID::semi7] = std::make_unique<IntValue>(
      0, Scales::oscSemi, "semi7", kParameterIsAutomable | kParameterIsInteger);

    value[ID::milli0] = std::make_unique<IntValue>(
      0, Scales::oscMilli, "milli0", kParameterIsAutomable | kParameterIsInteger);
    value[ID::milli1] = std::make_unique<IntValue>(
      0, Scales::oscMilli, "milli1", kParameterIsAutomable | kParameterIsInteger);
    value[ID::milli2] = std::make_unique<IntValue>(
      0, Scales::oscMilli, "milli2", kParameterIsAutomable | kParameterIsInteger);
    value[ID::milli3] = std::make_unique<IntValue>(
      0, Scales::oscMilli, "milli3", kParameterIsAutomable | kParameterIsInteger);
    value[ID::milli4] = std::make_unique<IntValue>(
      0, Scales::oscMilli, "milli4", kParameterIsAutomable | kParameterIsInteger);
    value[ID::milli5] = std::make_unique<IntValue>(
      0, Scales::oscMilli, "milli5", kParameterIsAutomable | kParameterIsInteger);
    value[ID::milli6] = std::make_unique<IntValue>(
      0, Scales::oscMilli, "milli6", kParameterIsAutomable | kParameterIsInteger);
    value[ID::milli7] = std::make_unique<IntValue>(
      0, Scales::oscMilli, "milli7", kParameterIsAutomable | kParameterIsInteger);

    value[ID::overtone0] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0), Scales::gainDecibel, "overtone0",
      kParameterIsAutomable);
    value[ID::overtone1] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 2), Scales::gainDecibel, "overtone1",
      kParameterIsAutomable);
    value[ID::overtone2] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 3), Scales::gainDecibel, "overtone2",
      kParameterIsAutomable);
    value[ID::overtone3] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 4), Scales::gainDecibel, "overtone3",
      kParameterIsAutomable);
    value[ID::overtone4] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 5), Scales::gainDecibel, "overtone4",
      kParameterIsAutomable);
    value[ID::overtone5] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 6), Scales::gainDecibel, "overtone5",
      kParameterIsAutomable);
    value[ID::overtone6] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 7), Scales::gainDecibel, "overtone6",
      kParameterIsAutomable);
    value[ID::overtone7] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 8), Scales::gainDecibel, "overtone7",
      kParameterIsAutomable);
    value[ID::overtone8] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 9), Scales::gainDecibel, "overtone8",
      kParameterIsAutomable);
    value[ID::overtone9] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 10), Scales::gainDecibel, "overtone9",
      kParameterIsAutomable);
    value[ID::overtone10] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 11), Scales::gainDecibel, "overtone10",
      kParameterIsAutomable);
    value[ID::overtone11] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 12), Scales::gainDecibel, "overtone11",
      kParameterIsAutomable);
    value[ID::overtone12] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 13), Scales::gainDecibel, "overtone12",
      kParameterIsAutomable);
    value[ID::overtone13] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 14), Scales::gainDecibel, "overtone13",
      kParameterIsAutomable);
    value[ID::overtone14] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 15), Scales::gainDecibel, "overtone14",
      kParameterIsAutomable);
    value[ID::overtone15] = std::make_unique<DecibelValue>(
      Scales::gainDecibel.invmap(1.0 / 16), Scales::gainDecibel, "overtone15",
      kParameterIsAutomable);

    value[ID::chordGain0] = std::make_unique<DecibelValue>(
      1.0, Scales::gainDecibel, "chordGain0", kParameterIsAutomable);
    value[ID::chordGain1] = std::make_unique<DecibelValue>(
      1.0, Scales::gainDecibel, "chordGain1", kParameterIsAutomable);
    value[ID::chordGain2] = std::make_unique<DecibelValue>(
      1.0, Scales::gainDecibel, "chordGain2", kParameterIsAutomable);
    value[ID::chordGain3] = std::make_unique<DecibelValue>(
      1.0, Scales::gainDecibel, "chordGain3", kParameterIsAutomable);

    value[ID::chordSemi0] = std::make_unique<IntValue>(
      0, Scales::oscSemi, "chordSemi0", kParameterIsAutomable | kParameterIsInteger);
    value[ID::chordSemi1] = std::make_unique<IntValue>(
      0, Scales::oscSemi, "chordSemi1", kParameterIsAutomable | kParameterIsInteger);
    value[ID::chordSemi2] = std::make_unique<IntValue>(
      0, Scales::oscSemi, "chordSemi2", kParameterIsAutomable | kParameterIsInteger);
    value[ID::chordSemi3] = std::make_unique<IntValue>(
      0, Scales::oscSemi, "chordSemi3", kParameterIsAutomable | kParameterIsInteger);

    value[ID::chordMilli0] = std::make_unique<IntValue>(
      0, Scales::oscMilli, "chordMilli0", kParameterIsAutomable | kParameterIsInteger);
    value[ID::chordMilli1] = std::make_unique<IntValue>(
      0, Scales::oscMilli, "chordMilli1", kParameterIsAutomable | kParameterIsInteger);
    value[ID::chordMilli2] = std::make_unique<IntValue>(
      0, Scales::oscMilli, "chordMilli2", kParameterIsAutomable | kParameterIsInteger);
    value[ID::chordMilli3] = std::make_unique<IntValue>(
      0, Scales::oscMilli, "chordMilli3", kParameterIsAutomable | kParameterIsInteger);

    value[ID::chordPan0] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "chordPan0", kParameterIsAutomable);
    value[ID::chordPan1] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "chordPan1", kParameterIsAutomable);
    value[ID::chordPan2] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "chordPan2", kParameterIsAutomable);
    value[ID::chordPan3] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "chordPan3", kParameterIsAutomable);

    value[ID::gain]
      = std::make_unique<LogValue>(0.5, Scales::gain, "gain", kParameterIsAutomable);
    value[ID::gainBoost] = std::make_unique<LinearValue>(
      0.0, Scales::gainBoost, "gainBoost", kParameterIsAutomable);
    value[ID::gainA] = std::make_unique<LogValue>(
      0.0, Scales::envelopeA, "gainA", kParameterIsAutomable);
    value[ID::gainD] = std::make_unique<LogValue>(
      0.5, Scales::envelopeD, "gainD", kParameterIsAutomable);
    value[ID::gainS] = std::make_unique<LogValue>(
      0.5, Scales::envelopeS, "gainS", kParameterIsAutomable);
    value[ID::gainR] = std::make_unique<LogValue>(
      0.0, Scales::envelopeR, "gainR", kParameterIsAutomable);
    value[ID::gainEnvelopeCurve] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "gainEnvelopeCurve", kParameterIsAutomable);

    value[ID::lowShelfPitch] = std::make_unique<LinearValue>(
      0.0, Scales::shelvingPitch, "lowShelfPitch", kParameterIsAutomable);
    value[ID::lowShelfGain] = std::make_unique<DecibelValue>(
      0.5, Scales::shelvingGain, "lowShelfGain", kParameterIsAutomable);
    value[ID::highShelfPitch] = std::make_unique<LinearValue>(
      0.0, Scales::shelvingPitch, "highShelfPitch", kParameterIsAutomable);
    value[ID::highShelfGain] = std::make_unique<DecibelValue>(
      0.5, Scales::shelvingGain, "highShelfGain", kParameterIsAutomable);

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
    preset2479,
    preset5Times7,
    presetAliasing,
    presetAnotherPad,
    presetCalmDecay,
    presetChorusKeyFollow,
    presetClick,
    presetDenseDetune,
    presetDim,
    presetET12ToneCluster,
    presetET13Bright,
    presetET15,
    presetET19,
    presetET7Cluster,
    presetEasy4th,
    presetLooseTuning,
    presetMilliDetune,
    presetModuloOrgan,
    presetNarrowModulo,
    presetNoisyFeedback,
    presetOddOvertone,
    presetRandomBell,
    presetRandomPitchStereo,
    presetReverseSemiLowPop,
    presetShelving,
    presetSinPad,
    presetSlowRotation,
    presetSolidHighBlurredLow,
    presetSomePad,
    presetSparseOvertone,
    presetThin,
    presetThisSynthIsNotGoodForBass,
    presetTotallyRandom,
    presetWeirdPan,

    Preset_ENUM_LENGTH,
  };

  std::array<const char *, 35> programName{
    "Default",
    "2479",
    "5Times7",
    "Aliasing",
    "AnotherPad",
    "CalmDecay",
    "ChorusKeyFollow",
    "Click",
    "DenseDetune",
    "Dim",
    "ET12ToneCluster",
    "ET13Bright",
    "ET15",
    "ET19",
    "ET7Cluster",
    "Easy4th",
    "LooseTuning",
    "MilliDetune",
    "ModuloOrgan",
    "NarrowModulo",
    "NoisyFeedback",
    "OddOvertone",
    "RandomBell",
    "RandomPitchStereo",
    "ReverseSemiLowPop",
    "Shelving",
    "SinPad",
    "SlowRotation",
    "SolidHighBlurredLow",
    "SomePad",
    "SparseOvertone",
    "Thin",
    "ThisSynthIsNotGoodForBass",
    "TotallyRandom",
    "WeirdPan",
  };

#ifndef TEST_BUILD
  void initProgramName(uint32_t index, String &programName)
  {
    programName = this->programName[index];
  }

  void loadProgram(uint32_t index);
#endif
};
