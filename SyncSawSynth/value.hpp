// (c) 2019 Takamitsu Endo
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

#ifndef TEST_BUILD
#include "DistrhoPlugin.hpp"
#endif

#ifndef TEST_BUILD
template<typename Scale> class ScaledParameterRanges : public ParameterRanges {
public:
  ScaledParameterRanges(double defaultNormalized, Scale &scale)
    : ParameterRanges(scale.map(defaultNormalized), scale.getMin(), scale.getMax())
    , scale(scale)
  {
  }

  float getNormalizedValue(const float &value) const noexcept
  {
    return scale.invmap(value);
  }

  float getFixedAndNormalizedValue(const float &value) const noexcept
  {
    return scale.invmap(value);
  }

  float getUnnormalizedValue(const float &value) const noexcept
  {
    return scale.map(value);
  }

protected:
  Scale &scale;
};
#endif

struct ValueInterface {
#ifndef TEST_BUILD
  virtual void setParameterRange(Parameter &parameter) = 0;
#endif
  virtual double getRaw() const = 0;
  virtual double getNormalized() = 0;
  virtual double getDefaultRaw() = 0;
  virtual double getDefaultNormalized() = 0;
  virtual void setFromRaw(double value) = 0;
  virtual void setFromNormalized(double value) = 0;
};

template<typename Scale> struct InternalValue : public ValueInterface {
  double defaultNormalized;
  double raw;
  Scale &scale;
  const char *name;
  uint32_t hints;

  InternalValue(double defaultNormalized, Scale &scale, const char *name, uint32_t hints)
    : defaultNormalized(defaultNormalized)
    , raw(scale.map(defaultNormalized))
    , scale(scale)
    , name(name)
    , hints(hints)
  {
  }

#ifndef TEST_BUILD
  void setParameterRange(Parameter &parameter) override
  {
    parameter.name = name;
    parameter.hints = hints;
    parameter.ranges = ScaledParameterRanges<Scale>(defaultNormalized, scale);
  }
#endif

  inline double getRaw() const override { return raw; }
  double getNormalized() override { return scale.invmap(raw); }
  double getDefaultRaw() { return scale.map(defaultNormalized); }
  inline double getDefaultNormalized() override { return defaultNormalized; }

  void setFromRaw(double value) override
  {
    value = value < scale.getMin() ? scale.getMin()
                                   : value > scale.getMax() ? scale.getMax() : value;
    raw = value;
  }

  void setFromNormalized(double value) override
  {
    value = value < 0.0 ? 0.0 : value > 1.0 ? 1.0 : value;
    raw = scale.map(value);
  }
};
