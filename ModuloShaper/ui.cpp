// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of ModuloShaper.
//
// ModuloShaper is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ModuloShaper is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ModuloShaper.  If not, see <https://www.gnu.org/licenses/>.

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

  fontSize(textSize * 1.4f);
  std::stringstream stream;
  stream << name << " " << std::to_string(MAJOR_VERSION) << "."
         << std::to_string(MINOR_VERSION) << "." << std::to_string(PATCH_VERSION);
  text(20.0f, 20.0f, stream.str().c_str(), nullptr);

  fontSize(14.0f);
  text(20.0f, 45.0f, "© 2020-2020 Takamitsu Endo (ryukau@gmail.com)", nullptr);

  text(20.0f, 70.0f, "- Shift + Left Drag: Fine Adjustment", nullptr);
  text(20.0f, 90.0f, "- Ctrl + Left Click: Reset to Default", nullptr);

  text(20.0f, 115.0f, "Caution! Tuning More* knobs may outputs loud signal.", nullptr);

  text(20.0f, 140.0f, "Have a nice day!", nullptr);
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
constexpr float splashHeight = 30.0f;
constexpr uint32_t defaultWidth = uint32_t(6 * knobX + 30);
constexpr uint32_t defaultHeight = uint32_t(40 + 2 * knobY + 2 * margin + labelY);

class ModuloShaperUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModuloShaperUI)

public:
  ModuloShaperUI() : PluginUIBase(defaultWidth, defaultHeight)
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

    const auto top0 = 16.0f;
    const auto left0 = 15.0f;

    addKnob(
      left0 + 0 * knobX, top0, knobX, margin, uiTextSize, "Cutoff", ID::lowpassCutoff);
    addKnob(left0 + 1 * knobX, top0, knobX, margin, uiTextSize, "Input", ID::inputGain);
    addKnob(left0 + 2 * knobX, top0, knobX, margin, uiTextSize, "Add", ID::add);
    addKnob(left0 + 3 * knobX, top0, knobX, margin, uiTextSize, "Mul", ID::mul);
    addKnob(left0 + 4 * knobX, top0, knobX, margin, uiTextSize, "Pre-Clip", ID::clipGain);
    addKnob(left0 + 5 * knobX, top0, knobX, margin, uiTextSize, "Output", ID::outputGain);

    const auto top1 = top0 + knobY + 2 * margin;

    addKnob<Style::warning>(
      left0 + 2 * knobX, top1, knobX, margin, uiTextSize, "More Add", ID::moreAdd);
    addKnob<Style::warning>(
      left0 + 3 * knobX, top1, knobX, margin, uiTextSize, "More Mul", ID::moreMul);

    const auto top1Checkbox = top1 + 2.5 * margin;
    const auto leftCheckbox = left0 + 4 * knobX + 2 * margin;
    addCheckbox(
      leftCheckbox, top1Checkbox, knobX, labelHeight, uiTextSize, "Hardclip",
      ID::hardclip);

    addCheckbox(
      leftCheckbox, top1Checkbox + labelY, knobX, labelHeight, uiTextSize, "Lowpass",
      ID::lowpass);

    const auto top2 = top1 + knobY + 3 * margin;

    addLabel(left0, top2, 1.5f * knobX, labelHeight, uiTextSize, "Anti-aliasing");
    std::vector<std::string> typeItems{
      "None", "16x OverSampling", "PolyBLEP 4", "PolyBLEP 8"};
    addOptionMenu(
      left0 + 1.5f * knobX, top2, 2 * knobX, labelHeight, uiTextSize, ID::type,
      typeItems);

    // Plugin name.
    const auto splashTop = defaultHeight - splashHeight - 15.0f;
    const auto splashLeft = defaultWidth + 2 * margin - 2 * knobX - 15.0f;
    addSplashScreen(
      splashLeft, splashTop, 2 * knobX - 2 * margin, splashHeight, 15.0f, 15.0f,
      defaultWidth - 30.0f, defaultHeight - 30.0f, pluginNameTextSize, "ModuloShaper");
  }
};

UI *createUI() { return new ModuloShaperUI(); }

END_NAMESPACE_DISTRHO
