// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019 Takamitsu Endo
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

#include "ui.hpp"

#include "gui/TinosBoldItalic.hpp"
#include "gui/button.hpp"
#include "gui/checkbox.hpp"
#include "gui/knob.hpp"
#include "gui/label.hpp"
#include "gui/numberknob.hpp"
#include "gui/optionmenu.hpp"
#include "gui/splash.hpp"
#include "gui/vslider.hpp"

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
constexpr uint32_t defaultWidth = uint32_t(40 + 13 * knobX);
constexpr uint32_t defaultHeight = uint32_t(20 + 5 * (knobY + labelHeight));

class TrapezoidSynthUI : public PluginUI {
public:
  TrapezoidSynthUI() : PluginUI(defaultWidth, defaultHeight)
  {
    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    using ID = ParameterID::ID;

    const auto left0 = 20.0f;

    const auto top0 = 20.0f;
    addGroupLabel(left0, top0, 6.0f * knobWidth, "Oscillator 1");
    const auto top0knob = top0 + labelHeight;
    addNumberKnob(
      left0 + 0.0f * knobX, top0knob, knobWidth, colorBlue, "Semi", ID::osc1Semi,
      Scales::semi);
    addNumberKnob(
      left0 + 1.0f * knobX, top0knob, knobWidth, colorBlue, "Cent", ID::osc1Cent,
      Scales::cent);
    addKnob(
      left0 + 2.0f * knobX, top0knob, knobWidth, colorBlue, "Drift", ID::osc1PitchDrift);
    addKnob(left0 + 3.0f * knobX, top0knob, knobWidth, colorBlue, "Slope", ID::osc1Slope);
    addKnob(
      left0 + 4.0f * knobX, top0knob, knobWidth, colorBlue, "PW", ID::osc1PulseWidth);
    addKnob(
      left0 + 5.0f * knobX, top0knob, knobWidth, colorBlue, "Feedback", ID::osc1Feedback);

    const auto top1 = top0knob + knobY;
    addGroupLabel(left0, top1, 6.0f * knobWidth, "Oscillator 2");
    const auto top1knob = top1 + labelHeight;
    addNumberKnob(
      left0 + 0.0f * knobX, top1knob, knobWidth, colorBlue, "Semi", ID::osc2Semi,
      Scales::semi);
    addNumberKnob(
      left0 + 1.0f * knobX, top1knob, knobWidth, colorBlue, "Cent", ID::osc2Cent,
      Scales::cent);
    addNumberKnob(
      left0 + 2.0f * knobX, top1knob, knobWidth, colorBlue, "Overtone", ID::osc2Overtone,
      Scales::overtone);
    addKnob(left0 + 3.0f * knobX, top1knob, knobWidth, colorBlue, "Slope", ID::osc2Slope);
    addKnob(
      left0 + 4.0f * knobX, top1knob, knobWidth, colorBlue, "PW", ID::osc2PulseWidth);
    addKnob(left0 + 5.0f * knobX, top1knob, knobWidth, colorBlue, "PM", ID::pmOsc2ToOsc1);

    const auto top2 = top1knob + knobY;
    addGroupLabel(left0, top2, 6.0f * knobX, "Gain Envelope");
    addCheckbox(
      left0 + 1.95f * knobX, top2, checkboxWidth, "Retrigger", ID::gainEnvRetrigger);
    const auto top2knob = top2 + labelHeight;
    addKnob(left0 + 0.0f * knobX, top2knob, knobWidth, colorBlue, "A", ID::gainA);
    addKnob(left0 + 1.0f * knobX, top2knob, knobWidth, colorBlue, "D", ID::gainD);
    addKnob(left0 + 2.0f * knobX, top2knob, knobWidth, colorBlue, "S", ID::gainS);
    addKnob(left0 + 3.0f * knobX, top2knob, knobWidth, colorBlue, "R", ID::gainR);
    addKnob(left0 + 4.0f * knobX, top2knob, knobWidth, colorBlue, "Curve", ID::gainCurve);
    addKnob(left0 + 5.0f * knobX, top2knob, knobWidth, colorBlue, "Gain", ID::gain);

    const auto top3 = top2knob + knobY;
    addGroupLabel(left0, top3, 6.0f * knobWidth, "Filter");
    const auto top3knob = top3 + labelHeight;
    std::vector<const char *> filterOrderItems{
      "Order 1", "Order 2", "Order 3", "Order 4",
      "Order 5", "Order 6", "Order 7", "Order 8",
    };
    addOptionMenu(
      left0 + 0.9f * knobX, top3, knobWidth, ID::filterOrder, filterOrderItems);
    addKnob(
      left0 + 0.0f * knobX, top3knob, knobWidth, colorBlue, "Cut", ID::filterCutoff);
    addKnob(
      left0 + 1.0f * knobX, top3knob, knobWidth, colorBlue, "Res.", ID::filterFeedback);
    addKnob(
      left0 + 2.0f * knobX, top3knob, knobWidth, colorBlue, "Sat.", ID::filterSaturation);
    addKnob(
      left0 + 3.0f * knobX, top3knob, knobWidth, colorBlue, "Env>Cut",
      ID::filterEnvToCutoff);
    addKnob(
      left0 + 4.0f * knobX, top3knob, knobWidth, colorBlue, "Key>Cut",
      ID::filterKeyToCutoff);
    addKnob(
      left0 + 5.0f * knobX, top3knob, knobWidth, colorBlue, "+OscMix",
      ID::oscMixToFilterCutoff);

    const auto top4 = top3knob + knobY;
    addGroupLabel(left0, top4, 6.0f * knobWidth, "Filter Envelope");
    addCheckbox(
      left0 + 2.0f * knobX, top4, checkboxWidth, "Retrigger", ID::filterEnvRetrigger);
    const auto top4knob = top4 + labelHeight;
    addKnob(left0 + 0.0f * knobX, top4knob, knobWidth, colorBlue, "A", ID::filterA);
    addKnob(left0 + 1.0f * knobX, top4knob, knobWidth, colorBlue, "D", ID::filterD);
    addKnob(left0 + 2.0f * knobX, top4knob, knobWidth, colorBlue, "S", ID::filterS);
    addKnob(left0 + 3.0f * knobX, top4knob, knobWidth, colorBlue, "R", ID::filterR);
    addKnob(
      left0 + 4.0f * knobX, top4knob, knobWidth, colorBlue, "Curve", ID::filterCurve);
    addNumberKnob(
      left0 + 5.0f * knobX, top4knob, knobWidth, colorBlue, ">Octave",
      ID::filterEnvToOctave, Scales::filterEnvToOctave);

    const auto left1 = left0 + 7.0f * knobX;

    addGroupLabel(left1, top0, 3.0f * knobWidth, "Misc");
    addKnob(left1 + 0.0f * knobX, top0knob, knobWidth, colorBlue, "OscMix", ID::oscMix);
    addNumberKnob(
      left1 + 1.0f * knobX, top0knob, knobWidth, colorBlue, "Octave", ID::octave,
      Scales::octave);
    addKnob(
      left1 + 2.0f * knobX, top0knob, knobWidth, colorBlue, "Smooth", ID::smoothness);

    addGroupLabel(left1 + 3.0f * knobX, top0, 3.0f * knobWidth, "Mod 1");
    addCheckbox(
      left1 + 3.95f * knobX, top0, checkboxWidth, "Retrigger", ID::modEnv1Retrigger);
    addKnob(
      left1 + 3.0f * knobX, top0knob, knobWidth, colorBlue, "Attack", ID::modEnv1Attack);
    addKnob(
      left1 + 4.0f * knobX, top0knob, knobWidth, colorBlue, "Curve", ID::modEnv1Curve);
    addKnob(
      left1 + 5.0f * knobX, top0knob, knobWidth, colorBlue, ">PM", ID::modEnv1ToPhaseMod);

    addGroupLabel(left1, top1, 6.0f * knobWidth, "Mod 2");
    addCheckbox(
      left1 + 0.95f * knobX, top1, checkboxWidth, "Retrigger", ID::modEnv2Retrigger);
    addKnob(
      left1 + 0.0f * knobX, top1knob, knobWidth, colorBlue, "Attack", ID::modEnv2Attack);
    addKnob(
      left1 + 1.0f * knobX, top1knob, knobWidth, colorBlue, "Curve", ID::modEnv2Curve);
    addKnob(
      left1 + 2.0f * knobX, top1knob, knobWidth, colorBlue, ">Feedback",
      ID::modEnv2ToFeedback);
    addKnob(
      left1 + 3.0f * knobX, top1knob, knobWidth, colorBlue, ">LFO",
      ID::modEnv2ToLFOFrequency);
    addKnob(
      left1 + 4.0f * knobX, top1knob, knobWidth, colorBlue, ">Slope2",
      ID::modEnv2ToOsc2Slope);
    addKnob(
      left1 + 5.0f * knobX, top1knob, knobWidth, colorBlue, ">Shifter1",
      ID::modEnv2ToShifter1);

    addGroupLabel(left1, top2, 3.0f * knobWidth, "Shifter 1");
    addNumberKnob(
      left1 + 0.0f * knobX, top2knob, knobWidth, colorBlue, "Semi", ID::shifter1Semi,
      Scales::shifterSemi);
    addNumberKnob(
      left1 + 1.0f * knobX, top2knob, knobWidth, colorBlue, "Cent", ID::shifter1Cent,
      Scales::shifterCent);
    addKnob(
      left1 + 2.0f * knobX, top2knob, knobWidth, colorBlue, "Gain", ID::shifter1Gain);

    addGroupLabel(left1 + 3.0f * knobX, top2, 3.0f * knobWidth, "Shifter 2");
    addNumberKnob(
      left1 + 3.0f * knobX, top2knob, knobWidth, colorBlue, "Semi", ID::shifter2Semi,
      Scales::shifterSemi);
    addNumberKnob(
      left1 + 4.0f * knobX, top2knob, knobWidth, colorBlue, "Cent", ID::shifter2Cent,
      Scales::shifterCent);
    addKnob(
      left1 + 5.0f * knobX, top2knob, knobWidth, colorBlue, "Gain", ID::shifter2Gain);

    addGroupLabel(left1, top3, 6.0f * knobWidth, "LFO");
    std::vector<const char *> lfoTypeItems{"Sin", "Saw", "Pulse", "Noise"};
    addOptionMenu(left1 + 0.8f * knobX, top3, knobWidth, ID::lfoType, lfoTypeItems);
    addCheckbox(left1 + 2.2f * knobX, top3, 55.0f, "Tempo", ID::lfoTempoSync);
    addKnob(
      left1 + 0.0f * knobX, top3knob, knobWidth, colorBlue, "Freq", ID::lfoFrequency);
    addKnob(left1 + 1.0f * knobX, top3knob, knobWidth, colorBlue, "Shape", ID::lfoShape);
    addKnob(
      left1 + 2.0f * knobX, top3knob, knobWidth, colorBlue, ">Pitch1", ID::lfoToPitch);
    addKnob(
      left1 + 3.0f * knobX, top3knob, knobWidth, colorBlue, ">Slope1", ID::lfoToSlope);
    addKnob(
      left1 + 4.0f * knobX, top3knob, knobWidth, colorBlue, ">PW1", ID::lfoToPulseWidth);
    addKnob(
      left1 + 5.0f * knobX, top3knob, knobWidth, colorBlue, ">Cut", ID::lfoToCutoff);

    addGroupLabel(left1, top4, 6.0f * knobWidth, "Slide");
    std::vector<const char *> pitchSlideType{"Always", "Sustain", "Reset to 0"};
    addOptionMenu(left1 + 0.75f * knobX, top4, 70.0f, ID::pitchSlideType, pitchSlideType);
    addKnob(left1 + 0.0f * knobX, top4knob, knobWidth, colorBlue, "Time", ID::pitchSlide);
    addKnob(
      left1 + 1.0f * knobX, top4knob, knobWidth, colorBlue, "Offset",
      ID::pitchSlideOffset);

    // Plugin name.
    const auto splashWidth = 3.75f * knobX;
    const auto splashHeight = 40.0f;
    addSplashScreen(
      left1 + 2.25f * knobX, top4 + 1.5f * labelHeight, splashWidth, splashHeight, 20.0f,
      20.0f, defaultWidth - 40.0f, defaultHeight - 40.0f, "TrapezoidSynth");
  }

protected:
  void parameterChanged(uint32_t index, float value) override
  {
    updateUI(index, param.parameterChanged(index, value));
  }

