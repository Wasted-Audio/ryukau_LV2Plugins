// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of LatticeReverb.
//
// LatticeReverb is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LatticeReverb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LatticeReverb.  If not, see <https://www.gnu.org/licenses/>.

#include "../common/uibase.hpp"
#include "parameter.hpp"

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
constexpr float barboxWidth = 4 * textKnobX;
constexpr float barboxHeight = 2 * knobY;

constexpr float tabViewWidth = barboxWidth + labelY + 2 * uiMargin;
constexpr float tabViewHeight
  = labelY + 3 * barboxHeight + 2 * labelHeight + 2 * uiMargin;

constexpr float leftPanelWidth = 4 * knobX + 6 * margin + labelHeight;

constexpr uint32_t defaultWidth
  = uint32_t(leftPanelWidth + labelY + tabViewWidth + 2 * uiMargin);
constexpr uint32_t defaultHeight = uint32_t(tabViewHeight + 2 * uiMargin);

enum tabIndex { tabBase, tabOffset, tabModulation };

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

  std::string leftText = R"(- BarBox -
Ctrl + Left Drag|Reset to Default
Right Drag|Draw Line
Shift + D|Toggle Min/Mid/Max
I|Invert Value
P|Permute
R|Randomize
S|Sort Decending Order
T|Random Walk
Shift + T|Random Walk to 0
Z|Undo
Shift + Z|Redo
, (Comma)|Rotate Back
. (Period)|Rotate Forward
1|Decrease
2-9|Decrease 2n-9n

And more! Refer to the manual for full list
of shortcuts.)";

  std::string rightText = R"(- Knob -
Shift + Left Drag|Fine Adjustment
Ctrl + Left Click|Reset to Default

- Number -
Shares same controls with knob, and:
Right Click|Flip Min/Mid/Max

Changing InnerFeed or OuterFeed may
outputs loud signal.

Use Panic! button in case of blow up.

Have a nice day!)";

  const float top0 = 140.0f;
  const float mid = (defaultWidth - 2 * uiMargin) / 2;
  const float lineHeight = 20.0f;
  const float blockWidth = 160.0f;
  drawTextBlock(20.0f, top0, lineHeight, blockWidth, leftText);
  drawTextBlock(mid, top0, lineHeight, blockWidth, rightText);
}

START_NAMESPACE_DISTRHO

class LatticeReverbUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LatticeReverbUI)

