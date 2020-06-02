// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of L4Reverb.
//
// L4Reverb is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// L4Reverb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with L4Reverb.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"

#include "../common/uibase.hpp"

#include "gui/panicbutton.hpp"

constexpr float uiMargin = 20.0f;
constexpr float uiTextSize = 14.0f;
constexpr float midTextSize = 16.0f;
constexpr float pluginNameTextSize = 22.0f;
constexpr float margin = 5.0f;
constexpr float labelHeight = 20.0f;
constexpr float labelY = 30.0f;
constexpr float knobWidth = 50.0f;
constexpr float knobHeight = 40.0f;
constexpr float knobX = 60.0f; // With margin.
constexpr float knobY = knobHeight + labelY;
constexpr float textKnobX = 80.0f;
constexpr float splashHeight = 40.0f;
constexpr float scrollBarHeight = 10.0f;
constexpr float barboxWidth = 2 * nDepth1;
constexpr float barboxHeight = 2 * knobY + labelY + scrollBarHeight;
constexpr float barboxWidthSmall = 4 * nDepth2;

constexpr float barboxSectionWidth = barboxWidth + barboxWidthSmall + 3 * labelY;
constexpr float barboxSectionHeight
  = 3 * scrollBarHeight + 3 * barboxHeight + 2 * labelHeight;

constexpr float leftPanelWidth = 4 * textKnobX + 6 * margin;

constexpr uint32_t defaultWidth
  = uint32_t(leftPanelWidth + labelY + barboxSectionWidth + 2 * uiMargin);
constexpr uint32_t defaultHeight = uint32_t(barboxSectionHeight + 2 * uiMargin);

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
  text(20.0f, 90.0f, "© 2020 Takamitsu Endo (ryukau@gmail.com)", nullptr);

  text(20.0f, 140.0f, "- Knob -", nullptr);
  text(20.0f, 160.0f, "Shift + Left Drag", nullptr);
  text(180.0f, 160.0f, "Fine Adjustment", nullptr);
  text(20.0f, 180.0f, "Ctrl + Left Click", nullptr);
  text(180.0f, 180.0f, "Reset to Default", nullptr);

  text(20.0f, 220.0f, "- Number -", nullptr);
  text(20.0f, 240.0f, "Shares same controls with knob, and:", nullptr);
  text(20.0f, 260.0f, "Right Click", nullptr);
  text(180.0f, 260.0f, "Flip Min/Max", nullptr);

  text(20.0f, 300.0f, "- BarBox -", nullptr);
  text(20.0f, 320.0f, "Ctrl + Left Click", nullptr);
  text(180.0f, 320.0f, "Reset to Default", nullptr);
  text(20.0f, 340.0f, "Right Drag", nullptr);
  text(180.0f, 340.0f, "Draw Line", nullptr);
  text(20.0f, 360.0f, "Shift + D", nullptr);
  text(180.0f, 360.0f, "Toggle Min/Mid/Max", nullptr);
  text(20.0f, 380.0f, "I", nullptr);
  text(180.0f, 380.0f, "Invert Value", nullptr);
  text(20.0f, 400.0f, "R", nullptr);
  text(180.0f, 400.0f, "Randomize", nullptr);
  text(20.0f, 420.0f, "T", nullptr);
  text(180.0f, 420.0f, "Subtle Randomize", nullptr);
  text(20.0f, 440.0f, ", (Comma)", nullptr);
  text(180.0f, 440.0f, "Rotate Back", nullptr);
  text(20.0f, 460.0f, ". (Period)", nullptr);
  text(180.0f, 460.0f, "Rotate Forward", nullptr);
  text(20.0f, 480.0f, "1", nullptr);
  text(180.0f, 480.0f, "Decrease", nullptr);
  text(20.0f, 500.0f, "2-9", nullptr);
  text(180.0f, 500.0f, "Decrease 2n-9n", nullptr);

  const float mid = (defaultWidth - 2 * uiMargin) / 2;

  text(mid, 140.0f, "Changing InnerFeed or OuterFeed may outputs", nullptr);
  text(mid, 160.0f, "loud signal.", nullptr);

  text(mid, 200.0f, "Use Panic! button in case of blow up.", nullptr);

  text(mid, 240.0f, "Have a nice day!", nullptr);
}

START_NAMESPACE_DISTRHO

