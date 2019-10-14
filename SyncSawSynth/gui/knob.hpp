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

#include "../dsp/constants.hpp"
#include "valuewidget.hpp"

class Knob : public ValueWidget {
public:
  explicit Knob(NanoWidget *group, PluginUI *ui) : ValueWidget(group, ui, 0.1f) {}

  Point<float> mapValueToArc(float normalized, float length)
  {
    auto radian = (2.0 * normalized - 1.0) * (SomeDSP::pi - arcNotchHalf);
    return Point<float>(-sin(radian) * length, cos(radian) * length);
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

    // Line from center to tip.
    strokeColor(tipColor);
    beginPath();
    moveTo(centerX, centerY);
    const auto tip = mapValueToArc(value, -radius);
    lineTo(tip.getX() + centerX, tip.getY() + centerY);
    stroke();

    // Tip.
    fillColor(tipColor);
    beginPath();
    arc(tip.getX() + centerX, tip.getY() + centerY, halfArcWidth, -SomeDSP::pi, SomeDSP::pi, CCW);
    fill();
  }

  bool onMouse(const MouseEvent &ev) override
  {
    if (ev.press && contains(ev.pos)) {
      if (ev.button == 1) {
        isMouseLeftDown = true;
        anchorPoint = ev.pos;
        if (ev.mod & kModifierControl) {
          value = defaultValue;
          updateValue();
        }
      }
      repaint();
      return true;
    }
    isMouseLeftDown = false;
    repaint();
    return false;
  }

  bool onMotion(const MotionEvent &ev) override
  {
    if (isMouseLeftDown) {
      auto sensi = (ev.mod & kModifierShift) ? lowSensitivity : sensitivity;
      value += (float)((anchorPoint.getY() - ev.pos.getY()) * sensi);
      value = value > 1.0 ? 1.0 : value < 0.0 ? 0.0 : value;
      updateValue();

      anchorPoint = ev.pos;

      isMouseEntered = true;
      repaint();
      return true;
    }
    isMouseEntered = contains(ev.pos);
    repaint();
    return false;
  }

  bool onScroll(const ScrollEvent &ev) override
  {
    if (!contains(ev.pos)) return false;
    auto sensi = (ev.mod & kModifierShift) ? lowSensitivity : 8.0f * sensitivity;
    value += ev.delta.getY() * sensi;
    value = value > 1.0 ? 1.0 : value < 0.0 ? 0.0 : value;
    updateValue();
    repaint();
    return true;
  }

  void setDefaultValue(double value)
  {
    defaultValue = value < 0.0 ? 0.0 : value > 1.0 ? 1.0 : value;
  }
  void setValue(double value) override
  {
    this->value = value < 0.0 ? 0.0 : value > 1.0 ? 1.0 : value;
  }
  void setHighlightColor(Color color) { highlightColor = color; }
  void setArcColor(Color color) { arcColor = color; }
  void setTipColor(Color color) { tipColor = color; }
  void setArcWidth(float width)
  {
    if (width > 0.0f) halfArcWidth = width / 2.0f;
  }

protected:
  double defaultValue = 0.5;

  Color highlightColor{0x33, 0xaa, 0xff};
  Color arcColor{0xdd, 0xdd, 0xdd};
  Color tipColor{0, 0, 0};

  float halfArcWidth = 4.0f;
  const float arcNotchHalf = float(SomeDSP::pi) / 6.0f; // Radian.
  float defaultTickLength = 0.5f;

  const float sensitivity = 0.004f; // MovedPixel * sensitivity = valueChanged.
  const float lowSensitivity = sensitivity / 5.0f;

  Point<int> anchorPoint{0, 0};
  bool isMouseLeftDown = false;
  bool isMouseEntered = false;
};
