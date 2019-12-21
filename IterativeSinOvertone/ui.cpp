// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019 Takamitsu Endo
//
// This file is part of IterativeSinOvertone.
//
// IterativeSinOvertone is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// IterativeSinOvertone is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with IterativeSinOvertone.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>
#include <memory>
#include <vector>

#include "ui.hpp"

#include "gui/TinosBoldItalic.hpp"
#include "gui/barbox.hpp"
#include "gui/button.hpp"
#include "gui/checkbox.hpp"
#include "gui/knob.hpp"
#include "gui/label.hpp"
#include "gui/optionmenu.hpp"
#include "gui/rotaryknob.hpp"
#include "gui/splash.hpp"
#include "gui/vslider.hpp"

START_NAMESPACE_DISTRHO

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
constexpr uint32_t defaultWidth = uint32_t(16.0f * knobX + 40);
constexpr uint32_t defaultHeight
  = uint32_t(40 + 5 * labelY + 1 * knobY + 4 * barboxY + 2 * margin) + uint32_t(knobY);

class IterativeSinOvertoneUI : public PluginUI {
public:
  IterativeSinOvertoneUI() : PluginUI(defaultWidth, defaultHeight)
  {
    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    using ID = ParameterID::ID;

    const float top0 = 20.0f;
    const float left0 = 20.0f;

    // Gain.
    const auto gainTop = top0;
    const auto gainLeft = left0;
    addGroupLabel(gainLeft, gainTop, 2.0 * knobX, "Gain");
    const auto gainKnobTop = gainTop + labelY;
    addKnob(
      gainLeft + 0.0 * knobX, gainKnobTop, knobWidth, colorBlue, "Boost", ID::gainBoost);
    addKnob(gainLeft + 1.0 * knobX, gainKnobTop, knobWidth, colorBlue, "Gain", ID::gain);

    // Pitch.
    const auto pitchTop0 = top0;
    const auto pitchLeft0 = left0 + 2.0f * knobX + 2.0f * margin;
    addGroupLabel(pitchLeft0, pitchTop0, 4.0f * knobX, "Pitch");
    addCheckbox(
      pitchLeft0 + 0.0f * knobX, pitchTop0 + labelY - 2.0f * margin, knobX,
      "Add Aliasing", ID::aliasing);

    const auto pitchTop1 = pitchTop0 + 2.0f * labelY - 3.0f * margin;
    const auto pitchTop2 = pitchTop1 + labelY;
    addLabel(pitchLeft0 + margin, pitchTop1, knobX - 2.0f * margin, "Octave");
    addTextKnob(
      pitchLeft0 + knobX, pitchTop1, knobX, colorBlue, ID::masterOctave,
      Scales::masterOctave);

    const auto pitchLeft1 = pitchLeft0 + 2.1f * knobX;
    addLabel(pitchLeft1 + margin, pitchTop1, knobX - 2.0f * margin, "Multiply");
    addTextKnob(
      pitchLeft1 + knobX, pitchTop1, knobX, colorBlue, ID::pitchMultiply,
      Scales::pitchMultiply, false, 3);

    addLabel(pitchLeft1 + margin, pitchTop2, knobX - 2.0f * margin, "Modulo");
    addTextKnob(
      pitchLeft1 + knobX, pitchTop2, knobX, colorBlue, ID::pitchModulo,
      Scales::pitchModulo, false, 3);

    // Random.
    const float randomTop0 = top0;
    const float randomLeft0 = left0 + 6.0f * knobX + 4.0f * margin;
    const float randomLeft1 = randomLeft0 + knobX;
    const float randomLeft2 = randomLeft1 + knobX;
    addGroupLabel(randomLeft0, randomTop0, 4.0f * knobX, "Random");
    addCheckbox(
      randomLeft0 + 8.0f, randomTop0 + labelY - 2.0f * margin, knobX, "Retrigger",
      ID::randomRetrigger);

    const float randomTop1 = randomTop0 + 2.0f * labelY - 3.0f * margin;
    addLabel(randomLeft0, randomTop1, knobX - 2.0f * margin, "Seed");
    addTextKnob(
      randomLeft1 - 2.0f * margin, randomTop1, knobX, colorBlue, ID::seed, Scales::seed);
    addKnob(
      randomLeft2, randomTop0 + labelY, knobWidth, colorBlue, "To Gain",
      ID::randomGainAmount);
    addKnob(
      randomLeft2 + knobX, randomTop0 + labelY, knobWidth, colorBlue, "To Pitch",
      ID::randomFrequencyAmount);

    // Misc.
    const auto miscLeft = randomLeft0 + 4.5f * knobX + 2.0f * margin;
    addKnob(miscLeft, pitchTop0, knobWidth, colorBlue, "Smooth", ID::smoothness);
    std::vector<const char *> nVoiceOptions
      = {"Mono", "2 Voices", "4 Voices", "8 Voices", "16 Voices", "32 Voices"};
    addOptionMenu(
      miscLeft - (checkboxWidth - knobWidth) / 2.0f, pitchTop0 + knobY, checkboxWidth,
      ID::nVoice, nVoiceOptions);

    // Attack.
    const float topAttack = top0 + knobY + labelY;
    const float leftAttack = left0;
    addGroupLabel(leftAttack, topAttack, 4.0f * knobX, "Attack");
    addBarBox(leftAttack, topAttack + labelY, barboxWidth, barboxHeight, ID::attack0);

    // Decay.
    const float topDecay = topAttack + barboxY + labelY;
    const float leftDecay = left0;
    addGroupLabel(leftDecay, topDecay, 4.0f * knobX, "Decay");
    addBarBox(leftDecay, topDecay + labelY, barboxWidth, barboxHeight, ID::decay0);

    // Overtone.
    const float topOvertone = topDecay + barboxY + labelY;
    const float leftOvertone = left0;
    addGroupLabel(leftOvertone, topOvertone, 4.0f * knobX, "Overtone");
    addBarBox(
      leftOvertone, topOvertone + labelY, barboxWidth, barboxHeight, ID::overtone0);

    // Phase.
    const float topPhase = topOvertone + barboxY + labelY;
    const float leftPhase = left0;
    addGroupLabel(leftPhase, topPhase, 4.0f * knobX, "Phase");
    addBarBox(leftPhase, topPhase + labelY, barboxWidth, barboxHeight, ID::phase0);

    // Plugin name.
    const auto splashTop = defaultHeight - splashHeight - 20.0f;
    const auto splashLeft = defaultWidth - 3.0f * knobX - 15.0f;
    addSplashScreen(
      splashLeft, splashTop, 3.0f * knobX, splashHeight, 20.0f, 20.0f,
      defaultWidth - splashHeight, defaultHeight - splashHeight, "IterativeSinOvertone");
  }