public:
  LatticeReverbUI() : PluginUIBase(defaultWidth, defaultHeight)
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

    const auto top0 = uiMargin;
    const auto left0 = uiMargin;

    // Multipliers.
    const auto mulTop0 = top0;
    const auto mulTop1 = mulTop0 + labelY;
    const auto mulTop2 = mulTop1 + labelY;
    const auto mulTop3 = mulTop2 + labelY;
    const auto mulTop4 = mulTop3 + labelY;
    const auto mulLeft0 = left0;
    const auto mulLeft1 = mulLeft0 + textKnobX + 2 * margin;
    const auto mulLeft2 = mulLeft1 + textKnobX + 2 * margin;

    addGroupLabel(
      mulLeft0, mulTop0, leftPanelWidth, labelHeight, midTextSize, "Multiplier");

    addLabel(mulLeft1, mulTop1, textKnobX, labelHeight, uiTextSize, "Base");
    addLabel(mulLeft2, mulTop1, textKnobX, labelHeight, uiTextSize, "Offset");

    addLabel(
      mulLeft0, mulTop2, textKnobX, labelHeight, uiTextSize, "Time",
      ALIGN_LEFT | ALIGN_MIDDLE);
    addLabel(
      mulLeft0, mulTop3, textKnobX, labelHeight, uiTextSize, "OuterFeed",
      ALIGN_LEFT | ALIGN_MIDDLE);
    addLabel(
      mulLeft0, mulTop4, textKnobX, labelHeight, uiTextSize, "InnerFeed",
      ALIGN_LEFT | ALIGN_MIDDLE);

    addTextKnob(
      mulLeft1, mulTop2, textKnobX, labelHeight, uiTextSize, ID::timeMultiply,
      Scales::multiply, false, 4);
    addTextKnob(
      mulLeft1, mulTop3, textKnobX, labelHeight, uiTextSize, ID::outerFeedMultiply,
      Scales::multiply, false, 4);
    addTextKnob(
      mulLeft1, mulTop4, textKnobX, labelHeight, uiTextSize, ID::innerFeedMultiply,
      Scales::multiply, false, 4);
    addTextKnob(
      mulLeft2, mulTop2, textKnobX, labelHeight, uiTextSize, ID::timeOffsetMultiply,
      Scales::multiply, false, 4);
    addTextKnob(
      mulLeft2, mulTop3, textKnobX, labelHeight, uiTextSize, ID::outerFeedOffsetMultiply,
      Scales::multiply, false, 4);
    addTextKnob(
      mulLeft2, mulTop4, textKnobX, labelHeight, uiTextSize, ID::innerFeedOffsetMultiply,
      Scales::multiply, false, 4);

    // Panic button.
    auto panicButton
      = std::make_shared<PanicButton>(this, this, "Panic!", fontId, palette);
    panicButton->setSize(leftPanelWidth - 3.0f * knobX, 2 * labelHeight);
    panicButton->setAbsolutePos(left0 + 1.5f * knobX, mulTop4 + 2 * labelY);
    panicButton->setTextSize(midTextSize);
    widget.push_back(panicButton);

    // Mix.
    const auto miscTop0 = mulTop0 + 8 * labelY + 3 * margin;
    const auto miscTop1 = miscTop0 + labelY;

    const auto mixLeft0 = left0;
    const auto mixLeft1 = mixLeft0 + knobX + 2 * margin;

    addGroupLabel(
      mixLeft0, miscTop0, 2 * knobX + 2 * margin, labelHeight, midTextSize, "Mix");
    addKnob(mixLeft0, miscTop1, knobX, margin, uiTextSize, "Dry", ID::dry);
    addKnob(mixLeft1, miscTop1, knobX, margin, uiTextSize, "Wet", ID::wet);

    // Misc.
    const auto miscLeft0 = left0 + 2 * knobX + 2 * margin + labelHeight;
    const auto miscLeft1 = miscLeft0 + knobX + 2 * margin;

    addGroupLabel(
      miscLeft0, miscTop0, 2 * knobX + 2 * margin, labelHeight, midTextSize, "Stereo");
    addKnob(miscLeft0, miscTop1, knobX, margin, uiTextSize, "Cross", ID::stereoCross);
    addKnob(miscLeft1, miscTop1, knobX, margin, uiTextSize, "Spread", ID::stereoSpread);

    // Smooth.
    const auto leftSmooth = left0 + 1.5f * knobX + 3 * margin + 0.5f * labelHeight;
    addKnob(
      leftSmooth, miscTop1 + knobY + labelY, knobX, margin, uiTextSize, "Smooth",
      ID::smoothness);

    // Right side.
    const auto tabViewLeft = left0 + leftPanelWidth + labelY;

    std::vector<std::string> tabs{"Base", "Offset", "Modulation"};
    auto tabview = addTabView(
      tabViewLeft, top0, tabViewWidth, tabViewHeight, uiTextSize, labelY, tabs);

    const auto tabInsideTop0 = top0 + labelY + uiMargin;
    const auto tabInsideTop1 = tabInsideTop0 + barboxHeight + labelHeight;
    const auto tabInsideTop2 = tabInsideTop1 + barboxHeight + labelHeight;
    const auto tabInsideLeft0 = tabViewLeft + uiMargin;
    const auto tabInsideLeft1 = tabInsideLeft0 + labelY;

    // Base tab.
    tabview->addWidget(
      tabBase,
      addGroupVerticalLabel(
        tabInsideLeft0, tabInsideTop0, barboxHeight, labelHeight, midTextSize, "Time"));
    tabview->addWidget(
      tabBase,
      addBarBox(
        tabInsideLeft1, tabInsideTop0, barboxWidth, barboxHeight, ID::time0, nestingDepth,
        Scales::time));

    tabview->addWidget(
      tabBase,
      addGroupVerticalLabel(
        tabInsideLeft0, tabInsideTop1, barboxHeight, labelHeight, midTextSize,
        "OuterFeed"));
    auto barboxOuterFeed = addBarBox(
      tabInsideLeft1, tabInsideTop1, barboxWidth, barboxHeight, ID::outerFeed0,
      nestingDepth, Scales::feed);
    barboxOuterFeed->sliderZero = 0.5f;
    tabview->addWidget(tabBase, barboxOuterFeed);

    tabview->addWidget(
      tabBase,
      addGroupVerticalLabel(
        tabInsideLeft0, tabInsideTop2, barboxHeight, labelHeight, midTextSize,
        "InnerFeed"));
    auto barboxInnerFeed = addBarBox(
      tabInsideLeft1, tabInsideTop2, barboxWidth, barboxHeight, ID::innerFeed0,
      nestingDepth, Scales::feed);
    barboxInnerFeed->sliderZero = 0.5f;
    tabview->addWidget(tabBase, barboxInnerFeed);

    // Tab offset.
    tabview->addWidget(
      tabOffset,
      addGroupVerticalLabel(
        tabInsideLeft0, tabInsideTop0, barboxHeight, labelHeight, midTextSize, "Time"));
    auto barboxTimeOffset = addBarBox(
      tabInsideLeft1, tabInsideTop0, barboxWidth, barboxHeight, ID::timeOffset0,
      nestingDepth, Scales::timeOffset);
    barboxTimeOffset->sliderZero = 0.5f;
    tabview->addWidget(tabOffset, barboxTimeOffset);

    tabview->addWidget(
      tabOffset,
      addGroupVerticalLabel(
        tabInsideLeft0, tabInsideTop1, barboxHeight, labelHeight, midTextSize,
        "OuterFeed"));
    auto barboxOuterOffset = addBarBox(
      tabInsideLeft1, tabInsideTop1, barboxWidth, barboxHeight, ID::outerFeedOffset0,
      nestingDepth, Scales::feedOffset);
    barboxOuterOffset->sliderZero = 0.5f;
    tabview->addWidget(tabOffset, barboxOuterOffset);

    tabview->addWidget(
      tabOffset,
      addGroupVerticalLabel(
        tabInsideLeft0, tabInsideTop2, barboxHeight, labelHeight, midTextSize,
        "InnerFeed"));
    auto barboxInnerOffset = addBarBox(
      tabInsideLeft1, tabInsideTop2, barboxWidth, barboxHeight, ID::innerFeedOffset0,
      nestingDepth, Scales::feedOffset);
    barboxInnerOffset->sliderZero = 0.5f;
    tabview->addWidget(tabOffset, barboxInnerOffset);

    // Tab modulation
    tabview->addWidget(
      tabModulation,
      addGroupVerticalLabel(
        tabInsideLeft0, tabInsideTop0, barboxHeight, labelHeight, midTextSize,
        "Time LFO"));
    tabview->addWidget(
      tabModulation,
      addBarBox(
        tabInsideLeft1, tabInsideTop0, barboxWidth, barboxHeight, ID::timeLfoAmount0,
        nestingDepth, Scales::time));

    const auto tabViewCenter1 = tabInsideTop1 + (barboxHeight - labelHeight) / 2;
    tabview->addWidget(
      tabModulation,
      addLabel(
        tabInsideLeft0, tabViewCenter1, barboxHeight, labelHeight, uiTextSize,
        "Time LFO Cutoff"));
    tabview->addWidget(
      tabModulation,
      addTextKnob(
        tabInsideLeft0 + 2 * textKnobX, tabViewCenter1, textKnobX, labelHeight,
        uiTextSize, ID::timeLfoLowpass, Scales::timeLfoLowpas, false, 5));

    tabview->addWidget(
      tabModulation,
      addGroupVerticalLabel(
        tabInsideLeft0, tabInsideTop2, barboxHeight, labelHeight, midTextSize,
        "Lowpass Cutoff"));
    tabview->addWidget(
      tabModulation,
      addBarBox(
        tabInsideLeft1, tabInsideTop2, barboxWidth, barboxHeight, ID::lowpassCutoff0,
        nestingDepth, Scales::defaultScale));

    tabview->refreshTab();

    // Plugin name.
    const auto splashTop = defaultHeight - splashHeight - uiMargin;
    const auto splashLeft = left0 + knobX;
    addSplashScreen(
      splashLeft, splashTop, leftPanelWidth - 2 * knobX, splashHeight, uiMargin, uiMargin,
      defaultWidth - splashHeight, defaultHeight - splashHeight, pluginNameTextSize,
      "LatticeReverb");
  }
};

UI *createUI() { return new LatticeReverbUI(); }

END_NAMESPACE_DISTRHO
