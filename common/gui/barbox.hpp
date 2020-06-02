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

#include "style.hpp"
#include "valuewidget.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <sstream>
#include <string>

template<typename Scrollable> class ScrollBar : public NanoWidget {
public:
  explicit ScrollBar(
    NanoWidget *group, std::shared_ptr<Scrollable> parent, Palette &palette)
    : NanoWidget(group), parent(parent), pal(palette)
  {
  }

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();

    strokeWidth(2.0f);
    strokeColor(pal.border());

    // Bar.
    auto rightHandleL = rightPos * width - handleWidth;
    auto barL = leftPos * width + handleWidth;
    beginPath();
    rect(barL, 0, rightHandleL - barL, height);
    fillColor(pointed == Part::bar ? pal.highlightButton() : pal.highlightMain());
    fill();
    stroke();

    // Left handle.
    beginPath();
    rect(leftPos * width, 0, handleWidth, height);
    fillColor(pointed == Part::leftHandle ? pal.highlightButton() : pal.unfocused());
    fill();
    stroke();

    // Right handle.
    beginPath();
    rect(rightHandleL, 0, handleWidth, height);
    fillColor(pointed == Part::rightHandle ? pal.highlightButton() : pal.unfocused());
    fill();
    stroke();
  }

  bool onMouse(const MouseEvent &ev) override
  {
    if (contains(ev.pos) && ev.press) {
      grabbed = pointed = hitTest(ev.pos);

      if (grabbed == Part::leftHandle)
        grabOffset = int(leftPos * getWidth()) - ev.pos.getX();
      else if (grabbed == Part::rightHandle)
        grabOffset = int(rightPos * getWidth()) - ev.pos.getX();
      else if (grabbed == Part::bar)
        grabOffset = int(leftPos * getWidth()) - ev.pos.getX();

      return true;
    }

    grabbed = Part::background;
    return false;
  }

  bool onMotion(const MotionEvent &ev) override
  {
    auto posX
      = std::clamp<int>(ev.pos.getX() + grabOffset, 0, getWidth()) / float(getWidth());
    auto handleW = handleWidth / getWidth();
    switch (grabbed) {
      case Part::bar: {
        auto barWidth = rightPos - leftPos;
        leftPos = posX;
        rightPos = leftPos + barWidth;
        if (leftPos < 0.0f) {
          rightPos = barWidth;
          leftPos = 0.0f;
        } else if (rightPos > 1.0f) {
          leftPos = 1.0f - barWidth;
          rightPos = 1.0f;
        }

        // Just in case.
        leftPos = std::clamp(leftPos, 0.0f, 1.0f);
        rightPos = std::clamp(rightPos, 0.0f, 1.0f);
      } break;

      case Part::leftHandle: {
        auto rightMost = std::max(rightPos - 3 * handleW, 0.0f);
        leftPos = std::clamp(posX, 0.0f, rightMost);
      } break;

      case Part::rightHandle: {
        auto leftMost = std::min(leftPos + 3 * handleW, 1.0f);
        rightPos = std::clamp(posX, leftMost, 1.0f);
      } break;

      default:
        pointed = hitTest(ev.pos);
        repaint();
        return false;
    }

    parent->setViewRange(leftPos, rightPos);

    repaint();
    return true;
  }

  void setHandleWidth(float width) { handleWidth = std::max(width, 0.0f); }

