// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
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

#include "../common/uibase.hpp"
#include "parameter.hpp"

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
constexpr float barboxWidth = 12.0f * knobX;
constexpr float barboxHeight = 2.0f * knobY;
constexpr float barboxY = barboxHeight + 2.0f * margin;
constexpr float checkboxWidth = 60.0f;
constexpr float splashHeight = 40.0f;
constexpr uint32_t defaultWidth
  = uint32_t(barboxWidth + 2 * knobX + labelY + 4 * margin + 40);
constexpr uint32_t defaultHeight
  = uint32_t(40 + 2 * labelY + knobY + 4 * barboxY + 2 * margin);

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

  std::string leftText = R"(- Overtone -
Ctrl + Left Drag|Reset to Default
Shift + Left Drag|Skip Between Frames
Right Drag|Draw Line
D|Reset to Default
Shift + D|Toggle Min/Mid/Max
E|Emphasize Low
Shift + E|Emphasize High
F|Low-pass Filter
Shift + F|High-pass Filter
I|Invert
Shift + I|Full Invert
N|Normalize (Preserve Min)
Shift + N|Normalize
P|Permute
R|Randomize
Shift + R|Sparse Randomize
S|Sort Decending Order
Shift + S|Sort Ascending Order
T|Random Walk
Shift + T|Random Walk to 0
Z|Undo
Shift + Z|Redo
, (Comma)|Rotate Back
. (Period)|Rotate Forward
1|Decrease
2-9|Decrease 2n-9n)";

  std::string rightText = R"(- Knob -
Shift + Left Drag|Fine Adjustment
Ctrl + Left Click|Reset to Default

- Number -
Shares same controls with knob, and:
Right Click|Toggle Min/Mid/Max

Have a nice day!)";

  const float top0 = 140.0f;
  const float mid = (defaultWidth - 2 * 20) / 2;
  const float lineHeight = 20.0f;
  const float blockWidth = 160.0f;
  drawTextBlock(20.0f, top0, lineHeight, blockWidth, leftText);
  drawTextBlock(mid, top0, lineHeight, blockWidth, rightText);
}

START_NAMESPACE_DISTRHO

class EnvelopedSineUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopedSineUI)

