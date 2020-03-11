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

#include "../dsp/constants.hpp"
#include "valuewidget.hpp"

#include <sstream>
#include <string>

class Knob : public ValueWidget {
public:
  float sensitivity = 0.004f; // MovedPixel * sensitivity = valueChanged.
  float lowSensitivity = sensitivity / 5.0f;

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
    arc(
      tip.getX() + centerX, tip.getY() + centerY, halfArcWidth, -SomeDSP::pi, SomeDSP::pi,
      CCW);
    fill();
  }

  virtual bool onMouse(const MouseEvent &ev) override
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

  Point<int> anchorPoint{0, 0};
  bool isMouseLeftDown = false;
  bool isMouseEntered = false;
};

template<typename Scale> class TextKnob : public Knob {
public:
  explicit TextKnob(
    NanoWidget *group, PluginUI *ui, FontId fontId, Scale &scale, bool isDecibel)
    : Knob(group, ui), fontId(fontId), scale(scale), isDecibel(isDecibel)
  {
    sensitivity = 0.002f;
    lowSensitivity = sensitivity / 10.0f;
  }

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();

    // Border.
    beginPath();
    rect(0, 0, width, height);
    strokeColor(isMouseEntered ? highlightColor : foregroundColor);
    strokeWidth(borderWidth);
    stroke();

    // Text.
    fillColor(foregroundColor);
    fontFaceId(fontId);
    fontSize(textSize);
    textAlign(ALIGN_CENTER | ALIGN_MIDDLE);

    auto displayValue = isDecibel ? 20.0 * log10(scale.map(value)) : scale.map(value);
    std::ostringstream os;
    os.precision(precision);
    os << std::fixed << displayValue + offset;
    std::string txt(os.str());
    text(width / 2, height / 2, txt.c_str(), nullptr);
  }

  virtual bool onMouse(const MouseEvent &ev) override
  {
    if (ev.press && contains(ev.pos)) {
      if (ev.button == 1) {
        isMouseLeftDown = true;
        anchorPoint = ev.pos;
        if (ev.mod & kModifierControl) {
          value = defaultValue;
          updateValue();
        }
      } else if (ev.button == 3) {
        value = value > 0 ? 0 : 1;
        updateValue();
      }
      repaint();
      return true;
    }
    isMouseLeftDown = false;
    repaint();
    return false;
  }

  void setForegroundColor(Color color) { foregroundColor = color; }
  void setPrecision(uint32_t precision) { this->precision = precision; }
  void setTextSize(float size) { textSize = size < 0.0f ? 0.0f : size; }

  int32_t offset = 0;

protected:
  Color foregroundColor{0, 0, 0};
  float borderWidth = 1.0f;
  uint32_t precision = 0;

  float textSize = 14.0f;
  FontId fontId = -1;
  Scale &scale;
  const bool isDecibel;
};

template<typename Scale> class NumberKnob : public Knob {
public:
  explicit NumberKnob(NanoWidget *group, PluginUI *ui, FontId fontId, Scale &scale)
    : Knob(group, ui), fontId(fontId), scale(scale)
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
      centerX, centerY, std::to_string(int32_t(floor(scale.map(value)))).c_str(),
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
};
