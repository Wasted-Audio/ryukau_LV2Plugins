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

#include <string>

#include <iostream>

class StateButton : public StateWidget {
public:
  explicit StateButton(
    NanoWidget *group,
    PluginUI *ui,
    std::string labelText,
    std::string key,
    std::string value,
    FontId fontId)
    : StateWidget(group, ui, key, value), labelText(labelText), fontId(fontId)
  {
  }

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();

    // Rect.
    beginPath();
    rect(0, 0, width, height);
    if (isPressed) {
      fillColor(highlightColor);
      fill();
    }
    strokeColor(isMouseEntered ? highlightColor : foregroundColor);
    strokeWidth(borderWidth);
    stroke();

    // Text.
    fillColor(foregroundColor);
    fontFaceId(fontId);
    fontSize(textSize);
    textAlign(align);
    text(width / 2, height / 2, labelText.c_str(), nullptr);
  }

  virtual bool onMouse(const MouseEvent &ev) override
  {
    if (contains(ev.pos)) {
      isPressed = ev.press;
      if (isPressed) updateValue();
      repaint();
      return true;
    } else if (!ev.press) {
      isPressed = false;
      repaint();
    }
    return false;
  }

  virtual bool onMotion(const MotionEvent &ev) override
  {
    isMouseEntered = contains(ev.pos);
    repaint();
    return false;
  }

  void setForegroundColor(Color color) { foregroundColor = color; }
  void setHighlightColor(Color color) { highlightColor = color; }
  void setTextSize(float size) { textSize = size < 0.0f ? 0.0f : size; }

protected:
  Color foregroundColor{0, 0, 0};
  Color highlightColor{0x22, 0x99, 0xff};

  bool isMouseEntered = false;
  bool isPressed = false;

  std::string labelText{""};
  int align = ALIGN_CENTER | ALIGN_MIDDLE;
  float borderWidth = 1.0f;
  float textSize = 18.0f;
  FontId fontId = -1;
};

class Button : public ValueWidget {
public:
  explicit Button(NanoWidget *group, PluginUI *ui, std::string labelText, FontId fontId)
    : ValueWidget(group, ui, 0.0f), labelText(labelText), fontId(fontId)
  {
  }

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();

    // Rect.
    beginPath();
    rect(0, 0, width, height);
    if (value) {
      fillColor(highlightColor);
      fill();
    }
    strokeColor(isMouseEntered ? highlightColor : foregroundColor);
    strokeWidth(borderWidth);
    stroke();

    // Text.
    fillColor(foregroundColor);
    fontFaceId(fontId);
    fontSize(textSize);
    textAlign(align);
    text(width / 2, height / 2, labelText.c_str(), nullptr);
  }

  virtual bool onMouse(const MouseEvent &ev) override
  {
    if (contains(ev.pos)) {
      value = ev.press;
      updateValue();
      repaint();
      return true;
    } else if (!ev.press) {
      value = false;
      updateValue();
      repaint();
    }
    return false;
  }

  virtual bool onMotion(const MotionEvent &ev) override
  {
    isMouseEntered = contains(ev.pos);
    repaint();
    return false;
  }

  virtual bool onScroll(const ScrollEvent &ev) override
  {
    if (!contains(ev.pos)) return false;
    if (ev.delta.getY() < 0)
      value = true;
    else if (ev.delta.getY() > 0)
      value = false;
    updateValue();
    repaint();
    return true;
  }

  void setForegroundColor(Color color) { foregroundColor = color; }
  void setHighlightColor(Color color) { highlightColor = color; }
  void setTextSize(float size) { textSize = size < 0.0f ? 0.0f : size; }

protected:
  Color foregroundColor{0, 0, 0};
  Color highlightColor{0x22, 0x99, 0xff};

  bool isMouseEntered = false;

  std::string labelText{""};
  int align = ALIGN_CENTER | ALIGN_MIDDLE;
  float borderWidth = 1.0f;
  float textSize = 18.0f;
  FontId fontId = -1;
};

class ToggleButton : public Button {
public:
  explicit ToggleButton(
    NanoWidget *group, PluginUI *ui, std::string labelText, FontId fontId)
    : Button(group, ui, labelText, fontId)
  {
  }

  bool onMouse(const MouseEvent &ev) override
  {
    if (contains(ev.pos) && ev.press) {
      value = !value;
      updateValue();
      repaint();
      return true;
    }
    return false;
  }
};
