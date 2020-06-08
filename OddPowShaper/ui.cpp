// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of OddPowShaper.
//
// OddPowShaper is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OddPowShaper is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OddPowShaper.  If not, see <https://www.gnu.org/licenses/>.

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
constexpr uint32_t defaultWidth = uint32_t(5 * knobX + 30);
constexpr uint32_t defaultHeight = uint32_t(30 + knobX + labelY + 3 * margin);

class OddPowShaperUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OddPowShaperUI)

public:
  OddPowShaperUI() : PluginUIBase(defaultWidth, defaultHeight)
  {
    param = std::make_unique<GlobalParameter>();

    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    if (palette.fontPath().size() > 0)
      fontId = createFontFromFile("main", palette.fontPath().c_str());

    if (fontId < 0) {
      fontId = createFontFromMemory(
        "main", (unsigned char *)(FontData::TinosBoldItalicData),
        FontData::TinosBoldItalicDataSize, false);
    }

    using ID = ParameterID::ID;

    const auto top0 = 15.0f;
    const auto left0 = 15.0f;

    addKnob(left0 + 0 * knobX, top0, knobX, margin, uiTextSize, "Drive", ID::drive);
    addKnob(left0 + 1 * knobX, top0, knobX, margin, uiTextSize, "Boost", ID::boost);
    addKnob(left0 + 2 * knobX, top0, knobX, margin, uiTextSize, "Output", ID::outputGain);

    const auto top1 = top0 + knobY + 3 * margin;
    addLabel(left0, top1, knobX, labelHeight, uiTextSize, "Order");
    addTextKnob(
      left0 + knobX, top1, knobX, labelHeight, uiTextSize, ID::order, Scales::order,
      false, 0, 1);

    const auto checkboxLeft1 = left0 + 3 * knobX + 2 * margin;
    const auto checkboxHeight = labelY - margin;
    addCheckbox(checkboxLeft1, top0, knobX, labelHeight, uiTextSize, "Flip", ID::flip);
    addCheckbox(
      checkboxLeft1, top0 + checkboxHeight, knobX, labelHeight, uiTextSize, "Inverse",
      ID::inverse);
    addCheckbox(
      checkboxLeft1, top0 + 2 * checkboxHeight, knobX, labelHeight, uiTextSize,
      "OverSample", ID::oversample);

    // Plugin name.
    const auto splashTop = defaultHeight - splashHeight - 15.0f;
    const auto splashLeft = checkboxLeft1;
    addSplashScreen(
      splashLeft, splashTop, 2.0f * knobX - 2 * margin, splashHeight, 15.0f, 15.0f,
      defaultWidth - 30.0f, defaultHeight - 30.0f, pluginNameTextSize, "OddPowShaper");
  }
};

UI *createUI() { return new OddPowShaperUI(); }

END_NAMESPACE_DISTRHO
