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

#include "DistrhoPlugin.hpp"

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

struct ValueInterface {
  virtual void setParameterRange(Parameter &parameter) = 0;
  virtual double getRaw() const = 0;
  virtual double getNormalized() = 0;
  virtual double getDefaultNormalized() = 0;
  virtual void setFromRaw(double value) = 0;
  virtual void setFromNormalized(double value) = 0;
};

template<typename Scale> struct InternalValue : public ValueInterface {
  double defaultNormalized;
  double raw;
  Scale &scale;

  InternalValue(double defaultNormalized, Scale &scale)
    : defaultNormalized(defaultNormalized)
    , raw(scale.map(defaultNormalized))
    , scale(scale)
  {
  }

  void setParameterRange(Parameter &parameter) override
  {
    parameter.ranges = ScaledParameterRanges<Scale>(defaultNormalized, scale);
  }

  inline double getRaw() const override { return raw; }
  double getNormalized() override { return scale.invmap(raw); }
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
