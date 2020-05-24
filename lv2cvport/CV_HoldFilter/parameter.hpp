// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_HoldFilter.
//
// CV_HoldFilter is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_HoldFilter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_HoldFilter.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

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

namespace ParameterID {
enum ID {
  gain,
  cutoff,
  resonance,
  edge,
  pulseWidth,
  filterType,
  highpass,
  overSampling,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::IntScale<double> boolScale;
  static SomeDSP::LogScale<double> gain;
  static SomeDSP::LogScale<double> cutoff;
  static SomeDSP::LogScale<double> resonance;
  static SomeDSP::LinearScale<double> pulseWidth;
  static SomeDSP::LinearScale<double> edge;
  static SomeDSP::IntScale<double> filterType;
};

struct GlobalParameter : public ParameterInterface {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using ID = ParameterID::ID;
    using LinearValue = FloatValue<SomeDSP::LinearScale<double>>;
    using LogValue = FloatValue<SomeDSP::LogScale<double>>;

    value[ID::gain] = std::make_unique<LogValue>(
      Scales::gain.invmap(0.5), Scales::gain, "gain",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::cutoff] = std::make_unique<LogValue>(
      Scales::cutoff.invmap(2000), Scales::cutoff, "cutoff",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::resonance] = std::make_unique<LogValue>(
      Scales::resonance.invmap(0.1), Scales::resonance, "resonance",
      kParameterIsAutomable | kParameterIsLogarithmic);
    value[ID::pulseWidth] = std::make_unique<LinearValue>(
      Scales::pulseWidth.invmap(1.0), Scales::pulseWidth, "pulseWidth",
      kParameterIsAutomable);
    value[ID::filterType] = std::make_unique<IntValue>(
      Scales::filterType.invmap(0.1), Scales::filterType, "filterType",
      kParameterIsAutomable | kParameterIsInteger);
    value[ID::edge] = std::make_unique<LinearValue>(
      Scales::edge.invmap(1.0), Scales::edge, "edge", kParameterIsAutomable);
    value[ID::highpass] = std::make_unique<IntValue>(
      0, Scales::boolScale, "highpass", kParameterIsAutomable | kParameterIsBoolean);
    value[ID::overSampling] = std::make_unique<IntValue>(
      1, Scales::boolScale, "overSampling", kParameterIsAutomable | kParameterIsBoolean);
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
};
