// (c) 2019 Takamitsu Endo
//
// This file is part of FDNCymbal.
//
// FDNCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// FDNCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with FDNCymbal.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <string>

#include "knob.hpp"

template<typename Scale> class NumberKnob : public Knob {
public:
  explicit NumberKnob(
    NanoWidget *group, PluginUI *ui, FontId fontId, Scale &scale, uint32_t offset = 0)
    : Knob(group, ui), fontId(fontId), scale(scale), offset(offset)
  {
  }

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();
    const auto centerX = width / 2;
    const auto centerY = height / 2;

    // Arc.
    strokeColor(isMouseEntered ? highlightColor : arcColor);
    lineCap(ROUND);
    lineJoin(ROUND);
    strokeWidth(halfArcWidth * 2.0f);
    const auto radius = (centerX > centerY ? centerY : centerX) - halfArcWidth;
    beginPath();
    arc(
      centerX, centerY, radius, float(SomeDSP::pi) / 2.0f - arcNotchHalf,
      float(SomeDSP::pi) / 2.0f + arcNotchHalf, CCW);
    stroke();

    // Tick for default value. Sharing color and style with Arc.
    strokeWidth(halfArcWidth / 2.0f);
    beginPath();
    auto point = mapValueToArc(defaultValue, -radius * defaultTickLength);
    moveTo(point.getX() + centerX, point.getY() + centerY);
    point = mapValueToArc(defaultValue, -radius);
    lineTo(point.getX() + centerX, point.getY() + centerY);
    stroke();

    // Text.
    fontFaceId(fontId);
    fontSize(textSize);
    textAlign(ALIGN_CENTER | ALIGN_MIDDLE);
    text(
      centerX, centerY, std::to_string(int32_t(floor(scale.map(value))) + offset).c_str(),
      nullptr);

    // Tip.
    const auto tip = mapValueToArc(value, -radius);
    fillColor(tipColor);
    beginPath();
    arc(
      tip.getX() + centerX, tip.getY() + centerY, halfArcWidth, -SomeDSP::pi, SomeDSP::pi,
      CCW);
    fill();
  }

protected:
  float textSize = 14.0f;
  FontId fontId = -1;
  Scale &scale;
  uint32_t offset;
};