  void addBarBox(float left, float top, float width, float height, uint32_t id0)
  {
    std::vector<uint32_t> id(64);
    for (size_t i = 0; i < id.size(); ++i) id[i] = id0 + i;
    std::vector<double> value(id.size());
    for (size_t i = 0; i < value.size(); ++i)
      value[i] = param.value[id[i]]->getDefaultNormalized();
    std::vector<double> defaultValue(value);

    auto barBox = std::make_shared<BarBox>(this, this, id, value, defaultValue, fontId);
    barBox->setSize(width, height);
    barBox->setAbsolutePos(left, top);
    barBox->setBorderColor(colorFore);
    barBox->setValueColor(colorBlue);
    arrayWidget.push_back(barBox);
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
      repaint();
      return;
    }

    for (auto &aWidget : arrayWidget) {
      auto &idVec = aWidget->id;
      auto iter = std::find(idVec.begin(), idVec.end(), id);
      if (iter != idVec.end()) {
        aWidget->setValueAt(std::distance(idVec.begin(), iter), normalized);
        repaint();
        return;
      }
    }
  }

  void updateValue(uint32_t id, float normalized) override
  {
    setParameterValue(id, param.updateValue(id, normalized));
    repaint();
    // dumpParameter(); // Used to make preset. There may be better way to do this.
  }

  void programLoaded(uint32_t index) override
  {
    param.loadProgram(index);

    for (auto &vWidget : valueWidget)
      vWidget->setValue(param.value[vWidget->id]->getNormalized());

    for (auto &aWidget : arrayWidget) {
      for (size_t idx = 0; idx < aWidget->id.size(); ++idx) {
        aWidget->setValueAt(idx, param.value[aWidget->id[idx]]->getNormalized());
      }
    }

    repaint();
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

  FontId fontId = -1;

  std::vector<std::shared_ptr<Widget>> widget;
  std::vector<std::shared_ptr<ValueWidget>> valueWidget;
  std::vector<std::shared_ptr<ArrayWidget>> arrayWidget;

  void dumpParameter()
  {
    std::cout << "{\n";
    for (const auto &value : param.value)
      std::cout << "\"" << value->getName()
                << "\": " << std::to_string(value->getNormalized()) << ",\n";
    std::cout << "}" << std::endl;
  }

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
    if (id < param.value.size()) {
      auto defaultValue = param.value[id]->getDefaultNormalized();
      knob->setDefaultValue(defaultValue);
      knob->setValue(defaultValue);
    }
    valueWidget.push_back(knob);

    if (name != nullptr) addKnobLabel(left, top, width, height, name, labelPosition);
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
    LabelPosition labelPosition = LabelPosition::bottom)
  {
    auto height = width - 2.0f * margin;

    auto knob = std::make_shared<NumberKnob<Scale>>(this, this, fontId, scale);
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

  void addRotaryKnob(
    float left,
    float top,
    float width,
    Color highlightColor,
    const char *name,
    uint32_t id,
    LabelPosition labelPosition = LabelPosition::bottom)
  {
    auto height = width - 2.0f * margin;

    auto knob = std::make_shared<RotaryKnob>(this, this);
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
        left = left + width + 10.0;
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

  template<typename Scale>
  void addTextKnob(
    float left,
    float top,
    float width,
    Color highlightColor,
    uint32_t id,
    Scale &scale,
    bool isDecibel = false,
    uint32_t precision = 0)
  {
    auto knob = std::make_shared<TextKnob<Scale>>(this, this, fontId, scale, isDecibel);
    knob->id = id;
    knob->setSize(width, labelHeight);
    knob->setAbsolutePos(left, top);
    knob->setForegroundColor(colorFore);
    knob->setHighlightColor(highlightColor);
    auto defaultValue = param.value[id]->getDefaultNormalized();
    knob->setDefaultValue(defaultValue);
    knob->setValue(defaultValue);
    knob->setPrecision(precision);
    knob->setTextSize(uiTextSize);
    valueWidget.push_back(knob);
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
    menu->setDefaultValue(param.value[id]->getDefaultNormalized());
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

    const auto width = 70.0f;
    const auto sliderHeight = 230.0f;

    auto slider = std::make_shared<VSlider>(this, this);
    slider->id = id;
    slider->setSize(width, sliderHeight);
    slider->setAbsolutePos(left, top);
    auto defaultValue = param.value[id]->getDefaultNormalized();
    slider->setDefaultValue(defaultValue);
    slider->setValue(defaultValue);
    slider->setHighlightColor(valueColor);
    slider->setValueColor(valueColor);
    slider->setBorderColor(colorFore);
    valueWidget.push_back(slider);

    top += sliderHeight + 10.0;

    auto label = std::make_shared<Label>(this, name, fontId);
    label->setSize(width, labelHeight);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->setTextSize(uiTextSize);
    widget.push_back(label);
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IterativeSinOvertoneUI)
};

UI *createUI() { return new IterativeSinOvertoneUI(); }

END_NAMESPACE_DISTRHO
