// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of TrapezoidSynth.
//
// TrapezoidSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TrapezoidSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TrapezoidSynth.  If not, see <https://www.gnu.org/licenses/>.

#include <memory>
#include <unordered_map>
#include <vector>

#include "../common/uibase.hpp"
#include "parameter.hpp"

#include "gui/label.hpp"

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
constexpr float knobWidth = 50.0f;
constexpr float knobHeight = knobWidth - 2.0f * margin;
constexpr float knobX = knobWidth; // With margin.
constexpr float knobY = knobHeight + labelY + 2.0f * margin;
constexpr float sliderWidth = 70.0f;
constexpr float sliderHeight = 2.0f * (knobHeight + labelY) + 67.5f;
constexpr float sliderX = 80.0f;
constexpr float sliderY = sliderHeight + labelY;
constexpr float checkboxWidth = 80.0f;
constexpr uint32_t defaultWidth = uint32_t(40 + 14 * knobX);
constexpr uint32_t defaultHeight = uint32_t(20 + 5 * (knobY + labelHeight));

class TrapezoidSynthUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrapezoidSynthUI)

  auto addTpzLabel(
    int left,
    int top,
    float width,
    float height,
    float textSize,
    std::string name,
    int textAlign = ALIGN_LEFT | ALIGN_MIDDLE)
  {
    auto label = std::make_shared<TpzLabel>(this, name, fontId, palette);
    label->setSize(width, height);
    label->setAbsolutePos(left, top);
    label->drawBorder = true;
    label->setBorderWidth(2.0f);
    label->setTextSize(textSize);
    label->setTextAlign(textAlign);
    widget.push_back(label);
    return label;
  }

