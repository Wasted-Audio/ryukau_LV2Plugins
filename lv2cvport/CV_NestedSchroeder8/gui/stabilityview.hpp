// (c) 2020 Takamitsu Endo
//
// This file is part of CV_NestedSchroeder8.
//
// CV_NestedSchroeder8 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_NestedSchroeder8 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_NestedSchroeder8.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../parameter.hpp"

#include "Widget.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

class StabilityView : public NanoWidget {
public:
  explicit StabilityView(NanoWidget *group, FontId fontId)
    : NanoWidget(group), fontId(fontId)
  {
  }

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    // const auto width = getWidth();
    // const auto height = getHeight();

    // Info text.
    fontSize(textSize);
    textAlign(ALIGN_LEFT | ALIGN_MIDDLE);

    fillColor(stableS ? colorFore : colorUnstable);
    std::string textS = "S=" + std::to_string(valueS);
    text(0, 0, textS.c_str(), nullptr);

    fillColor(stableT ? colorFore : colorUnstable);
    std::string textT = "T=" + std::to_string(valueT);
    text(0, textSize, textT.c_str(), nullptr);

    fillColor(stableA ? colorFore : colorUnstable);
    std::string textA = "A=" + std::to_string(valueA);
    text(0, 2 * textSize, textA.c_str(), nullptr);
  }

  void update(GlobalParameter &param)
  {
    using ID = ParameterID::ID;

    const auto mul = param.value[ID::outerFeedMultiply]->getFloat();

    valueS = 0;
    valueT = 0;
    valueA = 0;
    for (size_t idx = 0; idx < nestingDepth; ++idx) {
      auto absed = fabsf(mul * param.value[ID::outerFeed0 + idx]->getFloat());

      valueS += absed;
      valueT += absed * absed;
      if (valueA < absed) valueA = absed;
    }

    stableS = valueS < 1.0f;
    stableT = valueT < 1.0f / nestingDepth;
    stableA = valueA < 1.0f / nestingDepth;

    repaint();
  }

  void setForegroundColor(Color color) { colorFore = color; }
  void setBackgroundColor(Color color) { colorBack = color; }
  void setTextSize(float size) { textSize = size < 0.0f ? 0.0f : size; }

protected:
  Color colorFore{0, 0, 0};
  Color colorBack{0xff, 0xff, 0xff};
  Color colorUnstable{0xff, 0x00, 0x00};

  FontId fontId = -1;
  float textSize = 14.0f;

  float valueS = 0;
  float valueT = 0;
  float valueA = 0;

  bool stableS = false;
  bool stableT = false;
  bool stableA = false;
};
