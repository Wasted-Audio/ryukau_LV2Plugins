// (c) 2019-2020 Takamitsu Endo
//
// This file is part of EsPhaser.
//
// EsPhaser is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// EsPhaser is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with EsPhaser.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cmath>

#include "valuewidget.hpp"

class VSlider : public ValueWidget {
public:
  explicit VSlider(NanoWidget *group, PluginUI *ui) : ValueWidget(group, ui, 0.1f) {}

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();

    // Value.
    beginPath();
    rect(0, (1.0f - value) * height, width, value * height);
    fillColor(valueColor);
    fill();

    // Border.
    beginPath();
    rect(0, 0, width, height);
    strokeWidth(isMouseEntered ? highlightBorderWidth : defaultBorderWidth);
    strokeColor(isMouseEntered ? highlightColor : borderColor);
    stroke();
  }

  bool onMouse(const MouseEvent &ev) override
  {
    if (ev.press && contains(ev.pos)) {
      if (ev.button == 1) {
        value = float(getHeight() - ev.pos.getY()) / getHeight();
        value = value > 1.0 ? 1.0 : value < 0.0 ? 0.0 : value;
        updateValue();
        anchorPoint = ev.pos;
        isMouseLeftDown = true;
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
      if (ev.mod & kModifierShift)
        value += double(anchorPoint.getY() - ev.pos.getY()) / 16.0 / getHeight();
      else
        value = double(int(getHeight()) - ev.pos.getY()) / getHeight();
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
  void setValueColor(Color color) { valueColor = color; }
  void setBorderColor(Color color) { borderColor = color; }
  void setDefaultBorderWidth(float width) { defaultBorderWidth = width; }
  void setHighlightBorderWidth(float width) { highlightBorderWidth = width; }

private:
  double defaultValue = 0.5;

  Color highlightColor{0x33, 0xaa, 0xff};
  Color valueColor{0xdd, 0xdd, 0xdd};
  Color borderColor{0, 0, 0};

  float defaultBorderWidth = 2.0f;
  float highlightBorderWidth = 4.0f;

  const float sensitivity = 0.004f; // MovedPixel * sensitivity = valueChanged.
  const float lowSensitivity = sensitivity / 5.0f;

  Point<int> anchorPoint{0, 0};
  bool isMouseLeftDown = false;
  bool isMouseEntered = false;
};
