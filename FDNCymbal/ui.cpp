// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019 Takamitsu Endo
//
// This file is part of FDNCymbal.
//
// FDNCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// FDNCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with FDNCymbal.  If not, see <https://www.gnu.org/licenses/>.

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
constexpr float knobWidth = 80.0f;
constexpr float knobHeight = knobWidth - 2.0f * margin;
constexpr float knobX = knobWidth; // With margin.
constexpr float knobY = knobHeight + labelY + 2.0f * margin;
constexpr float sliderWidth = 70.0f;
constexpr float sliderHeight = 2.0f * (knobHeight + labelY) + 67.5f;
constexpr float sliderX = 80.0f;
constexpr float sliderY = sliderHeight + labelY;
constexpr float checkboxWidth = 80.0f;
constexpr uint32_t defaultWidth = uint32_t(40 + sliderX + 7 * knobX + 6 * margin);
constexpr uint32_t defaultHeight
  = uint32_t(20 + 3 * knobY + 3 * labelHeight + 3 * margin);

class FDNCymbalUI : public PluginUI {
public:
  FDNCymbalUI() : PluginUI(defaultWidth, defaultHeight)
  {
    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    using ID = ParameterID::ID;

    // Gain.
    const auto left0 = 20.0f;
    const auto top0 = 20.0f;

    addVSlider(left0, top0, colorBlue, "Gain", ID::gain);

    // Stick.
    const auto leftStick = left0 + sliderX + 2.0f * margin;
    addButton(leftStick, top0, 2.0f * knobX, "Stick", ID::stick);

    const auto topStick = top0 + labelHeight + margin;
    addKnob(leftStick, topStick, knobWidth, colorBlue, "Decay", ID::stickDecay);
    addKnob(
      leftStick + knobX, topStick, knobWidth, colorBlue, "ToneMix", ID::stickToneMix);

    // Random.
    const auto leftRandom = leftStick + 2.0f * knobX + 2.0f * margin;
    addGroupLabel(leftRandom, top0, 2.0f * knobX, "Random");

    const auto topRandom = top0 + labelHeight;
    addNumberKnob(
      leftRandom, topRandom + margin, knobWidth, colorBlue, "Seed", ID::seed,
      Scales::seed);
    addLabel(leftRandom + knobX, topRandom, knobWidth, "Retrigger");
    addCheckbox(
      leftRandom + knobX + 2.0f * margin, topRandom + labelHeight, checkboxWidth, "Time",
      ID::retriggerTime);
    addCheckbox(
      leftRandom + knobX + 2.0f * margin, topRandom + 2.0f * labelHeight, checkboxWidth,
      "Stick", ID::retriggerStick);
    addCheckbox(
      leftRandom + knobX + 2.0f * margin, topRandom + 3.0f * labelHeight, checkboxWidth,
      "Tremolo", ID::retriggerTremolo);

    // FDN.
    const auto leftFDN = leftRandom + 2.0f * knobX + 2.0f * margin;
    addGroupLabel(leftFDN, top0, 3.0f * knobX, "FDN");

    const auto topFDN = top0 + labelHeight + margin;
    addKnob(leftFDN, topFDN, knobWidth, colorBlue, "Time", ID::fdnTime);
    addKnob(leftFDN + knobX, topFDN, knobWidth, colorRed, "Feedback", ID::fdnFeedback);
    addKnob(
      leftFDN + 2.0f * knobX, topFDN, knobWidth, colorBlue, "CascadeMix",
      ID::fdnCascadeMix);

    // Allpass.
    const auto top1 = top0 + knobY + labelHeight + margin;
    const auto leftAP = left0 + sliderX + 2.0f * margin;
    addGroupLabel(leftAP, top1, knobX, "Allpass");

    const auto topAP = top1 + labelHeight + margin;
    addKnob(leftAP, topAP, knobWidth, colorBlue, "Mix", ID::allpassMix);

    const auto leftAP1 = leftAP + knobX + 2.0f * margin;
    addGroupLabel(leftAP1, top1, 3.0f * knobX, "Stage 1");

    addKnob(leftAP1, topAP, knobWidth, colorBlue, "Time", ID::allpass1Time);
    addKnob(
      leftAP1 + knobX, topAP, knobWidth, colorBlue, "Feedback", ID::allpass1Feedback);
    addKnob(
      leftAP1 + 2.0f * knobX, topAP, knobWidth, colorRed, "HP Cutoff",
      ID::allpass1HighpassCutoff);

    const auto leftAP2 = leftAP1 + 3.0f * knobX + 2.0f * margin;
    addGroupLabel(leftAP2, top1, 3.0f * knobX, "Stage 2");

    addKnob(leftAP2, topAP, knobWidth, colorBlue, "Time", ID::allpass2Time);
    addKnob(
      leftAP2 + knobX, topAP, knobWidth, colorBlue, "Feedback", ID::allpass2Feedback);
    addKnob(
      leftAP2 + 2.0f * knobX, topAP, knobWidth, colorRed, "HP Cutoff",
      ID::allpass2HighpassCutoff);

    // Smooth.
    const auto top2 = top1 + knobY + labelHeight + margin;
    addKnob(
      left0 - margin, top2 + labelHeight + margin, sliderX, colorBlue, "Smooth",
      ID::smoothness);

    // Tremolo.
    const auto leftTremolo = left0 + sliderX + 2.0f * margin;
    addGroupLabel(leftTremolo, top2, 4.0f * knobX, "Tremolo");

    const auto topTremolo = top2 + labelHeight + margin;
    addKnob(leftTremolo, topTremolo, knobWidth, colorBlue, "Mix", ID::tremoloMix);
    addKnob(
      leftTremolo + knobX, topTremolo, knobWidth, colorBlue, "Depth", ID::tremoloDepth);
    addKnob(
      leftTremolo + 2.0f * knobX, topTremolo, knobWidth, colorBlue, "Frequency",
      ID::tremoloFrequency);
    addKnob(
      leftTremolo + 3.0f * knobX, topTremolo, knobWidth, colorBlue, "DelayTime",
      ID::tremoloDelayTime);

    const auto leftTremoloRandom = leftTremolo + 4.0f * knobX + 2.0f * margin;
    addGroupLabel(leftTremoloRandom, top2, 3.0f * knobX + 2.0f * margin, "Random");
    addKnob(
      leftTremoloRandom, topTremolo - 1.5f * margin, 50.0f, colorBlue, "Depth",
      ID::randomTremoloDepth, LabelPosition::right);
    addKnob(
      leftTremoloRandom + 1.0f * (knobX + margin), topTremolo - 1.5f * margin, 50.0f,
      colorBlue, "Freq", ID::randomTremoloFrequency, LabelPosition::right);
    addKnob(
      leftTremoloRandom + 2.0f * (knobX + margin), topTremolo - 1.5f * margin, 50.0f,
      colorBlue, "Time", ID::randomTremoloDelayTime, LabelPosition::right);

    // Plugin name.
    const auto splashWidth = 3.0f * knobX;
    const auto splashHeight = 40.0f;
    addSplashScreen(
      defaultWidth - 20.0f - splashWidth, defaultHeight - 20.0f - splashHeight,
      splashWidth, splashHeight, 20.0f, 20.0f, defaultWidth - 40.0f,
      defaultHeight - 40.0f, "FDNCymbal");
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

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FDNCymbalUI)
};

UI *createUI() { return new FDNCymbalUI(); }

END_NAMESPACE_DISTRHO
