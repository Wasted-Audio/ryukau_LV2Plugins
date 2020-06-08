// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of WaveCymbal.
//
// WaveCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// WaveCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with WaveCymbal.  If not, see <https://www.gnu.org/licenses/>.

#include <memory>
#include <unordered_map>
#include <vector>

#include "../common/uibase.hpp"
#include "parameter.hpp"

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
  strokeWidth(borderWidth);
  stroke();

  // Text.
  fillColor(pal.foreground());
  fontFaceId(fontId);
  textAlign(align);

  fontSize(textSize * 1.5f);
  std::stringstream stream;
  stream << name << " " << std::to_string(MAJOR_VERSION) << "."
         << std::to_string(MINOR_VERSION) << "." << std::to_string(PATCH_VERSION);
  text(20.0f, 50.0f, stream.str().c_str(), nullptr);

  fontSize(textSize);
  text(20.0f, 90.0f, "© 2019-2020 Takamitsu Endo (ryukau@gmail.com)", nullptr);
  text(20.0f, 150.0f, "Shift + Drag: Fine Adjustment", nullptr);
  text(20.0f, 180.0f, "Ctrl + Click: Reset to Default", nullptr);
  text(20.0f, 240.0f, "Have a nice day!", nullptr);
}

START_NAMESPACE_DISTRHO

constexpr float uiTextSize = 14.0f;
constexpr float midTextSize = 16.0f;
constexpr float pluginNameTextSize = 28.0f;
constexpr float margin = 5.0f;
constexpr float labelHeight = 20.0f;
constexpr float labelY = 30.0f;
constexpr float knobWidth = 80.0f;
constexpr float knobHeight = knobWidth - 2.0f * margin;
constexpr float knobX = knobWidth; // With margin.
constexpr float knobY = knobHeight + labelY + 2.0f * margin;
constexpr float sliderWidth = 70.0f;
constexpr float sliderHeight = 2.0f * (knobHeight + labelY) + 75.0f;
constexpr float sliderX = 80.0f;
constexpr float sliderY = sliderHeight + labelY;
constexpr float checkboxWidth = 80.0f;
constexpr uint32_t defaultWidth = uint32_t(2 * 20 + sliderX + 7 * knobX + 10 * margin);
constexpr uint32_t defaultHeight = uint32_t(2 * 15 + 2 * knobY + 2 * labelHeight + 55);

class WaveCymbalUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveCymbalUI)

