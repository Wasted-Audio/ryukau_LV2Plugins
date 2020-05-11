// (c) 2019-2020 Takamitsu Endo
//
// This file is part of Uhhyou Plugins.
//
// Uhhyou Plugins is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Uhhyou Plugins is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Uhhyou Plugins.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "valuewidget.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

template<typename Scale> class BarBox : public ArrayWidget {
public:
  float sliderZero = 0.0f;
  int32_t indexOffset = 0;
  bool liveUpdateLineEdit = true; // Set this false when line edit is slow.

  explicit BarBox(
    NanoWidget *group,
    PluginUI *ui,
    std::vector<uint32_t> id,
    Scale &scale,
    std::vector<double> value,
    std::vector<double> defaultValue,
    FontId fontId)
    : ArrayWidget(group, ui, id, value)
    , defaultValue(defaultValue)
    , fontId(fontId)
    , scale(scale)
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

    float sliderZeroHeight = height * (1.0f - sliderZero);
    for (size_t i = 0; i < value.size(); ++i) {
      float rectH = value[i] >= sliderZero ? (value[i] - sliderZero) * height
                                           : (sliderZero - value[i]) * height;
      float rectY = value[i] >= sliderZero ? sliderZeroHeight - rectH : sliderZeroHeight;
      beginPath();
      rect(i * sliderWidth, rectY, sliderWidth - defaultBorderWidth, rectH);
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

    // Index text.
    if (sliderWidth >= 4.0f) {
      fillColor(borderColor);
      fontFaceId(fontId);
      fontSize(textSize);
      textAlign(ALIGN_CENTER | ALIGN_MIDDLE);
      for (size_t i = 0; i < value.size(); ++i)
        text(
          (i + 0.5f) * sliderWidth, height - 4, std::to_string(i + indexOffset).c_str(),
          nullptr);
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

        // Index text.
        fillColor(Color(0, 0, 0, 0x88));
        fontFaceId(fontId);
        fontSize(textSize * 4.0f);
        textAlign(ALIGN_CENTER | ALIGN_MIDDLE);
        std::ostringstream os;
        os << "#" << std::to_string(int32_t(index) + indexOffset) << ": "
           << std::to_string(scale.map(value[index]));
        std::string indexText(os.str());
        text(width / 2, height / 2, indexText.c_str(), nullptr);
      }
    }
  }

  bool onKeyboard(const KeyboardEvent &ev)
  {
    if (!isMouseEntered || !ev.press) return false;
    handleKey(ev);
    return true;
  }

  void handleKey(const KeyboardEvent &ev)
  {
    size_t index = calcIndex(mousePosition);
    if (ev.key == 'd') { // reset to Default.
      value = defaultValue;
      updateValue();
    } else if (ev.key == 'D') { // Alternative default. (toggle min/mid/max)
      std::fill(
        value.begin() + index, value.end(),
        value[index] == 0 ? 0.5 : value[index] == 0.5 ? 1.0 : 0.0);
      updateValue();
    } else if (ev.key == 'e') {
      emphasizeLow(index);
    } else if (ev.key == 'E') {
      emphasizeHigh(index);
    } else if (ev.key == 'f') {
      averageLowpass(index);
    } else if (ev.key == 'F') {
      highpass(index);
    } else if (ev.key == 'i') {
      invert(index, true);
    } else if (ev.key == 'I') {
      invert(index, false);
    } else if (ev.key == 'n') {
      normalize(index, true);
    } else if (ev.key == 'N') {
      normalize(index, false);
    } else if (ev.key == 'p') { // Permute.
      permute(index);
    } else if (ev.key == 'r') {
      randomize(index, 1.0);
    } else if (ev.key == 'R') {
      sparseRandomize(index);
    } else if (ev.key == 's') { // Sort descending order.
      std::sort(value.begin() + index, value.end(), std::greater<>());
      updateValue();
    } else if (ev.key == 'S') { // Sort ascending order.
      std::sort(value.begin() + index, value.end());
      updateValue();
    } else if (ev.key == 't') { // subTle randomize.
      randomize(index, 0.02);
    } else if (ev.key == ',') { // Rotate back.
      if (index == value.size() - 1) index = 0;
      std::rotate(value.begin() + index, value.begin() + index + 1, value.end());
      updateValue();
    } else if (ev.key == '.') { // Rotate forward.
      size_t rIndex = index == 0 ? 0 : value.size() - 1 - index;
      std::rotate(value.rbegin() + rIndex, value.rbegin() + rIndex + 1, value.rend());
      updateValue();
    } else if (ev.key == '1') { // Decrease odd.
      multiplySkip(index, 1);
    } else if (ev.key == '2') { // Decrease even.
      multiplySkip(index, 2);
    } else if (ev.key == '3') { // Decrease 3n.
      multiplySkip(index, 3);
    } else if (ev.key == '4') { // Decrease 4n.
      multiplySkip(index, 4);
    } else if (ev.key == '5') { // Decrease 5n.
      multiplySkip(index, 5);
    } else if (ev.key == '6') { // Decrease 6n.
      multiplySkip(index, 6);
    } else if (ev.key == '7') { // Decrease 7n.
      multiplySkip(index, 7);
    } else if (ev.key == '8') { // Decrease 8n.
      multiplySkip(index, 8);
    } else if (ev.key == '9') { // Decrease 9n.
      multiplySkip(index, 9);
    } else {
      return;
    }
    updateValue();
    repaint();
  }

  void averageLowpass(size_t start)
  {
    const int32_t range = 1;

    std::vector<double> result(value);
    for (size_t i = start; i < value.size(); ++i) {
      result[i] = 0.0;
      for (int32_t j = -range; j <= range; ++j) {
        size_t index = i + j; // Note that index is unsigned.
        if (index >= value.size()) continue;
        result[i] += value[index];
      }
      setValueAt(i, result[i] / double(2 * range + 1));
    }
  }

  void highpass(size_t start)
  {
    // value[i] = sum((-0.5, 1.0, -0.5) * value[(i - 1, i, i + 1)])
    // Value of index outside of array is assumed to be same as closest element.
    std::vector<double> result(value);
    size_t last = value.size() - 1;
    for (size_t i = start; i < value.size(); ++i) {
      result[i] = 0.0;
      result[i] -= (i >= 1) ? value[i - 1] : value[i];
      result[i] -= (i < last) ? value[i + 1] : value[i];
      result[i] = value[i] + 0.5f * result[i];
      setValueAt(i, result[i]);
    }
  }

  void randomize(size_t start, double mix)
  {
    std::random_device dev;
    std::mt19937_64 rng(dev());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (size_t i = start; i < value.size(); ++i)
      value[i] += mix * (dist(rng) - value[i]);
  }

  void sparseRandomize(size_t start)
  {
    std::random_device device;
    std::mt19937_64 rng(device());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (size_t i = start; i < value.size(); ++i)
      if (dist(rng) < 0.1f) value[i] = dist(rng);
  }

  void permute(size_t start)
  {
    std::random_device device;
    std::mt19937 rng(device());
    std::shuffle(value.begin() + start, value.end(), rng);
  }

  void invert(size_t start, bool preserveMin)
  {
    auto min = preserveMin ? *(std::min_element(value.begin(), value.end())) : 0.0;
    for (size_t i = start; i < value.size(); ++i) setValueAt(i, 1.0f - value[i] + min);
  }

  void normalize(size_t start, bool preserveMin) noexcept
  {
    auto min = preserveMin ? 0.0 : *(std::min_element(value.begin(), value.end()));
    auto mul = 1.0 / (*(std::max_element(value.begin(), value.end())) - min);
    if (!std::isfinite(mul)) return;
    for (size_t i = start; i < value.size(); ++i) setValueAt(i, (value[i] - min) * mul);
  }

  void multiplySkip(size_t start, size_t interval) noexcept
  {
    for (size_t i = start; i < value.size(); i += interval) value[i] *= 0.9;
  }

  void emphasizeLow(size_t start)
  {
    for (size_t i = start; i < value.size(); ++i)
      setValueAt(i, value[i] / pow(i + 1, 0.0625));
  }

  void emphasizeHigh(size_t start)
  {
    for (size_t i = start; i < value.size(); ++i)
      setValueAt(i, value[i] * (0.9 + 0.1 * double(i + 1) / value.size()));
  }

  bool onMouse(const MouseEvent &ev) override
  {
    // Note: Specific to this plugin.
    if (!ev.press && ev.button == 3) {
      updateValue();
    }

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
      if (liveUpdateLineEdit) updateValue();
      return true;
    }
    return false;
  }

  bool onScroll(const ScrollEvent &ev) override
  {
    if (!contains(ev.pos)) return false;

    size_t index = calcIndex(ev.pos);
    if (index >= value.size()) return false;

    if (ev.mod & kModifierShift)
      setValueAt(index, value[index] + 0.003 * ev.delta.getY());
    else
      setValueAt(index, value[index] + 0.01 * ev.delta.getY());

    updateValueAt(index);
    repaint();
    return true;
  }

  void onResize(const ResizeEvent &ev)
  {
    sliderWidth = float(ev.size.getWidth()) / value.size();
    defaultBorderWidth = sliderWidth < 4.0f ? 1.0f : 2.0f;
  }

  void setHighlightColor(Color color) { highlightColor = color; }
  void setValueColor(Color color) { valueColor = color; }
  void setBorderColor(Color color) { borderColor = color; }
  void setDefaultBorderWidth(float width) { defaultBorderWidth = width; }
  void setHighlightBorderWidth(float width) { highlightBorderWidth = width; }

private:
  inline size_t calcIndex(Point<int> position)
  {
    return size_t(position.getX() / sliderWidth);
  }

  void setValueFromPosition(Point<int> position, uint modifier)
  {
    size_t index = calcIndex(position);
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

    // updateValue(); // Disable frequent update for this plugin.
    repaint();
  }

  std::vector<double> defaultValue;

  Color highlightColor{0x00, 0xff, 0x00, 0x33};
  Color splitterColor{0xff, 0xff, 0xff};
  Color valueColor{0xdd, 0xdd, 0xdd};
  Color borderColor{0, 0, 0};
  Color centerLineColor{0, 0, 0, 0x88};

  float defaultBorderWidth = 2.0f;
  float highlightBorderWidth = 4.0f;

  float textSize = 9.0f;
  FontId fontId = -1;

  Scale &scale;

  Point<int> anchor{0, 0};
  bool isMouseLeftDown = false;
  bool isMouseRightDown = false;
  bool isMouseEntered = false;

  Point<int> mousePosition{-1, -1};
  float sliderWidth = 0;
};
