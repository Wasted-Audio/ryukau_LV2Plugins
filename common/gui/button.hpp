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

#include <string>

#include <iostream>

template<Style style = Style::common> class StateButton : public StateWidget {
public:
  explicit StateButton(
    NanoWidget *group,
    PluginUI *ui,
    std::string labelText,
    std::string key,
    std::string value,
    FontId fontId,
    Palette &palette)
    : StateWidget(group, ui, key, value)
    , labelText(labelText)
    , fontId(fontId)
    , pal(palette)
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
    fill();
    if constexpr (style == Style::accent) {
      fillColor(isPressed ? pal.highlightAccent() : pal.background());
      strokeColor(isMouseEntered ? pal.highlightAccent() : pal.border());
    } else if (style == Style::warning) {
      fillColor(isPressed ? pal.highlightWarning() : pal.background());
      strokeColor(isMouseEntered ? pal.highlightWarning() : pal.border());
    } else {
      fillColor(isPressed ? pal.highlightMain() : pal.background());
      strokeColor(isMouseEntered ? pal.highlightMain() : pal.border());
    }
    strokeWidth(borderWidth);
    stroke();

    // Text.
    fillColor(pal.foreground());
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

  void setTextSize(float size) { textSize = size < 0.0f ? 0.0f : size; }

protected:
  bool isMouseEntered = false;
  bool isPressed = false;

  std::string labelText{""};
  int align = ALIGN_CENTER | ALIGN_MIDDLE;
  float borderWidth = 1.0f;
  float textSize = 18.0f;
  FontId fontId = -1;
  Palette &pal;
};

class ButtonBase : public ValueWidget {
public:
  explicit ButtonBase(
    NanoWidget *group,
    PluginUI *ui,
    std::string labelText,
    FontId fontId,
    Palette &palette)
    : ValueWidget(group, ui, 0.0f), labelText(labelText), fontId(fontId), pal(palette)
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
    strokeColor(isMouseEntered ? pal.highlightButton() : pal.border());
    strokeWidth(borderWidth);
    fillColor(value ? pal.highlightButton() : pal.boxBackground());
    fill();
    stroke();

    // Text.
    fillColor(value ? pal.boxBackground() : pal.foreground());
    fontFaceId(fontId);
    fontSize(textSize);
    textAlign(align);
    text(width / 2, height / 2, labelText.c_str(), nullptr);
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

  void setTextSize(float size) { textSize = size < 0.0f ? 0.0f : size; }

protected:
  bool isMouseEntered = false;

  std::string labelText{""};
  int align = ALIGN_CENTER | ALIGN_MIDDLE;
  float borderWidth = 1.0f;
  float textSize = 18.0f;
  FontId fontId = -1;
  Palette &pal;
};

template<Style style = Style::common> class KickButton : public ButtonBase {
public:
  explicit KickButton(
    NanoWidget *group,
    PluginUI *ui,
    std::string labelText,
    FontId fontId,
    Palette &palette)
    : ButtonBase(group, ui, labelText, fontId, palette)
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
    if constexpr (style == Style::accent) {
      fillColor(value ? pal.highlightAccent() : pal.boxBackground());
      strokeColor(isMouseEntered ? pal.highlightAccent() : pal.border());
    } else if (style == Style::warning) {
      fillColor(value ? pal.highlightWarning() : pal.boxBackground());
      strokeColor(isMouseEntered ? pal.highlightWarning() : pal.border());
    } else {
      fillColor(value ? pal.highlightButton() : pal.boxBackground());
      strokeColor(isMouseEntered ? pal.highlightButton() : pal.border());
    }
    strokeWidth(borderWidth);
    fill();
    stroke();

    // Text.
    fillColor(value ? pal.boxBackground() : pal.foreground());
    fontFaceId(fontId);
    fontSize(textSize);
    textAlign(align);
    text(width / 2, height / 2, labelText.c_str(), nullptr);
  }

  bool onMouse(const MouseEvent &ev) override
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
};

template<Style style = Style::common> class ToggleButton : public ButtonBase {
public:
  explicit ToggleButton(
    NanoWidget *group,
    PluginUI *ui,
    std::string labelText,
    FontId fontId,
    Palette &palette)
    : ButtonBase(group, ui, labelText, fontId, palette)
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
    if constexpr (style == Style::accent) {
      fillColor(value ? pal.highlightAccent() : pal.boxBackground());
      strokeColor(isMouseEntered ? pal.highlightAccent() : pal.border());
    } else if (style == Style::warning) {
      fillColor(value ? pal.highlightWarning() : pal.boxBackground());
      strokeColor(isMouseEntered ? pal.highlightWarning() : pal.border());
    } else {
      fillColor(value ? pal.highlightButton() : pal.boxBackground());
      strokeColor(isMouseEntered ? pal.highlightButton() : pal.border());
    }
    strokeWidth(borderWidth);
    fill();
    stroke();

    // Text.
    fillColor(value ? pal.boxBackground() : pal.foreground());
    fontFaceId(fontId);
    fontSize(textSize);
    textAlign(align);
    text(width / 2, height / 2, labelText.c_str(), nullptr);
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