public:
  WaveCymbalUI() : PluginUIBase(defaultWidth, defaultHeight)
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

    // Gain.
    const auto left0 = 10.0f;
    const auto top0 = 20.0f;

    const auto leftGain = left0 + 2.0f * margin;
    addVSlider(
      leftGain, top0, sliderWidth, sliderHeight, margin, labelHeight, uiTextSize, "Gain",
      ID::gain);

    // Excitation.
    const auto leftExcitation = leftGain + sliderX + 2.0f * margin;
    addToggleButton(
      leftExcitation, top0, 2.0f * knobX, labelHeight, midTextSize, "Excitation",
      ID::excitation);

    const auto topExcitation = top0 + labelHeight + margin;
    addKnob<Style::warning>(
      leftExcitation, topExcitation, knobWidth, margin, uiTextSize, "Feedback",
      ID::pickCombFeedback);
    addKnob(
      leftExcitation + knobX, topExcitation, knobWidth, margin, uiTextSize, "Time",
      ID::pickCombTime);

    // Objects.
    const auto leftObjects = leftExcitation + 2.0f * knobX + 4.0f * margin;
    addGroupLabel(leftObjects, top0, 2.0f * knobX, labelHeight, midTextSize, "Objects");

    const auto topObjects = top0 + labelHeight + margin;
    addNumberKnob(
      leftObjects, topObjects, knobWidth, margin, uiTextSize, "nCymbal", ID::nCymbal,
      Scales::nCymbal, 1);
    addNumberKnob(
      leftObjects + knobX, topObjects, knobWidth, margin, uiTextSize, "nString",
      ID::stack, Scales::stack, 1);

    // Wave.
    const auto leftWave = leftObjects + 2.0f * knobX + 4.0f * margin;
    addGroupLabel(leftWave, top0, 3.0f * knobX, labelHeight, midTextSize, "Wave");

    const auto topWave = top0 + labelHeight + margin;
    addKnob<Style::warning>(
      leftWave, topWave, knobWidth, margin, uiTextSize, "Damping", ID::damping);
    addKnob(
      leftWave + knobX, topWave, knobWidth, margin, uiTextSize, "PulsePosition",
      ID::pulsePosition);
    addKnob(
      leftWave + 2.0f * knobX, topWave, knobWidth, margin, uiTextSize, "PulseWidth",
      ID::pulseWidth);

    // Collision.
    const auto top1 = top0 + knobY + 3.0f * margin;

    const auto leftCollision = leftExcitation;
    addToggleButton(
      leftCollision, top1, checkboxWidth, labelHeight, midTextSize, "Collision",
      ID::collision);

    const auto topCollision = top1 + labelHeight + margin;
    addKnob<Style::warning>(
      leftCollision, topCollision, knobWidth, margin, uiTextSize, "Distance",
      ID::distance);

    // Random.
    const auto leftRandom = leftCollision + knobX + 4.0f * margin;
    addGroupLabel(leftRandom, top1, 2.0f * knobX, labelHeight, midTextSize, "Random");

    const auto topRandom = top1 + labelHeight + margin;
    addNumberKnob(
      leftRandom, topRandom, knobWidth, margin, uiTextSize, "Seed", ID::seed,
      Scales::seed, 0);
    addKnob<Style::warning>(
      leftRandom + knobX, topRandom, knobWidth, margin, uiTextSize, "Amount",
      ID::randomAmount);

    // String.
    const auto leftString = leftRandom + 2.0f * knobX + 4.0f * margin;
    addGroupLabel(leftString, top1, 4.0f * knobX, labelHeight, midTextSize, "String");

    const auto topString = top1 + labelHeight + margin;
    addKnob(
      leftString, topString, knobWidth, margin, uiTextSize, "MinHz", ID::minFrequency);
    addKnob(
      leftString + knobX, topString, knobWidth, margin, uiTextSize, "MaxHz",
      ID::maxFrequency);
    addKnob(
      leftString + 2.0f * knobX, topString, knobWidth, margin, uiTextSize, "Decay",
      ID::decay);
    addKnob(
      leftString + 3.0f * knobX, topString, knobWidth, margin, uiTextSize, "Q",
      ID::bandpassQ);

    // Oscillator.
    const auto top2 = top1 + labelHeight + knobY;

    addGroupLabel(
      leftExcitation, top2, 3.0f * knobX + 2.0f * margin, labelHeight, midTextSize,
      "Oscillator");

    const auto topOscillator = top2 + labelHeight + margin;
    addCheckbox(
      leftExcitation, topOscillator, checkboxWidth, labelHeight, uiTextSize, "Retrigger",
      ID::retrigger);

    std::vector<std::string> itemOscType
      = {"Off", "Impulse", "Sustain", "Velvet Noise", "Brown Noise"};
    addOptionMenu(
      leftExcitation + knobX, topOscillator, checkboxWidth, labelHeight, uiTextSize,
      ID::oscType, itemOscType);

    std::vector<std::string> itemCutoffMap = {"Log", "Linear"};
    addOptionMenu(
      leftExcitation + 2.0f * knobX + 2.0f * margin, topOscillator, checkboxWidth,
      labelHeight, uiTextSize, ID::cutoffMap, itemCutoffMap);

    // Smoothness.
    const auto leftSmoothness = leftExcitation + 3.0f * knobX + 4.0f * margin;
    addKnob(
      leftSmoothness + 22.5f, top2 - margin, 50.0f, margin, uiTextSize, "Smoothness",
      ID::smoothness);

    // Plugin name.
    const auto splashWidth = 3.0f * knobX;
    addSplashScreen(
      defaultWidth - 20.0f - splashWidth, top2 + 2.0 * margin, splashWidth, 40.0f, 100.0f,
      20.0f, defaultWidth - 200.0f, defaultHeight - 40.0f, pluginNameTextSize,
      "WaveCymbal");
  }
};

UI *createUI() { return new WaveCymbalUI(); }

END_NAMESPACE_DISTRHO
