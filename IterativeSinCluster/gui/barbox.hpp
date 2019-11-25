// (c) 2019 Takamitsu Endo
//
// This file is part of IterativeSinCluster.
//
// IterativeSinCluster is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// IterativeSinCluster is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with IterativeSinCluster.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cmath>

#include "valuewidget.hpp"

class BarBox : public ArrayWidget {
public:
  explicit BarBox(
    NanoWidget *group,
    PluginUI *ui,
    std::vector<uint32_t> id,
    std::vector<double> value,
    std::vector<double> defaultValue,
    FontId fontId)
    : ArrayWidget(group, ui, id, value), defaultValue(defaultValue), fontId(fontId)
  {
  }

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();

    // Value bar.
    fillColor(valueColor);
    for (size_t i = 0; i < value.size(); ++i) {
      auto sliderHeight = value[i] * height;
      beginPath();
      rect(
        roundf(i * sliderWidth), roundf(height - sliderHeight), roundf(sliderWidth),
        roundf(sliderHeight));
      fill();
    }

    // Splitter.
    strokeWidth(defaultBorderWidth);
    strokeColor(splitterColor);
    for (size_t i = 0; i < value.size(); ++i) {
      auto x = i * sliderWidth;
      beginPath();
      moveTo(x, 0);
      lineTo(x, height);
      stroke();
    }

    // Border.
    beginPath();
    rect(0, 0, width, height);
    strokeWidth(defaultBorderWidth);
    strokeColor(borderColor);
    stroke();

    // Highlight.
    if (uint(mousePosition.getY()) <= height && uint(mousePosition.getX()) <= width) {
      size_t index = size_t(value.size() * mousePosition.getX() / width);
      if (index < value.size()) {
        beginPath();
        rect(index * sliderWidth, 0, sliderWidth, height);
        fillColor(highlightColor);
        fill();
      }
    }

    // Text.
    fillColor(borderColor);
    fontFaceId(fontId);
    fontSize(textSize);
    textAlign(ALIGN_CENTER | ALIGN_MIDDLE);
    for (size_t i = 0; i < value.size(); ++i)
      text((i + 0.5f) * sliderWidth, height - 4, std::to_string(i + 1).c_str(), nullptr);
  }

  bool onMouse(const MouseEvent &ev) override
  {
    if (!(ev.press && contains(ev.pos))) {
      isMouseLeftDown = false;
      isMouseRightDown = false;
      return false;
    }

    if (ev.button == 1)
      isMouseLeftDown = true;
    else if (ev.button == 3)
      isMouseRightDown = true;

    setValueFromPosition(ev.pos, ev.mod);
    return true;
  }

  bool onMotion(const MotionEvent &ev) override
  {
    mousePosition = ev.pos;
    if (!isMouseLeftDown && !isMouseRightDown) return false;
    setValueFromPosition(ev.pos, ev.mod);
    return true;
  }

  bool onScroll(const ScrollEvent &ev) override
  {
    size_t index = size_t(ev.pos.getX() / sliderWidth);
    if (index >= value.size()) return false;

    if (ev.mod & kModifierShift)
      setValueAt(index, value[index] + 0.003 * ev.delta.getY());
    else
      setValueAt(index, value[index] + 0.02 * ev.delta.getY());

    updateValueAt(index);
    repaint();
    return true;
  }

  void onResize(const ResizeEvent &ev)
  {
    sliderWidth = float(ev.size.getWidth()) / value.size();
  }

  void setHighlightColor(Color color) { highlightColor = color; }
  void setValueColor(Color color) { valueColor = color; }
  void setBorderColor(Color color) { borderColor = color; }
  void setDefaultBorderWidth(float width) { defaultBorderWidth = width; }
  void setHighlightBorderWidth(float width) { highlightBorderWidth = width; }

private:
  void setValueFromPosition(Point<int> position, uint modifier)
  {
    size_t index = size_t(position.getX() / sliderWidth);
    if (index >= value.size()) return;
    if (isMouseLeftDown) {
      if (modifier & kModifierControl)
        setValueAt(index, defaultValue[index]);
      else
        setValueAt(index, 1.0 - double(position.getY()) / getHeight());
    } else if (isMouseRightDown) {
      if (modifier & kModifierControl)
        setValueAt(index, 1);
      else
        setValueAt(index, 0);
    }
    updateValueAt(index);
    repaint();
  }

  std::vector<double> defaultValue;

  Color highlightColor{0x00, 0xff, 0x00, 0x33};
  Color splitterColor{0xff, 0xff, 0xff};
  Color valueColor{0xdd, 0xdd, 0xdd};
  Color borderColor{0, 0, 0};

  float defaultBorderWidth = 2.0f;
  float highlightBorderWidth = 4.0f;

  float textSize = 9.0f;
  FontId fontId = -1;

  Point<int> anchorPoint{0, 0};
  bool isMouseLeftDown = false;
  bool isMouseRightDown = false;
  bool isMouseEntered = false;

  Point<int> mousePosition{-1, -1};
  float sliderWidth = 0;
};