protected:
  enum class Part : uint8_t { background = 0, bar, leftHandle, rightHandle };

  inline Part hitTest(Point<int> pt)
  {
    if (pt.getY() < 0 || pt.getY() > int(getHeight())) return Part::background;

    auto left = leftPos * getWidth();
    auto right = rightPos * getWidth();
    auto width = handleWidth;
    auto pX = pt.getX();

    auto leftHandleR = left + width;
    if (pX >= left && pX <= leftHandleR) return Part::leftHandle;

    auto rightHandleL = right - width;
    if (pX >= rightHandleL && pX <= right) return Part::rightHandle;

    if (pX > leftHandleR && pX < rightHandleL) return Part::bar;

    return Part::background;
  }

  float handleWidth = 10;
  float leftPos = 0;
  float rightPos = 1;
  int grabOffset = 0;

  Part pointed = Part::background;
  Part grabbed = Part::background;

  std::shared_ptr<Scrollable> parent;
  Palette &pal;
};

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
    FontId fontId,
    Palette &palette)
    : ArrayWidget(group, ui, id, value)
    , defaultValue(defaultValue)
    , fontId(fontId)
    , pal(palette)
    , scale(scale)
    , indexR(value.size())
    , indexRange(value.size() - indexL)
  {
  }

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();

    // Background.
    beginPath();
    rect(0, 0, width, height);
    fillColor(pal.boxBackground());
    fill();

    // Value bar.
    fillColor(pal.highlightMain());

    float sliderZeroHeight = height * (1.0f - sliderZero);
    for (int i = indexL; i < indexR; ++i) {
      float rectH = value[i] >= sliderZero ? (value[i] - sliderZero) * height
                                           : (sliderZero - value[i]) * height;
      float rectY = value[i] >= sliderZero ? sliderZeroHeight - rectH : sliderZeroHeight;
      beginPath();
      rect((i - indexL) * sliderWidth, rectY, sliderWidth - barWidth, rectH);
      fill();
    }

    // Index text.
    fontFaceId(fontId);
    if (sliderWidth >= 8.0f) {
      fillColor(pal.foreground());
      fontSize(textSize);
      textAlign(ALIGN_CENTER | ALIGN_MIDDLE);
      for (int i = 0; i < indexRange; ++i)
        text(
          (i + 0.5f) * sliderWidth, height - 4,
          std::to_string(i + indexL + indexOffset).c_str(), nullptr);
    } else if (value.size() != size_t(indexRange)) {
      fillColor(pal.overlay());
      fontSize(textSize * 2.0f);
      textAlign(ALIGN_LEFT | ALIGN_TOP);
      std::string str = "<- #" + std::to_string(indexL);
      text(0, 0, str.c_str(), nullptr);
    }

    // Border.
    beginPath();
    rect(0, 0, width, height);
    strokeWidth(borderWidth);
    strokeColor(pal.border());
    stroke();

    // Highlight.
    if (uint(mousePosition.getY()) <= height && uint(mousePosition.getX()) <= width) {
      int index = int(indexL + indexRange * mousePosition.getX() / width);
      if (indexL <= index && index < indexR) {
        beginPath();
        rect((index - indexL) * sliderWidth, 0, sliderWidth, height);
        fillColor(pal.overlayHighlight());
        fill();

        // Index text.
        fillColor(pal.overlay());
        fontFaceId(fontId);
        fontSize(textSize * 4.0f);
        textAlign(ALIGN_CENTER | ALIGN_MIDDLE);
        std::ostringstream os;
        os << "#" << std::to_string(index + indexOffset) << ": "
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
    if (ev.key == 'a') {
      alternateSign(index);
    } else if (ev.key == 'd') { // reset to Default.
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
      totalRandomize(index);
    } else if (ev.key == 'R') {
      sparseRandomize(index);
    } else if (ev.key == 's') { // Sort descending order.
      std::sort(value.begin() + index, value.end(), std::greater<>());
      updateValue();
    } else if (ev.key == 'S') { // Sort ascending order.
      std::sort(value.begin() + index, value.end());
      updateValue();
    } else if (ev.key == 't') { // subTle randomize. Random walk.
      randomize(index, 0.02);
    } else if (ev.key == 'T') { // subTle randomize. Converge to sliderZero.
      mixRandomize(index, 0.02);
    } else if (ev.key == ',') { // Rotate back.
      if (index == value.size() - 1) index = 0;
      std::rotate(value.begin() + index, value.begin() + index + 1, value.end());
      updateValue();
    } else if (ev.key == '.') { // Rotate forward.
      size_t rIndex = index == 0 ? 0 : value.size() - 1 - index;
      std::rotate(value.rbegin() + rIndex, value.rbegin() + rIndex + 1, value.rend());
      updateValue();
    } else if (ev.key == '1') { // Decrease.
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

  void alternateSign(size_t start)
  {
    for (size_t i = start; i < value.size(); i += 2)
      setValueAt(i, 2 * sliderZero - value[i]);
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
        result[i] += value[index] - sliderZero;
      }
      setValueAt(i, sliderZero + result[i] / double(2 * range + 1));
    }
  }

  /**
  Highpass equation is:
    `value[i] = sum((-0.5, 1.0, -0.5) * value[(i - 1, i, i + 1)])`
  Value of index outside of array is assumed to be same as closest element.
  */
  void highpass(size_t start)
  {
    std::vector<double> result(value);
    size_t last = value.size() - 1;
    for (size_t i = start; i < value.size(); ++i) {
      auto val = value[i] - sliderZero;
      result[i] = 0.0;
      result[i] -= (i >= 1) ? value[i - 1] - sliderZero : val;
      result[i] -= (i < last) ? value[i + 1] - sliderZero : val;
      result[i] = val + 0.5f * result[i];
      setValueAt(i, sliderZero + result[i]);
    }
  }

  void totalRandomize(size_t start)
  {
    std::random_device dev;
    std::mt19937_64 rng(dev());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (size_t i = start; i < value.size(); ++i) value[i] = dist(rng);
  }

  void randomize(size_t start, double amount)
  {
    std::random_device dev;
    std::mt19937_64 rng(dev());
    amount /= 2;
    for (size_t i = start; i < value.size(); ++i) {
      std::uniform_real_distribution<double> dist(value[i] - amount, value[i] + amount);
      setValueAt(i, dist(rng));
    }
  }

  void mixRandomize(size_t start, double mix)
  {
    std::random_device dev;
    std::mt19937_64 rng(dev());
    std::uniform_real_distribution<double> dist(sliderZero - 0.5, sliderZero + 0.5);
    for (size_t i = start; i < value.size(); ++i)
      setValueAt(i, value[i] + mix * (dist(rng) - value[i]));
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
    if (!preserveMin) {
      for (size_t i = start; i < value.size(); ++i) {
        double val
          = value[i] >= sliderZero ? 1.0 - value[i] + sliderZero : sliderZero - value[i];
        setValueAt(i, val);
      }
    } else {
      auto [minIter, maxIter] = std::minmax_element(
        value.begin(), value.end(), [&](const double &lhs, const double &rhs) {
          return fabs(lhs - sliderZero) < fabs(rhs - sliderZero);
        });
      double min = fabs(*minIter);
      double max = fabs(*maxIter);
      for (size_t i = start; i < value.size(); ++i)
        setValueAt(i, copysign(max - fabs(value[i] - sliderZero) + min, value[i]));
    }
  }

  void normalize(size_t start, bool preserveMin) noexcept
  {
    double minNeg = 2;
    double minPos = 2;
    double maxNeg = -1;
    double maxPos = -1;
    for (size_t i = start; i < value.size(); ++i) {
      auto val = fabs(value[i] - sliderZero);
      if (value[i] < sliderZero) {
        if (val > maxNeg)
          maxNeg = val;
        else if (val < minNeg)
          minNeg = val;
      } else {
        if (val > maxPos)
          maxPos = val;
        else if (val < minPos)
          minPos = val;
      }
    }
    if (minNeg > 1.0) minNeg = 0.0;
    if (minPos > 1.0) minPos = 0.0;
    if (maxNeg < 0.0) maxNeg = 0.0;
    if (maxPos < 0.0) maxPos = 0.0;

    if (preserveMin) {
      auto mulNeg = (sliderZero - minNeg) / (maxNeg - minNeg);
      auto mulPos = (1.0 - sliderZero - minPos) / (maxPos - minPos);
      if (mulNeg > 1e15 || mulNeg < 0.0) mulNeg = minNeg = 0.0;
      if (mulPos > 1e15 || mulPos < 0.0) mulPos = minPos = 0.0;
      for (size_t i = start; i < value.size(); ++i) {
        auto val = value[i] < sliderZero
          ? (value[i] - sliderZero + minNeg) * mulNeg + sliderZero - minNeg
          : (value[i] - sliderZero - minPos) * mulPos + sliderZero + minPos;
        setValueAt(i, val);
      }
    } else {
      auto mulNeg = sliderZero / (maxNeg - minNeg);
      auto mulPos = (1.0 - sliderZero) / (maxPos - minPos);
      if (mulNeg > 1e15 || mulNeg < 0.0) mulNeg = minNeg = 0.0;
      if (mulPos > 1e15 || mulPos < 0.0) mulPos = minPos = 0.0;
      for (size_t i = start; i < value.size(); ++i) {
        auto val = value[i] < sliderZero
          ? (value[i] - sliderZero + minNeg) * mulNeg + sliderZero
          : (value[i] - sliderZero - minPos) * mulPos + sliderZero;
        setValueAt(i, val);
      }
      return;
    }
  }

  void multiplySkip(size_t start, size_t interval) noexcept
  {
    for (size_t i = start; i < value.size(); i += interval) {
      setValueAt(i, (value[i] - sliderZero) * 0.9 + sliderZero);
    }
  }

  void emphasizeLow(size_t start)
  {
    for (size_t i = start; i < value.size(); ++i)
      setValueAt(i, (value[i] - sliderZero) / pow(i + 1, 0.0625) + sliderZero);
  }

  void emphasizeHigh(size_t start)
  {
    for (size_t i = start; i < value.size(); ++i)
      setValueAt(
        i,
        (value[i] - sliderZero) * (0.9 + 0.1 * double(i + 1) / value.size())
          + sliderZero);
  }

  bool onMouse(const MouseEvent &ev) override
  {
    // Note: Specific to this plugin.
    if (!ev.press && (ev.button == 1 || ev.button == 3)) {
      updateValue();
    }

    if (!(ev.press && contains(ev.pos))) {
      isMouseLeftDown = false;
      isMouseRightDown = false;
      return false;
    }

    if (ev.button == 1)
      isMouseLeftDown = true;
    else if (ev.button == 3)
      isMouseRightDown = true;

    anchor = ev.pos;

    setValueFromPosition(ev.pos, ev.mod);
    return true;
  }

  bool onMotion(const MotionEvent &ev) override
  {
    isMouseEntered = contains(ev.pos);
    mousePosition = ev.pos;
    if (isMouseLeftDown) {
      if (ev.mod & kModifierShift)
        setValueFromPosition(ev.pos, ev.mod);
      else
        setValueFromLine(anchor, ev.pos, ev.mod);
      anchor = ev.pos;
      return true;
    } else if (isMouseRightDown) {
      setValueFromLine(anchor, ev.pos, ev.mod);
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

  void onResize(const ResizeEvent &ev) { refreshSliderWidth(ev.size.getWidth()); }

  void setBorderWidth(float width) { borderWidth = width; }
  void setHighlightBorderWidth(float width) { highlightBorderWidth = width; }

  void setViewRange(float left, float right)
  {
    indexL = int(std::clamp(left, 0.0f, 1.0f) * value.size());
    indexR = int(std::clamp(right, 0.0f, 1.0f) * value.size());
    indexRange = indexR >= indexL ? indexR - indexL : 0;
    refreshSliderWidth(getWidth());
    repaint();
  }

private:
  inline size_t calcIndex(Point<int> position)
  {
    return size_t(indexL + position.getX() / sliderWidth);
  }

  void refreshSliderWidth(float width)
  {
    sliderWidth = indexRange >= 1 ? float(width) / indexRange : float(width);
    barWidth = sliderWidth <= 4.0f ? 1.0f : 2.0f;
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

  void setValueFromLine(Point<int> p0, Point<int> p1, uint modifier)
  {
    if (p0.getX() > p1.getX()) std::swap(p0, p1);

    int last = int(value.size()) - 1;
    if (last < 0) last = 0; // std::clamp is undefined if low is greater than high.

    int left = int(calcIndex(p0));
    int right = int(calcIndex(p1));

    if ((left < 0 && right < 0) || (left > last && right > last)) return;

    left = std::clamp(left, 0, last);
    right = std::clamp(right, 0, last);

    const float p0y = p0.getY();
    const float p1y = p1.getY();

    if (left == right) { // p0 and p1 are in a same bar.
      if (modifier & kModifierControl)
        setValueAt(left, defaultValue[left]);
      else
        setValueAt(left, 1.0f - (p0y + p1y) * 0.5f / getHeight());
      updateValueAt(left);
      repaint();
      return;
    } else if (modifier & kModifierControl) {
      for (int idx = left; idx >= 0 && idx <= right; ++idx)
        setValueAt(idx, defaultValue[idx]);
      if (liveUpdateLineEdit) updateValue();
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
    for (int idx = left + 1; idx >= 0 && idx < right; ++idx) {
      setValueAt(idx, 1.0f - (y + 0.5f * yInc) / getHeight());
      y += yInc;
    }

    if (liveUpdateLineEdit) updateValue();
    repaint();
  }

  std::vector<double> defaultValue;
  float barWidth = 2.0f;
  float borderWidth = 2.0f;
  float highlightBorderWidth = 4.0f;
  float scrollBarheight = 8.0f;

  float textSize = 9.0f;
  FontId fontId = -1;
  Palette &pal;

  Scale &scale;

  Point<int> mousePosition{-1, -1};
  Point<int> anchor{0, 0};
  int indexL = 0;
  int indexR = 0;
  int indexRange = 0;
  bool isMouseLeftDown = false;
  bool isMouseRightDown = false;
  bool isMouseEntered = false;

  float sliderWidth = 0;
};
