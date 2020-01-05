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

#include <memory>
#include <sstream>
#include <string>

#include "Widget.hpp"

class CreditSplash : public NanoWidget {
public:
  explicit CreditSplash(NanoWidget *group, const char *name, FontId fontId)
    : NanoWidget(group), name(name), fontId(fontId)
  {
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
    text(20.0f, 90.0f, "© 2019-2020 Takamitsu Endo (ryukau@gmail.com)", nullptr);

    text(20.0f, 150.0f, "- Knob -", nullptr);
    text(20.0f, 180.0f, "Shift + Left Drag    |  Fine Adjustment", nullptr);
    text(20.0f, 210.0f, "Ctrl + Left Click    |  Reset to Default", nullptr);

    text(20.0f, 270.0f, "- Number -", nullptr);
    text(20.0f, 300.0f, "Shares same controls with knob, and:", nullptr);
    text(20.0f, 330.0f, "Right Click    |  Flip Minimum and Maximum", nullptr);

    text(360.0f, 150.0f, "- Overtone -", nullptr);

    text(360.0f, 180.0f, "Ctrl + Left Click    |  Reset to Default", nullptr);
    text(360.0f, 210.0f, "Right Drag    |  Draw Line", nullptr);
    text(360.0f, 240.0f, "A    |  Sort Ascending Order", nullptr);
    text(360.0f, 270.0f, "D    |  Sort Decending Order", nullptr);
    text(360.0f, 300.0f, "F    |  Low-pass Filter", nullptr);
    text(360.0f, 330.0f, "Shift + F    |  High-pass Filter", nullptr);
    text(360.0f, 360.0f, "I    |  Invert Value", nullptr);
    text(360.0f, 390.0f, "Shift + I    |  Invert Value (Minimum to 0)", nullptr);
    text(360.0f, 420.0f, "N    |  Normalize", nullptr);
    text(360.0f, 450.0f, "Shift + N    |  Normalize (Minimum to 0)", nullptr);

    text(680.0f, 180.0f, "H    |  Emphasize High", nullptr);
    text(680.0f, 210.0f, "L    |  Emphasize Low", nullptr);
    text(680.0f, 240.0f, "P    |  Permute", nullptr);
    text(680.0f, 270.0f, "R    |  Randomize", nullptr);
    text(680.0f, 300.0f, "Shift + R    |  Sparse Randomize", nullptr);
    text(680.0f, 330.0f, "S    |  Subtle Randomize", nullptr);
    text(680.0f, 360.0f, ", (Comma)    |  Rotate Back", nullptr);
    text(680.0f, 390.0f, ". (Period)    |  Rotate Forward", nullptr);
    text(680.0f, 420.0f, "1    |  Decrease Odd", nullptr);
    text(680.0f, 450.0f, "2-9    |  Decrease 2n-9n", nullptr);

    text(740.0f, 510.0f, "Have a nice day!", nullptr);
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

  const char *name = nullptr;
  FontId fontId = -1;
  int align = ALIGN_BASELINE | ALIGN_MIDDLE;
  float borderWidth = 8.0f;
  float textSize = 18.0f;
};

class SplashButton : public NanoWidget {
public:
  explicit SplashButton(NanoWidget *group, const char *labelText, FontId fontId)
    : NanoWidget(group), labelText(labelText), fontId(fontId)
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
