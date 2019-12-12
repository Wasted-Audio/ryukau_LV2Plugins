// (c) 2019 Takamitsu Endo
//
// This file is part of TrapezoidSynth.
//
// TrapezoidSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TrapezoidSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TrapezoidSynth.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cstdlib>
#include <iostream>
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

  osc1Semi,
  osc1Cent,
  osc1PitchDrift,
  osc1Slope,
  osc1PulseWidth,
  osc1Feedback,

  osc2Semi,
  osc2Cent,
  osc2Overtone,
  osc2Slope,
  osc2PulseWidth,
  pmOsc2ToOsc1,

  gain,
  gainA,
  gainD,
  gainS,
  gainR,
  gainCurve,
  gainEnvRetrigger,

  filterOrder,
  filterCutoff,
  filterFeedback,
  filterSaturation,
  filterEnvToCutoff,
  filterKeyToCutoff,
  oscMixToFilterCutoff,
  filterA,
  filterD,
  filterS,
  filterR,
  filterCurve,
  filterEnvToOctave,
  filterEnvRetrigger,

  modEnv1Attack,
  modEnv1Curve,
  modEnv1ToPhaseMod,
  modEnv1Retrigger,

  modEnv2Attack,
  modEnv2Curve,
  modEnv2ToFeedback,
  modEnv2ToLFOFrequency,
  modEnv2ToOsc2Slope,
  modEnv2ToShifter1,
  modEnv2Retrigger,

  shifter1Semi,
  shifter1Cent,
  shifter1Gain,
  shifter2Semi,
  shifter2Cent,
  shifter2Gain,

  lfoType,
  lfoTempoSync,
  lfoFrequency,
  lfoShape,
  lfoToPitch,
  lfoToSlope,
  lfoToPulseWidth,
  lfoToCutoff,

  oscMix,
  octave,

  pitchSlideType,
  pitchSlide,
  pitchSlideOffset,

  smoothness,

  pitchBend,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;

  static SomeDSP::LinearScale<double> semi;
  static SomeDSP::LinearScale<double> cent;
  static SomeDSP::LinearScale<double> overtone;
  static SomeDSP::LinearScale<double> pitchDrift;
  static SomeDSP::LogScale<double> oscSlope;
  static SomeDSP::LogScale<double> oscFeedback;
  static SomeDSP::LogScale<double> phaseModulation;

  static SomeDSP::LogScale<double> envelopeA;
  static SomeDSP::LogScale<double> envelopeD;
  static SomeDSP::LogScale<double> envelopeS;
  static SomeDSP::LogScale<double> envelopeR;

  static SomeDSP::LogScale<double> filterCutoff;
  static SomeDSP::LogScale<double> filterResonance;
  static SomeDSP::LogScale<double> filterSaturation;
  static SomeDSP::IntScale<double> filterOrder;
  static SomeDSP::SPolyScale<double> filterEnvToCutoff;
  static SomeDSP::LinearScale<double> filterKeyToCutoff;
  static SomeDSP::LinearScale<double> oscMixToFilterCutoff;
  static SomeDSP::LinearScale<double> filterEnvToOctave;

  static SomeDSP::LogScale<double> modEnvelopeA;
  static SomeDSP::LogScale<double> modEnvelopeCurve;
  static SomeDSP::LogScale<double> modEnvToShifter;

  static SomeDSP::LinearScale<double> shifterSemi;
  static SomeDSP::LinearScale<double> shifterCent;

  static SomeDSP::IntScale<double> lfoType;
  static SomeDSP::LogScale<double> lfoFrequency;
  static SomeDSP::LogScale<double> lfoSlope;

  static SomeDSP::LinearScale<double> octave;

  static SomeDSP::IntScale<double> pitchSlideType;
  static SomeDSP::LogScale<double> pitchSlide;
  static SomeDSP::LinearScale<double> pitchSlideOffset;

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
    using SPolyValue = FloatValue<SomeDSP::SPolyScale<double>>;

    value[ID::bypass] = std::make_unique<IntValue>(
      false, Scales::boolScale, "bypass", kParameterIsAutomable | kParameterIsBoolean);

    value[ID::osc1Semi] = std::make_unique<LinearValue>(
      0.5, Scales::semi, "osc1Semi", kParameterIsAutomable);
    value[ID::osc1Cent] = std::make_unique<LinearValue>(
      0.5, Scales::cent, "osc1Cent", kParameterIsAutomable);
    value[ID::osc1PitchDrift] = std::make_unique<LinearValue>(
      0.0, Scales::pitchDrift, "osc1PitchDrift", kParameterIsAutomable);
    value[ID::osc1Slope] = std::make_unique<LogValue>(
      0.5, Scales::oscSlope, "osc1Slope", kParameterIsAutomable);
    value[ID::osc1PulseWidth] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "osc1PulseWidth", kParameterIsAutomable);
    value[ID::osc1Feedback] = std::make_unique<LogValue>(
      0.0, Scales::oscFeedback, "osc1Feedback", kParameterIsAutomable);

    value[ID::osc2Semi] = std::make_unique<LinearValue>(
      0.5, Scales::semi, "osc2Semi", kParameterIsAutomable);
    value[ID::osc2Cent] = std::make_unique<LinearValue>(
      0.5, Scales::cent, "osc2Cent", kParameterIsAutomable);
    value[ID::osc2Overtone] = std::make_unique<LinearValue>(
      0, Scales::overtone, "osc2Overtone", kParameterIsAutomable);
    value[ID::osc2Slope] = std::make_unique<LogValue>(
      0.5, Scales::oscSlope, "osc2Slope", kParameterIsAutomable);
    value[ID::osc2PulseWidth] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "osc2PulseWidth", kParameterIsAutomable);
    value[ID::pmOsc2ToOsc1] = std::make_unique<LogValue>(
      0.0, Scales::phaseModulation, "pmOsc2ToOsc1", kParameterIsAutomable);

    value[ID::gain]
      = std::make_unique<LogValue>(0.5, Scales::gain, "gain", kParameterIsAutomable);
    value[ID::gainA] = std::make_unique<LogValue>(
      0.0, Scales::envelopeA, "gainA", kParameterIsAutomable);
    value[ID::gainD] = std::make_unique<LogValue>(
      0.5, Scales::envelopeD, "gainD", kParameterIsAutomable);
    value[ID::gainS] = std::make_unique<LogValue>(
      0.8, Scales::envelopeS, "gainS", kParameterIsAutomable);
    value[ID::gainR] = std::make_unique<LogValue>(
      0.1, Scales::envelopeR, "gainR", kParameterIsAutomable);
    value[ID::gainCurve] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "gainCurve", kParameterIsAutomable);
    value[ID::gainEnvRetrigger] = std::make_unique<IntValue>(
      true, Scales::boolScale, "gainEnvRetrigger",
      kParameterIsAutomable | kParameterIsBoolean);

    value[ID::filterOrder] = std::make_unique<IntValue>(
      7, Scales::filterOrder, "filterOrder", kParameterIsAutomable | kParameterIsInteger);
    value[ID::filterCutoff] = std::make_unique<LogValue>(
      1.0, Scales::filterCutoff, "filterCutoff", kParameterIsAutomable);
    value[ID::filterFeedback] = std::make_unique<LogValue>(
      0.0, Scales::filterResonance, "filterFeedback", kParameterIsAutomable);
    value[ID::filterSaturation] = std::make_unique<LogValue>(
      0.3, Scales::filterSaturation, "filterSaturation", kParameterIsAutomable);
    value[ID::filterEnvToCutoff] = std::make_unique<SPolyValue>(
      0.5, Scales::filterEnvToCutoff, "filterEnvToCutoff", kParameterIsAutomable);
    value[ID::filterKeyToCutoff] = std::make_unique<LinearValue>(
      0.5, Scales::filterKeyToCutoff, "filterKeyToCutoff", kParameterIsAutomable);
    value[ID::oscMixToFilterCutoff] = std::make_unique<LinearValue>(
      0.0, Scales::oscMixToFilterCutoff, "oscMixToFilterCutoff", kParameterIsAutomable);

    value[ID::filterA] = std::make_unique<LogValue>(
      0.0, Scales::envelopeA, "filterA", kParameterIsAutomable);
    value[ID::filterD] = std::make_unique<LogValue>(
      0.5, Scales::envelopeD, "filterD", kParameterIsAutomable);
    value[ID::filterS] = std::make_unique<LogValue>(
      0.0, Scales::envelopeS, "filterS", kParameterIsAutomable);
    value[ID::filterR] = std::make_unique<LogValue>(
      0.5, Scales::envelopeR, "filterR", kParameterIsAutomable);
    value[ID::filterCurve] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "filterCurve", kParameterIsAutomable);
    value[ID::filterEnvToOctave] = std::make_unique<LinearValue>(
      0.5, Scales::filterEnvToOctave, "filterEnvToOctave", kParameterIsAutomable);
    value[ID::filterEnvRetrigger] = std::make_unique<IntValue>(
      true, Scales::boolScale, "filterEnvRetrigger",
      kParameterIsAutomable | kParameterIsBoolean);

    value[ID::modEnv1Attack] = std::make_unique<LogValue>(
      0.0, Scales::modEnvelopeA, "modEnv1Attack", kParameterIsAutomable);
    value[ID::modEnv1Curve] = std::make_unique<LogValue>(
      0.5, Scales::modEnvelopeCurve, "modEnv1Curve", kParameterIsAutomable);
    value[ID::modEnv1ToPhaseMod] = std::make_unique<LogValue>(
      0.0, Scales::phaseModulation, "modEnv1ToPhaseMod", kParameterIsAutomable);
    value[ID::modEnv1Retrigger] = std::make_unique<IntValue>(
      true, Scales::boolScale, "modEnv1Retrigger",
      kParameterIsAutomable | kParameterIsBoolean);

    value[ID::modEnv2Attack] = std::make_unique<LogValue>(
      0.0, Scales::modEnvelopeA, "modEnv2Attack", kParameterIsAutomable);
    value[ID::modEnv2Curve] = std::make_unique<LogValue>(
      0.5, Scales::modEnvelopeCurve, "modEnv2Curve", kParameterIsAutomable);
    value[ID::modEnv2ToFeedback] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "modEnv2ToFeedback", kParameterIsAutomable);
    value[ID::modEnv2ToLFOFrequency] = std::make_unique<LogValue>(
      0.0, Scales::lfoFrequency, "modEnv2ToLFOFrequency", kParameterIsAutomable);
    value[ID::modEnv2ToOsc2Slope] = std::make_unique<LogValue>(
      0.0, Scales::oscSlope, "modEnv2ToOsc2Slope", kParameterIsAutomable);
    value[ID::modEnv2ToShifter1] = std::make_unique<LogValue>(
      0.0, Scales::modEnvToShifter, "modEnv2ToShifter1", kParameterIsAutomable);
    value[ID::modEnv2Retrigger] = std::make_unique<IntValue>(
      true, Scales::boolScale, "modEnv2Retrigger",
      kParameterIsAutomable | kParameterIsBoolean);

    value[ID::shifter1Semi] = std::make_unique<LinearValue>(
      7.001 / Scales::shifterSemi.getMax(), Scales::shifterSemi, "shifter1Semi",
      kParameterIsAutomable);
    value[ID::shifter1Cent] = std::make_unique<LinearValue>(
      0.0, Scales::shifterCent, "shifter1Cent", kParameterIsAutomable);
    value[ID::shifter1Gain] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "shifter1Gain", kParameterIsAutomable);
    value[ID::shifter2Semi] = std::make_unique<LinearValue>(
      14.001 / Scales::shifterSemi.getMax(), Scales::shifterSemi, "shifter2Semi",
      kParameterIsAutomable);
    value[ID::shifter2Cent] = std::make_unique<LinearValue>(
      0.0, Scales::shifterCent, "shifter2Cent", kParameterIsAutomable);
    value[ID::shifter2Gain] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "shifter2Gain", kParameterIsAutomable);

    value[ID::lfoType] = std::make_unique<IntValue>(
      0, Scales::lfoType, "lfoType", kParameterIsAutomable | kParameterIsInteger);
    value[ID::lfoTempoSync] = std::make_unique<IntValue>(
      0, Scales::boolScale, "lfoTempoSync", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::lfoFrequency] = std::make_unique<LogValue>(
      0.5, Scales::lfoFrequency, "lfoFrequency", kParameterIsAutomable);
    value[ID::lfoShape] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "lfoShape", kParameterIsAutomable);
    value[ID::lfoToPitch] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "lfoToPitch", kParameterIsAutomable);
    value[ID::lfoToSlope] = std::make_unique<LogValue>(
      0.0, Scales::lfoSlope, "lfoToSlope", kParameterIsAutomable);
    value[ID::lfoToPulseWidth] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "lfoToPulseWidth", kParameterIsAutomable);
    value[ID::lfoToCutoff] = std::make_unique<LinearValue>(
      0.0, Scales::defaultScale, "lfoToCutoff", kParameterIsAutomable);

    value[ID::oscMix] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "oscMix", kParameterIsAutomable);
    value[ID::octave] = std::make_unique<LinearValue>(
      0.5, Scales::octave, "octave", kParameterIsAutomable);

    value[ID::pitchSlideType] = std::make_unique<IntValue>(
      1, Scales::pitchSlideType, "pitchSlideType",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::pitchSlide] = std::make_unique<LogValue>(
      0.1, Scales::pitchSlide, "pitchSlide", kParameterIsAutomable);
    value[ID::pitchSlideOffset] = std::make_unique<LinearValue>(
      0.5, Scales::pitchSlideOffset, "pitchSlideOffset", kParameterIsAutomable);

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
    presetBassAndSweep,
    presetBelow,
    presetBrokenModulation,
    presetDetuneWhileSliding,
    presetDetunedPhaseMod,
    presetDrift,
    presetGroan,
    presetLFOFreqDecay,
    presetMindlessModulation,
    presetMinor3rdShift,
    presetOctaveDecay,
    presetOsc1PitchLFO,
    presetOverTheNyquist,
    presetOvertone19,
    presetResShortDecay,
    presetSaturation,
    presetShifterPseudoResonance,
    presetSubBassDrum,
    presetTpzRoar,

    Preset_ENUM_LENGTH,
  };

  std::array<const char *, 20> programName{
    "Default",
    "BassAndSweep",
    "Below",
    "BrokenModulation",
    "DetuneWhileSliding",
    "DetunedPhaseMod",
    "Drift",
    "Groan",
    "LFOFreqDecay",
    "MindlessModulation",
    "Minor3rdShift",
    "OctaveDecay",
    "Osc1PitchLFO",
    "OverTheNyquist",
    "Overtone19",
    "ResShortDecay",
    "Saturation",
    "ShifterPseudoResonance",
    "SubBassDrum",
    "TpzRoar",
  };

  void initProgramName(uint32_t index, String &programName)
  {
    programName = this->programName[index];
  }

  void loadProgram(uint32_t index);

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
