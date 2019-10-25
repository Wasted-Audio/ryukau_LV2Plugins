// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019 Takamitsu Endo
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
constexpr float sliderHeight = 2.0f * (knobHeight + labelY) + 75.0f;
constexpr float sliderX = 80.0f;
constexpr float sliderY = sliderHeight + labelY;
constexpr float checkboxWidth = 80.0f;
constexpr uint32_t defaultWidth = uint32_t(2 * 20 + sliderX + 7 * knobX + 10 * margin);
constexpr uint32_t defaultHeight = uint32_t(2 * 15 + 2 * knobY + 2 * labelHeight + 55);

class WaveCymbalUI : public PluginUI {
public:
  WaveCymbalUI() : PluginUI(defaultWidth, defaultHeight)
  {
    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    using ID = ParameterID::ID;

    // Gain.
    const auto left0 = 10.0f;
    const auto top0 = 20.0f;

    const auto leftGain = left0 + 2.0f * margin;
    addVSlider(leftGain, top0, colorBlue, "Gain", ID::gain);

    // Excitation.
    const auto leftExcitation = leftGain + sliderX + 2.0f * margin;
    addButton(leftExcitation, top0, 2.0f * knobX, "Excitation", ID::excitation);

    const auto topExcitation = top0 + labelHeight + margin;
    addKnob(
      leftExcitation, topExcitation, knobWidth, colorRed, "Feedback",
      ID::pickCombFeedback);
    addKnob(
      leftExcitation + knobX, topExcitation, knobWidth, colorBlue, "Time",
      ID::pickCombTime);

    // Objects.
    const auto leftObjects = leftExcitation + 2.0f * knobX + 4.0f * margin;
    addGroupLabel(leftObjects, top0, 2.0f * knobX, "Objects");

    const auto topObjects = top0 + labelHeight + margin;
    addNumberKnob(
      leftObjects, topObjects, knobWidth, colorBlue, "nCymbal", ID::nCymbal,
      Scales::nCymbal, 1);
    addNumberKnob(
      leftObjects + knobX, topObjects, knobWidth, colorBlue, "nString", ID::stack,
      Scales::stack, 1);

    // Wave.
    const auto leftWave = leftObjects + 2.0f * knobX + 4.0f * margin;
    addGroupLabel(leftWave, top0, 3.0f * knobX, "Wave");

    const auto topWave = top0 + labelHeight + margin;
    addKnob(leftWave, topWave, knobWidth, colorRed, "Damping", ID::damping);
    addKnob(
      leftWave + knobX, topWave, knobWidth, colorBlue, "PulsePosition",
      ID::pulsePosition);
    addKnob(
      leftWave + 2.0f * knobX, topWave, knobWidth, colorBlue, "PulseWidth",
      ID::pulseWidth);


    // Collision.
    const auto top1 = top0 + knobY + 3.0f * margin;

    const auto leftCollision = leftExcitation;
    addButton(leftCollision, top1, checkboxWidth, "Collision", ID::collision);

    const auto topCollision = top1 + labelHeight + margin;
    addKnob(leftCollision, topCollision, knobWidth, colorRed, "Distance", ID::distance);

    // Random.
    const auto leftRandom = leftCollision + knobX + 4.0f * margin;
    addGroupLabel(leftRandom, top1, 2.0f * knobX, "Random");

    const auto topRandom = top1 + labelHeight + margin;
    addNumberKnob(
      leftRandom, topRandom, knobWidth, colorBlue, "Seed", ID::seed, Scales::seed, 0);
    addKnob(
      leftRandom + knobX, topRandom, knobWidth, colorRed, "Amount", ID::randomAmount);

    // String.
    const auto leftString = leftRandom + 2.0f * knobX + 4.0f * margin;
    addGroupLabel(leftString, top1, 4.0f * knobX, "String");

    const auto topString = top1 + labelHeight + margin;
    addKnob(leftString, topString, knobWidth, colorBlue, "MinHz", ID::minFrequency);
    addKnob(
      leftString + knobX, topString, knobWidth, colorBlue, "MaxHz", ID::maxFrequency);
    addKnob(
      leftString + 2.0f * knobX, topString, knobWidth, colorBlue, "Decay", ID::decay);
    addKnob(
      leftString + 3.0f * knobX, topString, knobWidth, colorBlue, "Q", ID::bandpassQ);

    // Oscillator.
    const auto top2 = top1 + labelHeight + knobY;

    addGroupLabel(leftExcitation, top2, 3.0f * knobX + 2.0f * margin, "Oscillator");

    const auto topOscillator = top2 + labelHeight + margin;
    addCheckbox(leftExcitation, topOscillator, checkboxWidth, "Retrigger", ID::retrigger);

    std::vector<const char *> itemOscType
      = {"Off", "Impulse", "Sustain", "Velvet Noise", "Brown Noise"};
    addOptionMenu(
      leftExcitation + knobX, topOscillator, checkboxWidth, ID::oscType, itemOscType);

    std::vector<const char *> itemCutoffMap = {"Log", "Linear"};
    addOptionMenu(
      leftExcitation + 2.0f * knobX + 2.0f * margin, topOscillator, checkboxWidth,
      ID::cutoffMap, itemCutoffMap);

    // Smoothness.
    const auto leftSmoothness = leftExcitation + 3.0f * knobX + 4.0f * margin;
    addKnob(
      leftSmoothness + 22.5f, top2 - margin, 50.0f, colorBlue, "Smoothness",
      ID::smoothness);

    // Plugin name.
    const auto splashWidth = 3.0f * knobX;
    addSplashScreen(
      defaultWidth - 20.0f - splashWidth, top2 + 2.0 * margin, splashWidth, 40.0f, 100.0f,
      20.0f, defaultWidth - 200.0f, defaultHeight - 40.0f, "WaveCymbal");
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

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveCymbalUI)
};

UI *createUI() { return new WaveCymbalUI(); }

END_NAMESPACE_DISTRHO
