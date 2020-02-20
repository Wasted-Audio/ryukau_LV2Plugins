// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CubicPadSynth.
//
// CubicPadSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CubicPadSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CubicPadSynth.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "DistrhoUI.hpp"
#include "parameter.hpp"

class PluginUI : public UI {
public:
  PluginUI(uint width = 0, uint height = 0) : UI(width, height) {}

  virtual void updateValue(uint32_t index, float normalized) = 0;
  virtual void updateState(std::string key, std::string value) = 0;
};
