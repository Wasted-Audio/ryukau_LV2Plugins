// (c) 2019 Takamitsu Endo
//
// This file is part of SyncSawSynth.
//
// SyncSawSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SyncSawSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SyncSawSynth.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <sstream>
#include <string>

#include "TinosBoldItalic.hpp"
#include "Widget.hpp"

class CreditSplash : public NanoWidget {
public:
  explicit CreditSplash(NanoWidget *group, const char *name)
    : NanoWidget(group), name(name)
  {
    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    hide();
  }

  void onNanoDisplay() override
  {
    if (!isVisible()) return;

    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();

    // Border.
    beginPath();
    rect(0, 0, width, height);
    fillColor(backgroundColor);
    fill();
    strokeColor(isMouseEntered ? highlightColor : foregroundColor);
    strokeWidth(borderWidth);
    stroke();

    // Text.
    fillColor(foregroundColor);
    fontFaceId(fontId);
    textAlign(align);

    fontSize(textSize * 1.5f);
    std::stringstream stream;
    stream << name << " " << std::to_string(MAJOR_VERSION) << "."
           << std::to_string(MINOR_VERSION) << "." << std::to_string(PATCH_VERSION);
    text(20.0f, 50.0f, stream.str().c_str(), nullptr);

    fontSize(textSize);
    text(20.0f, 90.0f, "© 2019 Takamitsu Endo (ryukau@gmail.com)", nullptr);
    text(20.0f, 150.0f, "Shift + Drag: Fine Adjustment", nullptr);
    text(20.0f, 180.0f, "Ctrl + Click: Reset to Default", nullptr);
    text(20.0f, 240.0f, "Have a nice day!", nullptr);
  }

  bool onMouse(const MouseEvent &ev) override
  {
    if (contains(ev.pos) && ev.press) {
      hide();
      repaint();
    }
    return true;
  }

  bool onMotion(const MotionEvent &ev) override
  {
    isMouseEntered = contains(ev.pos);
    repaint();
    return false;
  }

protected:
  Color backgroundColor{0xff, 0xff, 0xff};
  Color foregroundColor{0, 0, 0};
  Color highlightColor{0x22, 0x99, 0xff};

  bool isMouseEntered = false;

  int align = ALIGN_BASELINE | ALIGN_MIDDLE;
  float borderWidth = 8.0f;
  float textSize = 18.0f;
  FontId fontId = -1;
  const char *name = nullptr;
};

class SplashButton : public NanoWidget {
public:
  explicit SplashButton(NanoWidget *group, const char *labelText)
    : NanoWidget(group), labelText(labelText)
  {
    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);
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

  bool onMouse(const MouseEvent &ev) override
  {
    if (splashWidget != nullptr && ev.press && contains(ev.pos)) {
      splashWidget->show();
      repaint();
      return true;
    }
    return false;
  }

  bool onMotion(const MotionEvent &ev) override
  {
    isMouseEntered = contains(ev.pos);
    repaint();
    return false;
  }

  void setForegroundColor(Color color) { foregroundColor = color; }
  void setHighlightColor(Color color) { highlightColor = color; }
  void setTextSize(float size) { textSize = size < 0.0f ? 0.0f : size; }

  /**
    Drawing order of widgets are same as the order of instantiation. This causes a problem
    that a widget is unintentionally drawn over another widget. setSplashWidget() is a
    workaround of this drawing order problem.

    Usage:
    1. Instantiate SplashButton.
    2. Create some splashWidget.
    3. Pass splashWidget to SplashButton via setSplashWidget().
  */
  void setSplashWidget(std::shared_ptr<Widget> splashWidget)
  {
    this->splashWidget = splashWidget;
  }

protected:
  Color foregroundColor{0, 0, 0};
  Color highlightColor{0x22, 0x99, 0xff};

  bool isMouseEntered = false;

  const char *labelText = nullptr;
  FontId fontId = -1;
  int align = ALIGN_CENTER | ALIGN_MIDDLE;
  float borderWidth = 2.0f;
  float textSize = 18.0f;
  float margin = 20.0f;

  std::shared_ptr<Widget> splashWidget = nullptr;
};
