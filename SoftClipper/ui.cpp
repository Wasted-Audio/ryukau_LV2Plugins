// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of SoftClipper.
//
// SoftClipper is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SoftClipper is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SoftClipper.  If not, see <https://www.gnu.org/licenses/>.

#include "../common/uibase.hpp"
#include "parameter.hpp"

#include <sstream>
#include <tuple>

void CreditSplash::onNanoDisplay()
{
  if (!isVisible()) return;

  resetTransform();
  translate(getAbsoluteX(), getAbsoluteY());

  const auto width = getWidth();
  const auto height = getHeight();

  // Border.
  beginPath();
  rect(0, 0, width, height);
  fillColor(pal.background());
  fill();
  strokeColor(isMouseEntered ? pal.highlightMain() : pal.foreground());
  strokeWidth(2.0f);
  stroke();

  // Text.
  fillColor(pal.foreground());
  fontFaceId(fontId);
  textAlign(align);

  fontSize(18.0f);
  std::stringstream stream;
  stream << name << " " << std::to_string(MAJOR_VERSION) << "."
         << std::to_string(MINOR_VERSION) << "." << std::to_string(PATCH_VERSION);
  text(20.0f, 20.0f, stream.str().c_str(), nullptr);

  fontSize(14.0f);
  text(20.0f, 40.0f, "© 2020 Takamitsu Endo (ryukau@gmail.com)", nullptr);

  text(20.0f, 65.0f, "- Shift + Left Drag: Fine Adjustment", nullptr);
  text(20.0f, 85.0f, "- Ctrl + Left Click: Reset to Default", nullptr);

  text(372.0f, 85.0f, "Have a nice day!", nullptr);
}

START_NAMESPACE_DISTRHO

constexpr float uiTextSize = 14.0f;
constexpr float midTextSize = 16.0f;
constexpr float pluginNameTextSize = 18.0f;
constexpr float margin = 5.0f;
constexpr float labelHeight = 20.0f;
constexpr float labelY = 30.0f;
constexpr float knobWidth = 50.0f;
constexpr float knobHeight = 40.0f;
constexpr float knobX = 60.0f; // With margin.
constexpr float knobY = knobHeight + labelY;
constexpr float checkboxWidth = 60.0f;
constexpr float splashHeight = 20.0f;
constexpr uint32_t defaultWidth = uint32_t(8 * knobX + 30);
constexpr uint32_t defaultHeight = uint32_t(30 + knobX + labelY + 3 * margin);

class SoftClipperUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoftClipperUI)

public:
  SoftClipperUI() : PluginUIBase(defaultWidth, defaultHeight)
  {
    param = std::make_unique<GlobalParameter>();

    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    using ID = ParameterID::ID;

    const auto top0 = 15.0f;
    const auto left0 = 15.0f;

    addKnob(left0 + 0 * knobX, top0, knobX, margin, uiTextSize, "Input", ID::inputGain);
    addKnob(left0 + 1 * knobX, top0, knobX, margin, uiTextSize, "Clip", ID::clip);
    addKnob(left0 + 2 * knobX, top0, knobX, margin, uiTextSize, "Ratio", ID::ratio);
    addKnob(left0 + 3 * knobX, top0, knobX, margin, uiTextSize, "Slope", ID::slope);
    addKnob(left0 + 4 * knobX, top0, knobX, margin, uiTextSize, "Output", ID::outputGain);

    const auto left1 = left0 + 5 * knobX + 4 * margin;
    const auto left2 = left1 + knobX + 6 * margin;

    auto orderIntLabel
      = addLabel(left1, top0, knobX, labelHeight, uiTextSize, "Order Integer");
    orderIntLabel->setTextAlign(ALIGN_LEFT | ALIGN_MIDDLE);
    addTextKnob(
      left2, top0, knobX, labelHeight, uiTextSize, ID::orderInteger,
      Scales::orderInteger);

    auto orderFracLabel
      = addLabel(left1, top0 + labelY, knobX, labelHeight, uiTextSize, "Order Fraction");
    orderFracLabel->setTextAlign(ALIGN_LEFT | ALIGN_MIDDLE);
    addTextKnob(
      left2, top0 + labelY, knobX, labelHeight, uiTextSize, ID::orderFraction,
      Scales::defaultScale, false, 4);

    addCheckbox(
      left1, top0 + 2 * labelY, knobX, labelHeight, uiTextSize, "OverSample",
      ID::oversample);

    // Plugin name.
    const auto splashTop = defaultHeight - splashHeight - 15.0f;
    const auto splashLeft = left0;
    addSplashScreen(
      splashLeft, splashTop, 2.0f * knobX - 2 * margin, splashHeight, 15.0f, 15.0f,
      defaultWidth - 30.0f, defaultHeight - 30.0f, pluginNameTextSize, "SoftClipper");
  }
};

UI *createUI() { return new SoftClipperUI(); }

END_NAMESPACE_DISTRHO
