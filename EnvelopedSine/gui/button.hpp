// (c) 2019-2020 Takamitsu Endo
//
// This file is part of EnvelopedSine.
//
// EnvelopedSine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// EnvelopedSine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with EnvelopedSine.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "valuewidget.hpp"

class Button : public ValueWidget {
public:
  explicit Button(NanoWidget *group, PluginUI *ui, const char *labelText, FontId fontId)
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
    if (labelText == nullptr) return;
    fillColor(foregroundColor);
    fontFaceId(fontId);
    fontSize(textSize);
    textAlign(align);
    text(width / 2, height / 2, labelText, nullptr);
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

  const char *labelText = nullptr;
  int align = ALIGN_CENTER | ALIGN_MIDDLE;
  float borderWidth = 1.0f;
  float textSize = 18.0f;
  FontId fontId = -1;
};

class ToggleButton : public Button {
public:
  explicit ToggleButton(
    NanoWidget *group, PluginUI *ui, const char *labelText, FontId fontId)
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