  void updateUI(uint32_t id, float normalized)
  {
    for (auto &vWidget : valueWidget) {
      if (vWidget->id != id) continue;
      vWidget->setValue(normalized);
      break;
    }
    repaint();
  }

  void updateValue(uint32_t id, float normalized) override
  {
    setParameterValue(id, param.updateValue(id, normalized));
    repaint();
  }

  void programLoaded(uint32_t index) override
  {
    switch (index) {
      case 0:
        break;

        // Add program here.
    }
  }

  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(colorBack);
    fill();
  }

private:
  GlobalParameter param;

  Color colorBack{255, 255, 255};
  Color colorFore{0, 0, 0};
  Color colorInactive{237, 237, 237};
  Color colorBlue{11, 164, 241};
  Color colorGreen{19, 193, 54};
  Color colorOrange{252, 192, 79};
  Color colorRed{252, 128, 128};

  FontId fontId = -1;

  std::vector<std::shared_ptr<Widget>> widget;
  std::vector<std::shared_ptr<ValueWidget>> valueWidget;

  void addButton(float left, float top, float width, const char *title, uint32_t id)
  {
    auto button = std::make_shared<ToggleButton>(this, this, title, fontId);
    button->id = id;
    button->setSize(width, labelHeight);
    button->setAbsolutePos(left, top);
    button->setForegroundColor(colorFore);
    button->setHighlightColor(colorOrange);
    button->setTextSize(midTextSize);
    valueWidget.push_back(button);
  }

  void addCheckbox(float left, float top, float width, const char *title, uint32_t id)
  {
    auto checkbox = std::make_shared<CheckBox>(this, this, title, fontId);
    checkbox->id = id;
    checkbox->setSize(width, labelHeight);
    checkbox->setAbsolutePos(left, top);
    checkbox->setForegroundColor(colorFore);
    checkbox->setHighlightColor(colorBlue);
    checkbox->setTextSize(uiTextSize);
    valueWidget.push_back(checkbox);
  }

  void addGroupLabel(int left, int top, float width, const char *name)
  {
    auto label = std::make_shared<Label>(this, name, fontId);
    label->setSize(width, labelHeight);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->drawBorder = true;
    label->setBorderWidth(2.0f);
    label->setTextSize(midTextSize);
    label->setTextAlign(ALIGN_LEFT | ALIGN_MIDDLE);
    widget.push_back(label);
  };

  enum class LabelPosition {
    top,
    left,
    bottom,
    right,
  };

  void addKnob(
    float left,
    float top,
    float width,
    Color highlightColor,
    const char *name,
    uint32_t id,
    LabelPosition labelPosition = LabelPosition::bottom)
  {
    auto height = width - 2.0f * margin;

    auto knob = std::make_shared<Knob>(this, this);
    knob->id = id;
    knob->setSize(width - 2.0f * margin, height);
    knob->setAbsolutePos(left + margin, top + margin);
    knob->setHighlightColor(highlightColor);
    auto defaultValue = param.value[id]->getDefaultNormalized();
    knob->setDefaultValue(defaultValue);
    knob->setValue(defaultValue);
    valueWidget.push_back(knob);

    addKnobLabel(left, top, width, height, name, labelPosition);
  }

  template<typename Scale>
  void addNumberKnob(
    float left,
    float top,
    float width,
    Color highlightColor,
    const char *name,
    uint32_t id,
    Scale &scale,
    uint32_t offset = 0,
    LabelPosition labelPosition = LabelPosition::bottom)
  {
    auto height = width - 2.0f * margin;

    auto knob = std::make_shared<NumberKnob<Scale>>(this, this, fontId, scale, offset);
    knob->id = id;
    knob->setSize(width - 2.0f * margin, height);
    knob->setAbsolutePos(left + margin, top + margin);
    knob->setHighlightColor(highlightColor);
    auto defaultValue = param.value[id]->getDefaultNormalized();
    knob->setDefaultValue(defaultValue);
    knob->setValue(defaultValue);
    valueWidget.push_back(knob);

    addKnobLabel(left, top, width, height, name, labelPosition);
  }

  void addKnobLabel(
    float left,
    float top,
    float width,
    float height,
    const char *name,
    LabelPosition labelPosition)
  {
    switch (labelPosition) {
      default:
      case LabelPosition::bottom:
        top = top + height;
        height = 30.0f;
        break;

      case LabelPosition::right:
        height = width;
        left = left + width;
        width *= 2.0f;
        break;
    }

    auto label = std::make_shared<Label>(this, name, fontId);
    label->setSize(width, height);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->setTextSize(uiTextSize);
    if (labelPosition == LabelPosition::right)
      label->setTextAlign(ALIGN_LEFT | ALIGN_MIDDLE);
    widget.push_back(label);
  }

  void addLabel(int left, int top, float width, const char *name)
  {
    auto label = std::make_shared<Label>(this, name, fontId);
    label->setSize(width, labelHeight);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->drawBorder = false;
    label->setTextSize(uiTextSize);
    widget.push_back(label);
  };

  void addOptionMenu(
    float left,
    float top,
    float width,
    uint32_t id,
    const std::vector<const char *> &items)
  {
    auto menu = std::make_shared<OptionMenu>(this, this, items, fontId);
    menu->id = id;
    menu->setSize(width, labelHeight);
    menu->setAbsolutePos(left, top);
    auto defaultValue = param.value[id]->getDefaultInt();
    menu->setDefaultValue(defaultValue);
    menu->setForegroundColor(colorFore);
    menu->setHighlightColor(colorBlue);
    menu->setTextSize(uiTextSize);
    valueWidget.push_back(menu);
  }

  void addSplashScreen(
    float buttonLeft,
    float buttonTop,
    float buttonWidth,
    float buttonHeight,
    float splashLeft,
    float splashTop,
    float splashWidth,
    float splashHeight,
    const char *name)
  {
    auto button = std::make_shared<SplashButton>(this, name, fontId);
    button->setSize(buttonWidth, buttonHeight);
    button->setAbsolutePos(buttonLeft, buttonTop);
    button->setForegroundColor(colorFore);
    button->setHighlightColor(colorOrange);
    button->setTextSize(pluginNameTextSize);
    widget.push_back(button);

    auto credit = std::make_shared<CreditSplash>(this, name, fontId);
    credit->setSize(splashWidth, splashHeight);
    credit->setAbsolutePos(splashLeft, splashTop);
    button->setSplashWidget(credit);
    widget.push_back(credit);
  }

  void addVSlider(float left, float top, Color valueColor, const char *name, uint32_t id)
  {
    // width, height = 100, 270.

    auto slider = std::make_shared<VSlider>(this, this);
    slider->id = id;
    slider->setSize(sliderWidth, sliderHeight);
    slider->setAbsolutePos(left, top);
    auto defaultValue = param.value[id]->getDefaultNormalized();
    slider->setDefaultValue(defaultValue);
    slider->setValue(defaultValue);
    slider->setHighlightColor(valueColor);
    slider->setValueColor(valueColor);
    slider->setBorderColor(colorFore);
    valueWidget.push_back(slider);

    top += sliderHeight + margin;

    auto label = std::make_shared<Label>(this, name, fontId);
    label->setSize(sliderWidth, labelHeight);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->setTextSize(uiTextSize);
    widget.push_back(label);
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrapezoidSynthUI)
};

UI *createUI() { return new TrapezoidSynthUI(); }

END_NAMESPACE_DISTRHO
