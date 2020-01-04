// (c) 2019-2020 Takamitsu Endo
//
// This file is part of IterativeSinOvertone.
//
// IterativeSinOvertone is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// IterativeSinOvertone is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with IterativeSinOvertone.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "valuewidget.hpp"

#include <algorithm>
#include <cmath>

#include <iostream>

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

    // Index text.
    fillColor(borderColor);
    fontFaceId(fontId);
    fontSize(textSize);
    textAlign(ALIGN_CENTER | ALIGN_MIDDLE);
    for (size_t i = 0; i < value.size(); ++i)
      text((i + 0.5f) * sliderWidth, height - 4, std::to_string(i + 1).c_str(), nullptr);
  }

  bool onKeyboard(const KeyboardEvent &ev)
  {
    if (!isMouseEntered || !ev.press) return false;
    handleKey(ev);
    return true;
  }

  void handleKey(const KeyboardEvent &ev)
  {
    /*
    IDEA:
    - n time permutation. n may be small (10~20 ?).
    - zero/max.
    - add curve.
    - saturate/excite with pow().
    - multiply then modulo. modf(const * value[i], 1.0f);
    - sparse randomize.

    Make number key to set anchor point.
     */
    if (ev.key == 'r') {
      randomize();
    } else if (ev.key == 's') { // Sort decending order.
      std::sort(value.begin(), value.end(), std::greater<>());
      updateValue();
    } else if (ev.key == 'a') { // Sort ascending order.
      std::sort(value.begin(), value.end());
      updateValue();
    } else if (ev.key == 'e') { // Mute even.
      multiplySkip(0, 2);
    } else if (ev.key == 'o') { // Mute odd.
      multiplySkip(1, 2);
    } else if (ev.key == 'n') { // Normalize.
      normalize();
    } else if (ev.key == '1') {
      multiplyLinearDecendCurve();
    } else if (ev.key == '2') {
      multiplyLinearAscendCurve();
    } else if (ev.key == '3') {
      multiplyHigh();
    } else if (ev.key == '4') {
      multiplyLow();
    } else {
      return;
    }
    repaint();
  }

  void normalize() noexcept
  {
    auto mul = 1.0 / *(std::max_element(value.begin(), value.end()));
    if (!std::isfinite(mul)) return;
    for (size_t i = 0; i < value.size(); ++i) setValueAt(i, value[i] * mul);
    updateValue();
  }

  void multiplySkip(size_t offset, size_t interval) noexcept
  {
    for (size_t i = offset; i < value.size(); i += interval) value[i] *= 0.9;
    updateValue();
  }

  void multiplyLinearDecendCurve()
  {
    double denom = value.size() - 1;
    for (size_t i = 0; i < value.size(); ++i) {
      value[i] *= 0.5 * (1.0 + (denom - i) / denom);
    }
    updateValue();
  }

  void multiplyLinearAscendCurve()
  {
    double denom = value.size() - 1;
    for (size_t i = 0; i < value.size(); ++i) {
      value[i] *= 0.5 * (1.0 + i / denom);
    }
    updateValue();
  }

  void multiplyHigh()
  {
    size_t start = value.size() / 2;
    double denom = value.size() - start;
    for (size_t i = start; i < value.size(); ++i) {
      value[i] *= 0.5 * (1.0 + (denom - i) / denom);
    }
    updateValue();
  }

  void multiplyLow()
  {
    size_t end = value.size() / 2;
    double denom = end;
    for (size_t i = 0; i < end; ++i) {
      value[i] *= 0.5 * (1.0 + i / denom);
    }
    updateValue();
  }

  bool onMouse(const MouseEvent &ev) override
  {
    if (!(ev.press && contains(ev.pos))) {
      isMouseLeftDown = false;
      isMouseRightDown = false;
      return false;
    }

    if (ev.button == 1) {
      isMouseLeftDown = true;
    } else if (ev.button == 3) {
      isMouseRightDown = true;
      anchor = ev.pos;
    }

    setValueFromPosition(ev.pos, ev.mod);
    return true;
  }

  bool onMotion(const MotionEvent &ev) override
  {
    isMouseEntered = contains(ev.pos);
    mousePosition = ev.pos;
    if (isMouseLeftDown) {
      setValueFromPosition(ev.pos, ev.mod);
      return true;
    } else if (isMouseRightDown) {
      setValueFromLine(anchor, ev.pos);
      return true;
    }
    return false;
  }

  bool onScroll(const ScrollEvent &ev) override
  {
    if (!contains(ev.pos)) return false;

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
    if (modifier & kModifierControl)
      setValueAt(index, defaultValue[index]);
    else
      setValueAt(index, 1.0 - double(position.getY()) / getHeight());
    updateValueAt(index);
    repaint();
  }

  void setValueFromLine(Point<int> p0, Point<int> p1)
  {
    if (p0.getX() > p1.getX()) std::swap(p0, p1);

    size_t left = size_t(p0.getX() / sliderWidth);
    size_t right = size_t(p1.getX() / sliderWidth);
    if (left >= value.size() || right >= value.size()) return;

    const float p0y = p0.getY();
    const float p1y = p1.getY();

    if (left == right) {
      // p0 and p1 are in a same bar.
      setValueAt(left, 1.0f - (p0y + p1y) * 0.5f / getHeight());
      updateValueAt(left);
      repaint();
      return;
    }

    const float xL = sliderWidth * (left + 1);
    const float xR = sliderWidth * right;

    if (fabs(xR - xL) >= 1e-5) {
      p0.setX(xL);
      p1.setX(xR);
    }

    setValueAt(left, 1.0f - p0y / getHeight());
    setValueAt(right, 1.0f - p1y / getHeight());

    // In between.
    const float p0x = p0.getX();
    const float p1x = p1.getX();
    const float slope = (p1y - p0y) / (p1x - p0x);
    const float yInc = slope * sliderWidth;
    float y = slope * (sliderWidth * (left + 1) - p0x) + p0y;
    for (size_t idx = left + 1; idx < right; ++idx) {
      setValueAt(idx, 1.0f - (y + 0.5f * yInc) / getHeight());
      y += yInc;
    }

    updateValue();
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

  Point<int> anchor{0, 0};
  bool isMouseLeftDown = false;
  bool isMouseRightDown = false;
  bool isMouseEntered = false;

  Point<int> mousePosition{-1, -1};
  float sliderWidth = 0;
};
