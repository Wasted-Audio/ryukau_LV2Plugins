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
    if (ev.key == 'a') { // Sort ascending order.
      std::sort(value.begin(), value.end());
      updateValue();
    } else if (ev.key == 'd') { // Sort decending order.
      std::sort(value.begin(), value.end(), std::greater<>());
      updateValue();
    } else if (ev.key == 'f') {
      averageLowpass();
    } else if (ev.key == 'F') {
      highpass();
    } else if (ev.key == 'h') {
      emphasizeHigh();
    } else if (ev.key == 'i') {
      invert(true);
    } else if (ev.key == 'I') {
      invert(false);
    } else if (ev.key == 'l') {
      emphasizeLow();
    } else if (ev.key == 'n') {
      normalize(true);
    } else if (ev.key == 'N') {
      normalize(false);
    } else if (ev.key == 'p') { // Permute.
      permute();
    } else if (ev.key == 'r') {
      randomize(1.0);
    } else if (ev.key == 'R') {
      sparseRandomize();
    } else if (ev.key == 's') { // Subtle randomize.
      randomize(0.02);
    } else if (ev.key == ',') { // Rotate back.
      std::rotate(value.begin(), value.begin() + 1, value.end());
      updateValue();
    } else if (ev.key == '.') { // Rotate forward.
      std::rotate(value.rbegin(), value.rbegin() + 1, value.rend());
      updateValue();
    } else if (ev.key == '1') { // Mute odd.
      multiplySkip(0, 2);
    } else if (ev.key == '2') { // Mute even.
      multiplySkip(1, 2);
    } else if (ev.key == '3') { // Mute 3n.
      multiplySkip(2, 3);
    } else if (ev.key == '4') { // Mute 4n.
      multiplySkip(3, 4);
    } else if (ev.key == '5') { // Mute 5n.
      multiplySkip(4, 5);
    } else if (ev.key == '6') { // Mute 6n.
      multiplySkip(5, 6);
    } else if (ev.key == '7') { // Mute 7n.
      multiplySkip(6, 7);
    } else if (ev.key == '8') { // Mute 8n.
      multiplySkip(7, 8);
    } else if (ev.key == '9') { // Mute 9n.
      multiplySkip(8, 9);
    } else {
      return;
    }
    updateValue();
    repaint();
  }

  void averageLowpass()
  {
    const int32_t range = 1;

    std::vector<double> result(value);
    for (size_t i = 0; i < value.size(); ++i) {
      result[i] = 0.0;
      for (int32_t j = -range; j <= range; ++j) {
        size_t index = i + j; // Note that index is unsigned.
        if (index >= value.size()) continue;
        result[i] += value[index];
      }
      setValueAt(i, result[i] / double(2 * range + 1));
    }
  }

  void highpass()
  {
    // value[i] = sum((-0.5, 1.0, -0.5) * value[(i - 1, i, i + 1)])
    // Value of index outside of array is assumed to be same as closest element.
    std::vector<double> result(value);
    size_t last = value.size() - 1;
    for (size_t i = 0; i < value.size(); ++i) {
      result[i] = 0.0;
      result[i] -= (i >= 1) ? value[i - 1] : value[i];
      result[i] -= (i < last) ? value[i + 1] : value[i];
      result[i] = value[i] + 0.5f * result[i];
      setValueAt(i, result[i]);
    }
  }

  void randomize(double mix)
  {
    std::random_device dev;
    std::mt19937_64 rng(dev());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (auto &val : value) val += mix * (dist(rng) - val);
  }

  void sparseRandomize()
  {
    std::random_device device;
    std::mt19937_64 rng(device());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (auto &val : value) {
      if (dist(rng) < 0.1f) val = dist(rng);
    }
  }

  void permute()
  {
    std::random_device device;
    std::mt19937 rng(device());
    std::shuffle(value.begin(), value.end(), rng);
  }

  void invert(bool preserveMin)
  {
    auto min = preserveMin ? *(std::min_element(value.begin(), value.end())) : 0.0;
    for (size_t i = 0; i < value.size(); ++i) setValueAt(i, 1.0f - value[i] + min);
  }

  void normalize(bool preserveMin) noexcept
  {
    auto min = preserveMin ? 0.0 : *(std::min_element(value.begin(), value.end()));
    auto mul = 1.0 / (*(std::max_element(value.begin(), value.end())) - min);
    if (!std::isfinite(mul)) return;
    for (size_t i = 0; i < value.size(); ++i) setValueAt(i, (value[i] - min) * mul);
  }

  void multiplySkip(size_t offset, size_t interval) noexcept
  {
    for (size_t i = offset; i < value.size(); i += interval) value[i] *= 0.9;
  }

  void emphasizeLow()
  {
    for (size_t i = 0; i < value.size(); ++i)
      setValueAt(i, value[i] / pow(i + 1, 0.0625));
  }

  void emphasizeHigh()
  {
    for (size_t i = 0; i < value.size(); ++i)
      setValueAt(i, value[i] * (0.9 + 0.1 * double(i + 1) / value.size()));
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