public:
  EnvelopedSineUI() : PluginUIBase(defaultWidth, defaultHeight)
  {
    param = std::make_unique<GlobalParameter>();

    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    using ID = ParameterID::ID;

    const auto top0 = 20.0f;
    const auto left0 = 20.0f;

    // Gain.
    const auto gainTop = top0;
    const auto gainLeft = left0;
    addGroupLabel(gainLeft, gainTop, 2.0 * knobX, labelHeight, midTextSize, "Gain");
    const auto gainKnobTop = gainTop + labelY;
    addKnob(
      gainLeft + 0.0 * knobX, gainKnobTop, knobWidth, margin, uiTextSize, "Boost",
      ID::gainBoost);
    addKnob(
      gainLeft + 1.0 * knobX, gainKnobTop, knobWidth, margin, uiTextSize, "Gain",
      ID::gain);

    // Pitch.
    const auto pitchTop0 = gainTop + labelY + knobY;
    const auto pitchLeft0 = left0;
    addGroupLabel(pitchLeft0, pitchTop0, 2.0f * knobX, labelHeight, midTextSize, "Pitch");
    addCheckbox(
      pitchLeft0, pitchTop0 + labelY - 2.0f * margin, knobX, labelHeight, uiTextSize,
      "Add Aliasing", ID::aliasing);

    const auto pitchLabelWidth = knobX - 2.0f * margin;
    const auto pitchLeft1 = pitchLeft0 + margin;
    const auto pitchLeft2 = pitchLeft0 + knobX;

    const auto pitchTop1 = pitchTop0 + 2.0f * labelY - 3.0f * margin;
    addLabel(pitchLeft1, pitchTop1, pitchLabelWidth, labelHeight, uiTextSize, "Octave");
    addTextKnob(
      pitchLeft2, pitchTop1, knobX, labelHeight, uiTextSize, ID::masterOctave,
      Scales::masterOctave);

    const auto pitchTop2 = pitchTop1 + labelY;
    addLabel(pitchLeft1, pitchTop2, pitchLabelWidth, labelHeight, uiTextSize, "Multiply");
    addTextKnob(
      pitchLeft2, pitchTop2, knobX, labelHeight, uiTextSize, ID::pitchMultiply,
      Scales::pitchMultiply, false, 3);

    const auto pitchTop3 = pitchTop2 + labelY;
    addLabel(pitchLeft1, pitchTop3, pitchLabelWidth, labelHeight, uiTextSize, "Modulo");
    addTextKnob(
      pitchLeft2, pitchTop3, knobX, labelHeight, uiTextSize, ID::pitchModulo,
      Scales::pitchModulo, false, 3);

    const auto pitchTop4 = pitchTop3 + labelY;
    addLabel(pitchLeft1, pitchTop4, pitchLabelWidth, labelHeight, uiTextSize, "Expand");
    addTextKnob(
      pitchLeft2, pitchTop4, knobX, labelHeight, uiTextSize, ID::overtoneExpand,
      Scales::overtoneExpand, false, 3);

    const auto pitchTop5 = pitchTop4 + labelY;
    addLabel(pitchLeft1, pitchTop5, pitchLabelWidth, labelHeight, uiTextSize, "Shift");
    addTextKnob(
      pitchLeft2, pitchTop5, knobX, labelHeight, uiTextSize, ID::overtoneShift,
      Scales::overtoneShift, false, 3);

    // Random.
    const auto randomTop0 = pitchTop0 + labelHeight + 6.0 * labelY - margin;
    const auto randomLeft0 = left0;
    const auto randomLeft1 = randomLeft0 + knobX;
    addGroupLabel(
      randomLeft0, randomTop0, 2.0f * knobX, labelHeight, midTextSize, "Random");
    addCheckbox(
      randomLeft0, randomTop0 + labelY - 2.0f * margin, knobX, labelHeight, uiTextSize,
      "Retrigger", ID::randomRetrigger);

    const auto randomTop1 = randomTop0 + 2.0f * labelY - 3.0f * margin;
    addLabel(
      randomLeft0, randomTop1, knobX - 2.0f * margin, labelHeight, uiTextSize, "Seed");
    addTextKnob(
      randomLeft1 - 2.0f * margin, randomTop1, knobX, labelHeight, uiTextSize, ID::seed,
      Scales::seed);

    const auto randomTop2 = randomTop1 + labelY;
    addKnob(
      randomLeft0, randomTop2, knobWidth, margin, uiTextSize, "To Gain", ID::randomGain);
    addKnob(
      randomLeft1, randomTop2, knobWidth, margin, uiTextSize, "To Pitch",
      ID::randomFrequency);

    const auto randomTop3 = randomTop2 + knobY;
    addKnob(
      randomLeft0, randomTop3, knobWidth, margin, uiTextSize, "To Attack",
      ID::randomAttack);
    addKnob(
      randomLeft1, randomTop3, knobWidth, margin, uiTextSize, "To Decay",
      ID::randomDecay);

    const auto randomTop4 = randomTop3 + knobY;
    addKnob(
      randomLeft0, randomTop4, knobWidth, margin, uiTextSize, "To Sat.",
      ID::randomSaturation);
    addKnob(
      randomLeft1, randomTop4, knobWidth, margin, uiTextSize, "To Phase",
      ID::randomPhase);

    // Misc.
    const auto miscTop = randomTop4 + knobY;
    const auto miscLeft = left0;
    addGroupLabel(miscLeft, miscTop, 2.0f * knobX, labelHeight, midTextSize, "Misc.");

    addKnob(
      miscLeft, miscTop + labelY, knobWidth, margin, uiTextSize, "Smooth",
      ID::smoothness);

    const auto miscLeft0 = miscLeft + knobX - (checkboxWidth - knobWidth) / 2.0f;
    const auto miscTop0 = miscTop + labelY;
    std::vector<std::string> nVoiceOptions
      = {"Mono", "2 Voices", "4 Voices", "8 Voices", "16 Voices", "32 Voices"};
    addOptionMenu(
      miscLeft0, miscTop0, checkboxWidth, labelHeight, uiTextSize, ID::nVoice,
      nVoiceOptions);
    addCheckbox(
      miscLeft0, miscTop0 + labelY, checkboxWidth, labelHeight, uiTextSize, "Unison",
      ID::unison);

    // Modifier.
    const auto modTop = top0 + 4.0f * (barboxY + margin);
    const auto modLeft = left0 + 2.0f * knobX + 4.0f * margin + labelY;
    addGroupLabel(modLeft, modTop, 4.0f * knobX, labelHeight, midTextSize, "Modifier");

    const auto modTop0 = modTop + labelY;
    addKnob(
      modLeft, modTop0, knobWidth, margin, uiTextSize, "Attack*", ID::attackMultiplier);
    addKnob(
      modLeft + knobX, modTop0, knobWidth, margin, uiTextSize, "Decay*",
      ID::decayMultiplier);
    addKnob(
      modLeft + 2.0f * knobX, modTop0, knobWidth, margin, uiTextSize, "Gain^",
      ID::gainPower);
    addKnob(
      modLeft + 3.0f * knobX, modTop0, knobWidth, margin, uiTextSize, "Sat. Mix",
      ID::saturationMix);

    const auto modTop1 = modTop0 + knobY;
    addCheckbox(
      modLeft + 0.4f * knobX, modTop1, checkboxWidth, labelHeight, uiTextSize, "Declick",
      ID::declick);

    // Phaser.
    const auto phaserTop = modTop;
    const auto phaserLeft = modLeft + 4.0f * knobX + 4.0f * margin;
    addGroupLabel(
      phaserLeft, phaserTop, 7.0f * knobX + labelY, labelHeight, midTextSize, "Phaser");

    const auto phaserTop0 = phaserTop + labelY;
    addKnob(phaserLeft, phaserTop0, knobWidth, margin, uiTextSize, "Mix", ID::phaserMix);
    addKnob(
      phaserLeft + knobX, phaserTop0, knobWidth, margin, uiTextSize, "Freq",
      ID::phaserFrequency);
    addKnob<Style::warning>(
      phaserLeft + 2.0f * knobX, phaserTop0, knobWidth, margin, uiTextSize, "Feedback",
      ID::phaserFeedback); // Warning
    addKnob(
      phaserLeft + 3.0f * knobX, phaserTop0, knobWidth, margin, uiTextSize, "Range",
      ID::phaserRange);
    addKnob(
      phaserLeft + 4.0f * knobX, phaserTop0, knobWidth, margin, uiTextSize, "Min",
      ID::phaserMin);
    addKnob(
      phaserLeft + 5.0f * knobX, phaserTop0, knobWidth, margin, uiTextSize, "Offset",
      ID::phaserOffset);
    addRotaryKnob(
      phaserLeft + 6.0f * knobX, phaserTop0, knobWidth + labelY, margin, uiTextSize,
      "Phase", ID::phaserPhase);

    const auto phaserTop1 = phaserTop0 + knobY;
    std::vector<std::string> phaserStageItems{
      "Stage 1",  "Stage 2",  "Stage 3",  "Stage 4",  "Stage 5",  "Stage 6",
      "Stage 7",  "Stage 8",  "Stage 9",  "Stage 10", "Stage 11", "Stage 12",
      "Stage 13", "Stage 14", "Stage 15", "Stage 16"};
    addOptionMenu(
      phaserLeft - margin, phaserTop1, knobX, labelHeight, uiTextSize, ID::phaserStage,
      phaserStageItems);

    // Attack.
    const auto attackTop = top0;
    const auto attackLeft = left0 + 2.0f * knobX + 4.0f * margin;
    addGroupVerticalLabel(
      attackLeft, attackTop, barboxHeight, labelHeight, midTextSize, "Attack");

    const auto attackLeft0 = attackLeft + labelY;
    addBarBox(
      attackLeft0, attackTop, barboxWidth, barboxHeight, ID::attack0, nOvertone,
      Scales::envelopeA);

    // Decay.
    const auto decayTop = attackTop + barboxY + margin;
    const auto decayLeft = attackLeft;
    addGroupVerticalLabel(
      decayLeft, decayTop, barboxHeight, labelHeight, midTextSize, "Decay");

    const auto decayLeft0 = decayLeft + labelY;
    addBarBox(
      decayLeft0, decayTop, barboxWidth, barboxHeight, ID::decay0, nOvertone,
      Scales::envelopeD);

    // Overtone.
    const auto overtoneTop = decayTop + barboxY + margin;
    const auto overtoneLeft = attackLeft;
    addGroupVerticalLabel(
      overtoneLeft, overtoneTop, barboxHeight, labelHeight, midTextSize, "Gain");

    const auto overtoneLeft0 = overtoneLeft + labelY;
    addBarBox(
      overtoneLeft0, overtoneTop, barboxWidth, barboxHeight, ID::overtone0, nOvertone,
      Scales::gainDecibel);

    // Saturation.
    const auto saturationTop = overtoneTop + barboxY + margin;
    const auto saturationLeft = attackLeft;
    addGroupVerticalLabel(
      saturationLeft, saturationTop, barboxHeight, labelHeight, midTextSize,
      "Saturation");

    const auto saturationLeft0 = saturationLeft + labelY;
    addBarBox(
      saturationLeft0, saturationTop, barboxWidth, barboxHeight, ID::saturation0,
      nOvertone, Scales::saturation);

    // Plugin name.
    const auto splashTop = defaultHeight - splashHeight - 20.0f;
    const auto splashLeft = left0;
    addSplashScreen(
      splashLeft, splashTop, 2.5f * knobX, splashHeight, 20.0f, 20.0f,
      defaultWidth - splashHeight, defaultHeight - splashHeight, pluginNameTextSize,
      "EnvelopedSine");
  }
};

UI *createUI() { return new EnvelopedSineUI(); }

END_NAMESPACE_DISTRHO
