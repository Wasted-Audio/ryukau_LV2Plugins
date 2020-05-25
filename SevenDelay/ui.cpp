// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of SevenDelay.
//
// SevenDelay is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SevenDelay is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SevenDelay.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../common/uibase.hpp"
#include "parameter.hpp"

#include "gui/waveview.hpp"

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
  stream << "SevenDelay " << std::to_string(MAJOR_VERSION) << "."
         << std::to_string(MINOR_VERSION) << "." << std::to_string(PATCH_VERSION);
  text(20.0f, 50.0f, stream.str().c_str(), nullptr);

  fontSize(textSize);
  text(20.0f, 90.0f, "© 2019-2020 Takamitsu Endo (ryukau@gmail.com)", nullptr);
  text(20.0f, 150.0f, "Shift + Drag: Fine Adjustment", nullptr);
  text(20.0f, 180.0f, "Ctrl + Click: Reset to Default", nullptr);
  text(20.0f, 240.0f, "Have a nice day!", nullptr);
}

START_NAMESPACE_DISTRHO

constexpr uint32_t defaultWidth = 960;
constexpr uint32_t defaultHeight = 330;
constexpr float pluginNameTextSize = 28.0f;
constexpr float labelHeight = 30.0f;
constexpr float midTextSize = 20.0f;
constexpr float uiTextSize = 16.0f;
constexpr float checkboxWidth = 80.0f;
constexpr float sliderWidth = 70.0f;
constexpr float sliderHeight = 230.0f;
constexpr float margin = 0.0f;

class SevenDelayUI : public PluginUIBase {
protected:
  std::shared_ptr<WaveView> waveView;

  void updateWaveView()
  {
    waveView->shape = param->getFloat(ParameterID::lfoShape);
    waveView->phase = param->getFloat(ParameterID::lfoInitialPhase);
  }

  void onNanoDisplay() override
  {
    updateWaveView();

    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SevenDelayUI)

public:
  SevenDelayUI() : PluginUIBase(defaultWidth, defaultHeight)
  {
    param = std::make_unique<GlobalParameter>();

    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    const auto normalWidth = 80.0f;
    const auto normalHeight = normalWidth + 40.0f;
    const auto smallWidth = 40.0f;
    const auto smallHeight = 50.0f;
    const auto interval = 100.0f;

    // Delay.
    const auto delayTop1 = 50.0f;
    const auto delayLeft = 20.0f;
    addGroupLabel(delayLeft, 10.0f, 480.0f, labelHeight, midTextSize, "Delay");
    addKnob(
      delayLeft, delayTop1, normalWidth, margin, uiTextSize, "Time", ParameterID::time);
    addKnob(
      1.0f * interval + delayLeft, delayTop1, normalWidth, margin, uiTextSize, "Feedback",
      ParameterID::feedback);
    addKnob(
      2.0f * interval + delayLeft, delayTop1, normalWidth, margin, uiTextSize, "Stereo",
      ParameterID::offset);
    addKnob(
      3.0f * interval + delayLeft, delayTop1, normalWidth, margin, uiTextSize, "Wet",
      ParameterID::wetMix);
    addKnob<Style::accent>(
      4.0f * interval + delayLeft, delayTop1, normalWidth, margin, uiTextSize, "Dry",
      ParameterID::dryMix);

    const auto delayTop2 = delayTop1 + normalHeight;
    const auto delayTop3 = delayTop2 + smallHeight;
    const auto delayTop4 = delayTop3 + smallHeight;
    addCheckbox(
      delayLeft + 10.0f, delayTop2, checkboxWidth, labelHeight, uiTextSize, "Sync",
      ParameterID::tempoSync);
    addCheckbox(
      delayLeft + 10.0f, delayTop3, checkboxWidth, labelHeight, uiTextSize, "Negative",
      ParameterID::negativeFeedback);

    addKnob(
      1.0f * interval + delayLeft, delayTop2, smallWidth, margin, uiTextSize, "In Spread",
      ParameterID::inSpread, LabelPosition::right);
    addKnob(
      1.0f * interval + delayLeft, delayTop3, smallWidth, margin, uiTextSize,
      "Out Spread", ParameterID::outSpread, LabelPosition::right);
    addKnob(
      2.3f * interval + delayLeft, delayTop2, smallWidth, margin, uiTextSize, "In Pan",
      ParameterID::inPan, LabelPosition::right);
    addKnob(
      2.3f * interval + delayLeft, delayTop3, smallWidth, margin, uiTextSize, "Out Pan",
      ParameterID::outPan, LabelPosition::right);
    addKnob(
      2.3f * interval + delayLeft, delayTop4, smallWidth, margin, uiTextSize, "DC Kill",
      ParameterID::dckill, LabelPosition::right);

    addKnob(
      3.6f * interval + delayLeft, delayTop2, smallWidth, margin, uiTextSize,
      "Allpass Cut", ParameterID::toneCutoff, LabelPosition::right);
    addKnob(
      3.6f * interval + delayLeft, delayTop3, smallWidth, margin, uiTextSize, "Allpass Q",
      ParameterID::toneQ, LabelPosition::right);
    addKnob(
      3.6f * interval + delayLeft, delayTop4, smallWidth, margin, uiTextSize, "Smooth",
      ParameterID::smoothness, LabelPosition::right);

    // LFO.
    // 750 - 520 = 230 / 3 = 66 + 10
    const auto lfoLeft1 = 520.0f;
    addGroupLabel(520.0f, 10.0f, 420.0f, labelHeight, midTextSize, "LFO");
    addVSlider(
      lfoLeft1, 50.0f, sliderWidth, sliderHeight, margin, labelHeight, uiTextSize,
      "To Time", ParameterID::lfoTimeAmount);
    addVSlider(
      lfoLeft1 + 75.0f, 50.0f, sliderWidth, sliderHeight, margin, labelHeight, uiTextSize,
      "To Allpass", ParameterID::lfoToneAmount);
    addVSlider<Style::accent>(
      lfoLeft1 + 150.0f, 50.0f, sliderWidth, sliderHeight, margin, labelHeight,
      uiTextSize, "Frequency", ParameterID::lfoFrequency);
    const auto lfoLeft2 = lfoLeft1 + 230.0f;
    addKnob(
      lfoLeft2, 50.0f, normalWidth, margin, uiTextSize, "Shape", ParameterID::lfoShape);
    addKnob(
      interval + lfoLeft2, 50.0f, normalWidth, margin, uiTextSize, "Phase",
      ParameterID::lfoInitialPhase);

    const auto waveViewLeft = 760.0f;
    const auto waveViewTop = 170.0f;
    const auto waveViewWidth = 180.0f;
    const auto waveViewHeight = 110.0f;
    addToggleButton(
      waveViewLeft, waveViewTop + waveViewHeight + 10.0f, waveViewWidth, labelHeight,
      midTextSize, "LFO Hold", ParameterID::lfoHold);

    waveView = std::make_shared<WaveView>(this, palette);
    waveView->setSize(waveViewWidth, waveViewHeight);
    waveView->setAbsolutePos(waveViewLeft, waveViewTop);

    // Plugin name.
    const auto nameLeft = delayLeft;
    const auto nameTop = defaultHeight - 50.0f;
    const auto nameWidth = 180.0f;
    addSplashScreen(
      nameLeft, nameTop, nameWidth, 40.0f, 200.0f, 20.0f, defaultWidth - 400.0f,
      defaultHeight - 40.0f, pluginNameTextSize, "SevenDelay");
  }
};

UI *createUI() { return new SevenDelayUI(); }

END_NAMESPACE_DISTRHO
