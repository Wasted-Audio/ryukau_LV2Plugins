// (c) 2019 Takamitsu Endo
//
// This file is part of WaveCymbal.
//
// WaveCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// WaveCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with WaveCymbal.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../ui.hpp"
#include "Widget.hpp"

class ValueWidget : public NanoWidget {
public:
  uint32_t id;

  explicit ValueWidget(NanoWidget *group, PluginUI *ui, double value)
    : NanoWidget(group), ui(ui), value(value)
  {
  }

  virtual double getValue() { return value; }
  virtual void setValue(double value) { this->value = value; }
  virtual void updateValue()
  {
    if (ui == nullptr) return;
    ui->updateValue(id, value);
  }

protected:
  PluginUI *ui = nullptr;
  double value = 0.0;
};