class L4ReverbUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(L4ReverbUI)

public:
  L4ReverbUI() : PluginUIBase(defaultWidth, defaultHeight)
  {
    param = std::make_unique<GlobalParameter>();

    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    using ID = ParameterID::ID;

    const auto top0 = uiMargin;
    const auto left0 = uiMargin;

    // Multipliers.
    const auto mulTop0 = top0;
    const auto mulTop1 = mulTop0 + labelY;
    const auto mulTop2 = mulTop1 + labelY;
    const auto mulTop3 = mulTop2 + labelY;
    const auto mulTop4 = mulTop3 + labelY;
    const auto mulTop5 = mulTop4 + labelY;
    const auto mulTop6 = mulTop5 + labelY;
    const auto mulTop7 = mulTop6 + labelY;
    const auto mulTop8 = mulTop7 + 2 * labelY;
    const auto mulLeft0 = left0;
    const auto mulLeft1 = mulLeft0 + textKnobX + 2 * margin;
    const auto mulLeft2 = mulLeft1 + textKnobX + 2 * margin;
    const auto mulLeft3 = mulLeft2 + textKnobX + 2 * margin;

    addGroupLabel(
      mulLeft0, mulTop0, leftPanelWidth, labelHeight, midTextSize, "Multiplier");

    addLabel(mulLeft1, mulTop1, textKnobX, labelHeight, uiTextSize, "Base");
    addLabel(mulLeft2, mulTop1, textKnobX, labelHeight, uiTextSize, "Offset");
    addLabel(mulLeft3, mulTop1, textKnobX, labelHeight, uiTextSize, "Modulation");

    addLabel(
      mulLeft0, mulTop2, textKnobX, labelHeight, uiTextSize, "Time",
      ALIGN_LEFT | ALIGN_MIDDLE);
    addLabel(
      mulLeft0, mulTop3, textKnobX, labelHeight, uiTextSize, "InnerFeed",
      ALIGN_LEFT | ALIGN_MIDDLE);
    addLabel(
      mulLeft0, mulTop4, textKnobX, labelHeight, uiTextSize, "D1 Feed",
      ALIGN_LEFT | ALIGN_MIDDLE);
    addLabel(
      mulLeft0, mulTop5, textKnobX, labelHeight, uiTextSize, "D2 Feed",
      ALIGN_LEFT | ALIGN_MIDDLE);
    addLabel(
      mulLeft0, mulTop6, textKnobX, labelHeight, uiTextSize, "D3 Feed",
      ALIGN_LEFT | ALIGN_MIDDLE);
    addLabel(
      mulLeft0, mulTop7, textKnobX, labelHeight, uiTextSize, "D4 Feed",
      ALIGN_LEFT | ALIGN_MIDDLE);

    addTextKnob(
      mulLeft1, mulTop2, textKnobX, labelHeight, uiTextSize, ID::timeMultiply,
      Scales::timeMultiply, false, 8);
    addTextKnob(
      mulLeft1, mulTop3, textKnobX, labelHeight, uiTextSize, ID::innerFeedMultiply,
      Scales::defaultScale, false, 4);
    addTextKnob(
      mulLeft1, mulTop4, textKnobX, labelHeight, uiTextSize, ID::d1FeedMultiply,
      Scales::defaultScale, false, 4);
    addTextKnob(
      mulLeft1, mulTop5, textKnobX, labelHeight, uiTextSize, ID::d2FeedMultiply,
      Scales::defaultScale, false, 4);
    addTextKnob(
      mulLeft1, mulTop6, textKnobX, labelHeight, uiTextSize, ID::d3FeedMultiply,
      Scales::defaultScale, false, 4);
    addTextKnob(
      mulLeft1, mulTop7, textKnobX, labelHeight, uiTextSize, ID::d4FeedMultiply,
      Scales::defaultScale, false, 4);

    addTextKnob(
      mulLeft2, mulTop2, textKnobX, labelHeight, uiTextSize, ID::timeOffsetRange,
      Scales::timeOffsetRange, false, 8);
    addTextKnob(
      mulLeft2, mulTop3, textKnobX, labelHeight, uiTextSize, ID::innerFeedOffsetRange,
      Scales::defaultScale, false, 4);
    addTextKnob(
      mulLeft2, mulTop4, textKnobX, labelHeight, uiTextSize, ID::d1FeedOffsetRange,
      Scales::defaultScale, false, 4);
    addTextKnob(
      mulLeft2, mulTop5, textKnobX, labelHeight, uiTextSize, ID::d2FeedOffsetRange,
      Scales::defaultScale, false, 4);
    addTextKnob(
      mulLeft2, mulTop6, textKnobX, labelHeight, uiTextSize, ID::d3FeedOffsetRange,
      Scales::defaultScale, false, 4);
    addTextKnob(
      mulLeft2, mulTop7, textKnobX, labelHeight, uiTextSize, ID::d4FeedOffsetRange,
      Scales::defaultScale, false, 4);

    const auto mulLeft3Mid = mulLeft3 + floorf(textKnobX / 2) - 5;
    addCheckbox(
      mulLeft3Mid, mulTop2, textKnobX, labelHeight, uiTextSize, "", ID::timeModulation);
    addCheckbox(
      mulLeft3Mid, mulTop3, textKnobX, labelHeight, uiTextSize, "",
      ID::innerFeedModulation);
    addCheckbox(
      mulLeft3Mid, mulTop4, textKnobX, labelHeight, uiTextSize, "", ID::d1FeedModulation);
    addCheckbox(
      mulLeft3Mid, mulTop5, textKnobX, labelHeight, uiTextSize, "", ID::d2FeedModulation);
    addCheckbox(
      mulLeft3Mid, mulTop6, textKnobX, labelHeight, uiTextSize, "", ID::d3FeedModulation);
    addCheckbox(
      mulLeft3Mid, mulTop7, textKnobX, labelHeight, uiTextSize, "", ID::d4FeedModulation);

    // Panic button.
    auto panicButton
      = std::make_shared<PanicButton>(this, this, "Panic!", fontId, palette);
    panicButton->setSize(2 * textKnobX + 2 * margin, 2 * labelHeight);
    panicButton->setAbsolutePos(mulLeft1, mulTop8 - labelHeight / 2);
    panicButton->setTextSize(midTextSize);
    widget.push_back(panicButton);

    // Mix.
    const auto offsetKnobX = floorf((textKnobX - knobX) / 2);

    const auto mixTop0 = mulTop0 + 11 * labelY;
    const auto mixTop1 = mixTop0 + labelY;

    const auto mixLeft0 = left0 + 2 * offsetKnobX;
    const auto mixLeft1 = mixLeft0 + knobX + 2 * margin;

    addGroupLabel(
      mixLeft0, mixTop0, 2 * knobX + 2 * margin, labelHeight, midTextSize, "Mix");
    addKnob(mixLeft0, mixTop1, knobX, margin, uiTextSize, "Dry", ID::dry);
    addKnob(mixLeft1, mixTop1, knobX, margin, uiTextSize, "Wet", ID::wet);

    // Stereo.
    const auto stereoLeft0 = left0 + 2 * textKnobX + 4 * margin + 2 * offsetKnobX;
    const auto stereoLeft1 = stereoLeft0 + knobX + 2 * margin;

    addGroupLabel(
      stereoLeft0, mixTop0, 2 * knobX + 2 * margin, labelHeight, midTextSize, "Stereo");
    addKnob<Style::warning>(
      stereoLeft0, mixTop1, knobX, margin, uiTextSize, "Cross", ID::stereoCross);
    addKnob(stereoLeft1, mixTop1, knobX, margin, uiTextSize, "Spread", ID::stereoSpread);

    // Misc.
    const auto miscTop0 = mixTop0 + labelY + knobY;
    const auto miscTop1 = miscTop0 + labelY;

    const auto miscLeft0 = mulLeft1;
    const auto miscLeft1 = miscLeft0 + textKnobX + 2 * margin;

    addLabel(
      miscLeft0, miscTop1, textKnobX, labelHeight, uiTextSize, "Seed",
      ALIGN_CENTER | ALIGN_MIDDLE);
    addTextKnob(
      miscLeft0, miscTop1 + labelY, textKnobX, labelHeight, uiTextSize, ID::seed,
      Scales::seed);

    addKnob(
      miscLeft1 + offsetKnobX, miscTop1, knobX, margin, uiTextSize, "Smooth",
      ID::smoothness);

    // Right side.
    const auto tabViewLeft = left0 + leftPanelWidth + labelY;

    const auto tabInsideTop0 = top0;
    const auto tabInsideTop1
      = tabInsideTop0 + barboxHeight + scrollBarHeight + labelHeight;
    const auto tabInsideTop2
      = tabInsideTop1 + barboxHeight + scrollBarHeight + labelHeight;
    const auto tabInsideLeft0 = tabViewLeft;
    const auto tabInsideLeft1 = tabInsideLeft0 + labelY;

    // Base tab.
    addGroupVerticalLabel(
      tabInsideLeft0, tabInsideTop0, barboxHeight, labelHeight, midTextSize, "Time");
    auto barboxTime = addBarBox(
      tabInsideLeft1, tabInsideTop0, 2 * nDepth1, barboxHeight, ID::time0, nDepth1,
      Scales::time);
    barboxTime->liveUpdateLineEdit = false;
    addScrollBar(
      tabInsideLeft1, tabInsideTop0 + barboxHeight, 2 * nDepth1, scrollBarHeight,
      barboxTime);

    addGroupVerticalLabel(
      tabInsideLeft0, tabInsideTop1, barboxHeight, labelHeight, midTextSize, "InnerFeed");
    auto barboxInnerFeed = addBarBox(
      tabInsideLeft1, tabInsideTop1, 2 * nDepth1, barboxHeight, ID::innerFeed0, nDepth1,
      Scales::feed);
    barboxInnerFeed->sliderZero = 0.5f;
    barboxInnerFeed->liveUpdateLineEdit = false;
    addScrollBar(
      tabInsideLeft1, tabInsideTop1 + barboxHeight, 2 * nDepth1, scrollBarHeight,
      barboxInnerFeed);

    addGroupVerticalLabel(
      tabInsideLeft0, tabInsideTop2, barboxHeight, labelHeight, midTextSize, "D1 Feed");
    auto barboxD1Feed = addBarBox(
      tabInsideLeft1, tabInsideTop2, 2 * nDepth1, barboxHeight, ID::d1Feed0, nDepth1,
      Scales::feed);
    barboxD1Feed->sliderZero = 0.5f;
    barboxD1Feed->liveUpdateLineEdit = false;
    addScrollBar(
      tabInsideLeft1, tabInsideTop2 + barboxHeight, 2 * nDepth1, scrollBarHeight,
      barboxD1Feed);

    const auto tabInsideLeft2 = tabInsideLeft1 + barboxWidth + labelY;
    const auto tabInsideLeft3 = tabInsideLeft2 + labelY;

    addGroupVerticalLabel(
      tabInsideLeft2, tabInsideTop0, barboxHeight, labelHeight, midTextSize, "D2 Feed");
    auto barboxD2Feed = addBarBox(
      tabInsideLeft3, tabInsideTop0, barboxWidthSmall, barboxHeight, ID::d2Feed0, nDepth2,
      Scales::feed);
    barboxD2Feed->sliderZero = 0.5f;

    addGroupVerticalLabel(
      tabInsideLeft2, tabInsideTop1, barboxHeight, labelHeight, midTextSize, "D3 Feed");
    auto barboxD3Feed = addBarBox(
      tabInsideLeft3, tabInsideTop1, barboxWidthSmall, barboxHeight, ID::d3Feed0, nDepth3,
      Scales::feed);
    barboxD3Feed->sliderZero = 0.5f;

    addGroupVerticalLabel(
      tabInsideLeft2, tabInsideTop2, barboxHeight, labelHeight, midTextSize, "D4 Feed");
    auto barboxD4Feed = addBarBox(
      tabInsideLeft3, tabInsideTop2, barboxWidthSmall, barboxHeight, ID::d4Feed0, nDepth4,
      Scales::feed);
    barboxD4Feed->sliderZero = 0.5f;

    // Plugin name.
    const auto splashTop = defaultHeight - splashHeight - uiMargin;
    const auto splashLeft = mulLeft1;
    addSplashScreen(
      splashLeft, splashTop, 2 * textKnobX + 2 * margin, splashHeight, uiMargin, uiMargin,
      defaultWidth - splashHeight, defaultHeight - splashHeight, pluginNameTextSize,
      "L4Reverb");
  }
};

UI *createUI() { return new L4ReverbUI(); }

END_NAMESPACE_DISTRHO
