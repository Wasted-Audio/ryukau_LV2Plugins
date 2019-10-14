// (c) 2019 Takamitsu Endo
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

#include "dsp/scale.hpp"
#include "value.hpp"

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
  static BoolScale<double> boolScale;
  static LinearScale<double> defaultScale;
  static LogScale<double> time;
  static SPolyScale<double> offset;
  static LogScale<double> lfoTimeAmount;
  static LogScale<double> lfoToneAmount;
  static LogScale<double> lfoFrequency;
  static LogScale<double> lfoShape;
  static LinearScale<double> lfoInitialPhase;
  static LogScale<double> smoothness;
  static LogScale<double> toneCutoff;
  static LogScale<double> toneQ;
  static LogScale<double> toneMix; // internal
  static LogScale<double> dckill;
  static LogScale<double> dckillMix; // internal
};

struct GlobalParameter {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    value[ParameterID::bypass]
      = std::make_unique<InternalValue<BoolScale<double>>>(0.0, Scales::boolScale);
    value[ParameterID::time]
      = std::make_unique<InternalValue<LogScale<double>>>(0.5, Scales::time);
    value[ParameterID::feedback]
      = std::make_unique<InternalValue<LinearScale<double>>>(0.625, Scales::defaultScale);
    value[ParameterID::offset]
      = std::make_unique<InternalValue<SPolyScale<double>>>(0.5, Scales::offset);
    value[ParameterID::wetMix]
      = std::make_unique<InternalValue<LinearScale<double>>>(0.75, Scales::defaultScale);
    value[ParameterID::dryMix]
      = std::make_unique<InternalValue<LinearScale<double>>>(1.0, Scales::defaultScale);
    value[ParameterID::tempoSync]
      = std::make_unique<InternalValue<BoolScale<double>>>(0.0, Scales::boolScale);
    value[ParameterID::negativeFeedback]
      = std::make_unique<InternalValue<BoolScale<double>>>(0.0, Scales::boolScale);
    value[ParameterID::lfoTimeAmount]
      = std::make_unique<InternalValue<LogScale<double>>>(0.0, Scales::lfoTimeAmount);
    value[ParameterID::lfoToneAmount]
      = std::make_unique<InternalValue<LogScale<double>>>(0.0, Scales::lfoToneAmount);
    value[ParameterID::lfoFrequency]
      = std::make_unique<InternalValue<LogScale<double>>>(0.5, Scales::lfoFrequency);
    value[ParameterID::lfoShape]
      = std::make_unique<InternalValue<LogScale<double>>>(0.5, Scales::lfoShape);
    value[ParameterID::lfoInitialPhase]
      = std::make_unique<InternalValue<LinearScale<double>>>(
        0.0, Scales::lfoInitialPhase);
    value[ParameterID::lfoHold]
      = std::make_unique<InternalValue<BoolScale<double>>>(0.0, Scales::boolScale);
    value[ParameterID::smoothness]
      = std::make_unique<InternalValue<LogScale<double>>>(0.3, Scales::smoothness);
    value[ParameterID::inSpread]
      = std::make_unique<InternalValue<LinearScale<double>>>(0.0, Scales::defaultScale);
    value[ParameterID::inPan]
      = std::make_unique<InternalValue<LinearScale<double>>>(0.5, Scales::defaultScale);
    value[ParameterID::outSpread]
      = std::make_unique<InternalValue<LinearScale<double>>>(0.0, Scales::defaultScale);
    value[ParameterID::outPan]
      = std::make_unique<InternalValue<LinearScale<double>>>(0.5, Scales::defaultScale);
    value[ParameterID::toneCutoff]
      = std::make_unique<InternalValue<LogScale<double>>>(1.0, Scales::toneCutoff);
    value[ParameterID::toneQ]
      = std::make_unique<InternalValue<LogScale<double>>>(0.9, Scales::toneQ);
    value[ParameterID::dckill]
      = std::make_unique<InternalValue<LogScale<double>>>(0.0, Scales::dckill);
  }

  void initParameter(uint32_t index, Parameter &parameter)
  {
    if (index >= value.size()) return;
    value[index]->setParameterRange(parameter);
  }

  double getParameterValue(uint32_t index) const
  {
    if (index >= value.size()) return 0.0;
    return value[index]->getRaw();
  }

  void setParameterValue(uint32_t index, float raw)
  {
    if (index >= value.size()) return;
    value[index]->setFromRaw(raw);
  }

  double parameterChanged(uint32_t index, float raw)
  {
    if (index >= value.size()) return 0.0;
    value[index]->setFromRaw(raw);
    return value[index]->getNormalized();
  }

  double updateValue(uint32_t index, float normalized)
  {
    if (index >= value.size()) return 0.0;
    value[index]->setFromNormalized(normalized);
    return value[index]->getRaw();
  }
};