public:
  TrapezoidSynthUI() : PluginUIBase(defaultWidth, defaultHeight)
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

    const auto left0 = 20.0f;

    const auto top0 = 20.0f;
    addTpzLabel(left0, top0, 6.0f * knobWidth, labelHeight, midTextSize, "Oscillator 1");
    const auto top0knob = top0 + labelHeight;
    addNumberKnob(
      left0 + 0.0f * knobX, top0knob, knobWidth, margin, uiTextSize, "Semi", ID::osc1Semi,
      Scales::semi);
    addNumberKnob(
      left0 + 1.0f * knobX, top0knob, knobWidth, margin, uiTextSize, "Cent", ID::osc1Cent,
      Scales::cent);
    addKnob(
      left0 + 2.0f * knobX, top0knob, knobWidth, margin, uiTextSize, "Drift",
      ID::osc1PitchDrift);
    addKnob(
      left0 + 3.0f * knobX, top0knob, knobWidth, margin, uiTextSize, "Slope",
      ID::osc1Slope);
    addKnob(
      left0 + 4.0f * knobX, top0knob, knobWidth, margin, uiTextSize, "PW",
      ID::osc1PulseWidth);
    addKnob(
      left0 + 5.0f * knobX, top0knob, knobWidth, margin, uiTextSize, "Feedback",
      ID::osc1Feedback);

    const auto top1 = top0knob + knobY;
    addTpzLabel(left0, top1, 6.0f * knobWidth, labelHeight, midTextSize, "Oscillator 2");
    const auto top1knob = top1 + labelHeight;
    addNumberKnob(
      left0 + 0.0f * knobX, top1knob, knobWidth, margin, uiTextSize, "Semi", ID::osc2Semi,
      Scales::semi);
    addNumberKnob(
      left0 + 1.0f * knobX, top1knob, knobWidth, margin, uiTextSize, "Cent", ID::osc2Cent,
      Scales::cent);
    addNumberKnob(
      left0 + 2.0f * knobX, top1knob, knobWidth, margin, uiTextSize, "Overtone",
      ID::osc2Overtone, Scales::overtone);
    addKnob(
      left0 + 3.0f * knobX, top1knob, knobWidth, margin, uiTextSize, "Slope",
      ID::osc2Slope);
    addKnob(
      left0 + 4.0f * knobX, top1knob, knobWidth, margin, uiTextSize, "PW",
      ID::osc2PulseWidth);
    addKnob(
      left0 + 5.0f * knobX, top1knob, knobWidth, margin, uiTextSize, "PM",
      ID::pmOsc2ToOsc1);

    const auto top2 = top1knob + knobY;
    addTpzLabel(left0, top2, 6.0f * knobX, labelHeight, midTextSize, "Gain Envelope");
    auto checkBoxGainEnvRetrigger = addCheckbox(
      left0 + 1.95f * knobX, top2, checkboxWidth, labelHeight, uiTextSize, "Retrigger",
      ID::gainEnvRetrigger);
    checkBoxGainEnvRetrigger->drawBackground = true;
    const auto top2knob = top2 + labelHeight;
    addKnob(
      left0 + 0.0f * knobX, top2knob, knobWidth, margin, uiTextSize, "A", ID::gainA);
    addKnob(
      left0 + 1.0f * knobX, top2knob, knobWidth, margin, uiTextSize, "D", ID::gainD);
    addKnob(
      left0 + 2.0f * knobX, top2knob, knobWidth, margin, uiTextSize, "S", ID::gainS);
    addKnob(
      left0 + 3.0f * knobX, top2knob, knobWidth, margin, uiTextSize, "R", ID::gainR);
    addKnob(
      left0 + 4.0f * knobX, top2knob, knobWidth, margin, uiTextSize, "Curve",
      ID::gainCurve);
    addKnob(
      left0 + 5.0f * knobX, top2knob, knobWidth, margin, uiTextSize, "Gain", ID::gain);

    const auto top3 = top2knob + knobY;
    addTpzLabel(left0, top3, 6.0f * knobWidth, labelHeight, midTextSize, "Filter");
    const auto top3knob = top3 + labelHeight;
    std::vector<std::string> filterOrderItems{
      "Order 1", "Order 2", "Order 3", "Order 4",
      "Order 5", "Order 6", "Order 7", "Order 8",
    };
    addOptionMenu(
      left0 + 0.9f * knobX, top3, knobWidth, labelHeight, uiTextSize, ID::filterOrder,
      filterOrderItems);
    addKnob(
      left0 + 0.0f * knobX, top3knob, knobWidth, margin, uiTextSize, "Cut",
      ID::filterCutoff);
    addKnob(
      left0 + 1.0f * knobX, top3knob, knobWidth, margin, uiTextSize, "Res.",
      ID::filterFeedback);
    addKnob(
      left0 + 2.0f * knobX, top3knob, knobWidth, margin, uiTextSize, "Sat.",
      ID::filterSaturation);
    addKnob(
      left0 + 3.0f * knobX, top3knob, knobWidth, margin, uiTextSize, "Env>Cut",
      ID::filterEnvToCutoff);
    addKnob(
      left0 + 4.0f * knobX, top3knob, knobWidth, margin, uiTextSize, "Key>Cut",
      ID::filterKeyToCutoff);
    addKnob(
      left0 + 5.0f * knobX, top3knob, knobWidth, margin, uiTextSize, "+OscMix",
      ID::oscMixToFilterCutoff);

    const auto top4 = top3knob + knobY;
    addTpzLabel(
      left0, top4, 6.0f * knobWidth, labelHeight, midTextSize, "Filter Envelope");
    auto checkBoxFiltEnvRetrigger = addCheckbox(
      left0 + 2.0f * knobX, top4, checkboxWidth, labelHeight, uiTextSize, "Retrigger",
      ID::filterEnvRetrigger);
    checkBoxFiltEnvRetrigger->drawBackground = true;
    const auto top4knob = top4 + labelHeight;
    addKnob(
      left0 + 0.0f * knobX, top4knob, knobWidth, margin, uiTextSize, "A", ID::filterA);
    addKnob(
      left0 + 1.0f * knobX, top4knob, knobWidth, margin, uiTextSize, "D", ID::filterD);
    addKnob(
      left0 + 2.0f * knobX, top4knob, knobWidth, margin, uiTextSize, "S", ID::filterS);
    addKnob(
      left0 + 3.0f * knobX, top4knob, knobWidth, margin, uiTextSize, "R", ID::filterR);
    addKnob(
      left0 + 4.0f * knobX, top4knob, knobWidth, margin, uiTextSize, "Curve",
      ID::filterCurve);
    addNumberKnob(
      left0 + 5.0f * knobX, top4knob, knobWidth, margin, uiTextSize, ">Octave",
      ID::filterEnvToOctave, Scales::filterEnvToOctave);

    const auto left1 = left0 + 7.0f * knobX;

    addTpzLabel(left1, top0, 3.0f * knobWidth, labelHeight, midTextSize, "Misc");
    addKnob(
      left1 + 0.0f * knobX, top0knob, knobWidth, margin, uiTextSize, "OscMix",
      ID::oscMix);
    addNumberKnob(
      left1 + 1.0f * knobX, top0knob, knobWidth, margin, uiTextSize, "Octave", ID::octave,
      Scales::octave);
    addKnob(
      left1 + 2.0f * knobX, top0knob, knobWidth, margin, uiTextSize, "Smooth",
      ID::smoothness);

    addTpzLabel(
      left1 + 4.0f * knobX, top0, 3.0f * knobWidth, labelHeight, midTextSize, "Mod 1");
    auto checkBoxMod1Retrigger = addCheckbox(
      left1 + 4.95f * knobX, top0, checkboxWidth, labelHeight, uiTextSize, "Retrigger",
      ID::modEnv1Retrigger);
    checkBoxMod1Retrigger->drawBackground = true;
    addKnob(
      left1 + 4.0f * knobX, top0knob, knobWidth, margin, uiTextSize, "Attack",
      ID::modEnv1Attack);
    addKnob(
      left1 + 5.0f * knobX, top0knob, knobWidth, margin, uiTextSize, "Curve",
      ID::modEnv1Curve);
    addKnob(
      left1 + 6.0f * knobX, top0knob, knobWidth, margin, uiTextSize, ">PM",
      ID::modEnv1ToPhaseMod);

    addTpzLabel(left1, top1, 7.0f * knobWidth, labelHeight, midTextSize, "Mod 2");
    auto checkBoxMod2Retrigger = addCheckbox(
      left1 + 0.95f * knobX, top1, checkboxWidth, labelHeight, uiTextSize, "Retrigger",
      ID::modEnv2Retrigger);
    checkBoxMod2Retrigger->drawBackground = true;
    addKnob(
      left1 + 0.0f * knobX, top1knob, knobWidth, margin, uiTextSize, "Attack",
      ID::modEnv2Attack);
    addKnob(
      left1 + 1.0f * knobX, top1knob, knobWidth, margin, uiTextSize, "Curve",
      ID::modEnv2Curve);
    addKnob(
      left1 + 2.0f * knobX, top1knob, knobWidth, margin, uiTextSize, ">Feedback",
      ID::modEnv2ToFeedback);
    addKnob(
      left1 + 3.0f * knobX, top1knob, knobWidth, margin, uiTextSize, ">LFO",
      ID::modEnv2ToLFOFrequency);
    addKnob(
      left1 + 4.0f * knobX, top1knob, knobWidth, margin, uiTextSize, ">Slope2",
      ID::modEnv2ToOsc2Slope);
    addKnob(
      left1 + 5.0f * knobX, top1knob, knobWidth, margin, uiTextSize, ">Shifter1",
      ID::modEnv2ToShifter1);

    addTpzLabel(left1, top2, 3.0f * knobWidth, labelHeight, midTextSize, "Shifter 1");
    addNumberKnob(
      left1 + 0.0f * knobX, top2knob, knobWidth, margin, uiTextSize, "Semi",
      ID::shifter1Semi, Scales::shifterSemi);
    addNumberKnob(
      left1 + 1.0f * knobX, top2knob, knobWidth, margin, uiTextSize, "Cent",
      ID::shifter1Cent, Scales::shifterCent);
    addKnob(
      left1 + 2.0f * knobX, top2knob, knobWidth, margin, uiTextSize, "Gain",
      ID::shifter1Gain);

    addTpzLabel(
      left1 + 4.0f * knobX, top2, 3.0f * knobWidth, labelHeight, midTextSize,
      "Shifter 2");
    addNumberKnob(
      left1 + 4.0f * knobX, top2knob, knobWidth, margin, uiTextSize, "Semi",
      ID::shifter2Semi, Scales::shifterSemi);
    addNumberKnob(
      left1 + 5.0f * knobX, top2knob, knobWidth, margin, uiTextSize, "Cent",
      ID::shifter2Cent, Scales::shifterCent);
    addKnob(
      left1 + 6.0f * knobX, top2knob, knobWidth, margin, uiTextSize, "Gain",
      ID::shifter2Gain);

    addTpzLabel(left1, top3, 7.0f * knobWidth, labelHeight, midTextSize, "LFO");
    auto checkBoxLfoRetrigger = addCheckbox(
      left1 + floorf(0.8f * knobX), top3, floorf(1.2f * knobX), labelHeight, uiTextSize,
      "Retrig.", ID::lfoRetrigger);
    checkBoxLfoRetrigger->drawBackground = true;
    auto checkBoxTempo = addCheckbox(
      left1 + 2.0f * knobX, top3, knobX, labelHeight, uiTextSize, "Sync",
      ID::lfoTempoSync);
    checkBoxTempo->drawBackground = true;
    auto knobLfoTempoNumerator = addTextKnob(
      left1 + 3.0f * knobX, top3, knobX - 1, labelHeight, uiTextSize,
      ID::lfoTempoNumerator, Scales::lfoTempoNumerator, false, 0, 1);
    knobLfoTempoNumerator->sensitivity = 0.001;
    knobLfoTempoNumerator->lowSensitivity = 0.00025;
    auto knobLfoTempoDenominator = addTextKnob(
      left1 + 4.0f * knobX, top3, knobX - 1, labelHeight, uiTextSize,
      ID::lfoTempoDenominator, Scales::lfoTempoDenominator, false, 0, 1);
    knobLfoTempoDenominator->sensitivity = 0.001;
    knobLfoTempoNumerator->lowSensitivity = 0.00025;
    std::vector<std::string> lfoTypeItems{"Sin", "Saw", "Pulse", "Noise"};
    addOptionMenu(
      left1 + 5.0f * knobX, top3, knobX, labelHeight, uiTextSize, ID::lfoType,
      lfoTypeItems);
    addKnob(
      left1 + 0.0f * knobX, top3knob, knobWidth, margin, uiTextSize, "Freq",
      ID::lfoFrequency);
    addKnob(
      left1 + 1.0f * knobX, top3knob, knobWidth, margin, uiTextSize, "Phase",
      ID::lfoPhase);
    addKnob(
      left1 + 2.0f * knobX, top3knob, knobWidth, margin, uiTextSize, "Shape",
      ID::lfoShape);
    addKnob(
      left1 + 3.0f * knobX, top3knob, knobWidth, margin, uiTextSize, ">Pitch1",
      ID::lfoToPitch);
    addKnob(
      left1 + 4.0f * knobX, top3knob, knobWidth, margin, uiTextSize, ">Slope1",
      ID::lfoToSlope);
    addKnob(
      left1 + 5.0f * knobX, top3knob, knobWidth, margin, uiTextSize, ">PW1",
      ID::lfoToPulseWidth);
    addKnob(
      left1 + 6.0f * knobX, top3knob, knobWidth, margin, uiTextSize, ">Cut",
      ID::lfoToCutoff);

    addTpzLabel(left1, top4, 7.0f * knobWidth, labelHeight, midTextSize, "Slide");
    std::vector<std::string> pitchSlideType{"Always", "Sustain", "Reset to 0"};
    addOptionMenu(
      left1 + 0.75f * knobX, top4, 70.0f, labelHeight, uiTextSize, ID::pitchSlideType,
      pitchSlideType);
    addKnob(
      left1 + 0.0f * knobX, top4knob, knobWidth, margin, uiTextSize, "Time",
      ID::pitchSlide);
    addKnob(
      left1 + 1.0f * knobX, top4knob, knobWidth, margin, uiTextSize, "Offset",
      ID::pitchSlideOffset);

    // Plugin name.
    const auto splashWidth = 3.75f * knobX;
    const auto splashHeight = 40.0f;
    addSplashScreen(
      left1 + 2.25f * knobX, top4 + 2.0f * labelHeight, splashWidth, splashHeight, 20.0f,
      20.0f, defaultWidth - 40.0f, defaultHeight - 40.0f, pluginNameTextSize,
      "TrapezoidSynth");
  }
};

UI *createUI() { return new TrapezoidSynthUI(); }

END_NAMESPACE_DISTRHO
