// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_NestedLongAllpass.
//
// CV_NestedLongAllpass is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_NestedLongAllpass is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_NestedLongAllpass.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

#include "../../common/ui.hpp"
#include "parameter.hpp"

#include "../../common/gui/TinosBoldItalic.hpp"
#include "../../common/gui/barbox.hpp"
#include "../../common/gui/button.hpp"
#include "../../common/gui/checkbox.hpp"
#include "../../common/gui/knob.hpp"
#include "../../common/gui/label.hpp"
#include "../../common/gui/optionmenu.hpp"
#include "../../common/gui/rotaryknob.hpp"
#include "../../common/gui/tabview.hpp"
#include "../../common/gui/textview.hpp"
#include "../../common/gui/vslider.hpp"

START_NAMESPACE_DISTRHO

constexpr float uiTextSize = 14.0f;
constexpr float midTextSize = 16.0f;
constexpr float pluginNameTextSize = 22.0f;
constexpr float margin = 5.0f;
constexpr float labelHeight = 20.0f;
constexpr float labelY = 30.0f;
constexpr float knobWidth = 50.0f;
constexpr float knobHeight = 40.0f;
constexpr float knobX = 80.0f; // With margin.
constexpr float knobY = knobHeight + labelY;
constexpr float barboxWidth = 4 * knobX;
constexpr float barboxHeight = 2 * knobY;
constexpr uint32_t defaultWidth
  = uint32_t(labelY + barboxWidth + knobX + 2 * margin + 30);
constexpr uint32_t defaultHeight
  = uint32_t(labelY + 3 * barboxHeight + 2 * labelHeight + 6 * margin + 30);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class CV_NestedLongAllpassUI : public PluginUI {
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
    if (id >= ParameterID::ID_ENUM_LENGTH) return;
    setParameterValue(id, param.updateValue(id, normalized));
    repaint();
    // dumpParameter(); // Used to make preset. There may be better way to do this.
  }

  void updateState(std::string /* key */, std::string /* value */)
  {
    // setState(key.c_str(), value.c_str());
  }

  void programLoaded(uint32_t index) override
  {
    param.loadProgram(index);

    for (auto &vWidget : valueWidget) {
      if (vWidget->id >= ParameterID::ID_ENUM_LENGTH) continue;
      vWidget->setValue(param.value[vWidget->id]->getNormalized());
    }

    for (auto &aWidget : arrayWidget) {
      for (size_t idx = 0; idx < aWidget->id.size(); ++idx) {
        if (aWidget->id[idx] >= ParameterID::ID_ENUM_LENGTH) continue;
        aWidget->setValueAt(idx, param.value[aWidget->id[idx]]->getNormalized());
      }
    }

    repaint();
  }

  void stateChanged(const char * /* key */, const char * /* value */)
  {
    // This method is required by DPF.
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
  std::vector<std::shared_ptr<ArrayWidget>> arrayWidget;
  std::vector<std::shared_ptr<StateWidget>> stateWidget;

  void dumpParameter()
  {
    std::cout << "{\n";
    for (const auto &value : param.value)
      std::cout << "\"" << value->getName()
                << "\": " << std::to_string(value->getNormalized()) << ",\n";
    std::cout << "}" << std::endl;
  }

  std::shared_ptr<BarBox> addBarBox(
    float left, float top, float width, float height, uint32_t id0, size_t nElement)
  {
    std::vector<uint32_t> id(nElement);
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
    return barBox;
  }

  std::shared_ptr<CheckBox>
  addCheckbox(float left, float top, float width, const char *title, uint32_t id)
  {
    auto checkbox = std::make_shared<CheckBox>(this, this, title, fontId);
    checkbox->id = id;
    checkbox->setSize(width, labelHeight);
    checkbox->setAbsolutePos(left, top);
    checkbox->setForegroundColor(colorFore);
    checkbox->setHighlightColor(colorBlue);
    checkbox->setTextSize(uiTextSize);
    valueWidget.push_back(checkbox);
    return checkbox;
  }

  std::shared_ptr<Label> addLabel(
    int left,
    int top,
    float width,
    std::string name,
    int textAlign = ALIGN_CENTER | ALIGN_MIDDLE)
  {
    auto label = std::make_shared<Label>(this, name, fontId);
    label->setSize(width, labelHeight);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->drawBorder = false;
    label->setTextSize(uiTextSize);
    label->setTextAlign(textAlign);
    widget.push_back(label);
    return label;
  }

  std::shared_ptr<Label> addGroupLabel(
    int left,
    int top,
    float width,
    std::string name,
    int textAlign = ALIGN_CENTER | ALIGN_MIDDLE)
  {
    auto label = std::make_shared<Label>(this, name, fontId);
    label->setSize(width, labelHeight);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->drawBorder = true;
    label->setBorderWidth(2.0f);
    label->setTextSize(midTextSize);
    label->setTextAlign(textAlign);
    widget.push_back(label);
    return label;
  }

  std::shared_ptr<VLabel> addGroupVerticalLabel(
    int left,
    int top,
    float width,
    std::string name,
    int textAlign = ALIGN_CENTER | ALIGN_MIDDLE)
  {
    auto label = std::make_shared<VLabel>(this, name, fontId);
    label->setSize(width, labelHeight);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->drawBorder = false;
    label->setBorderWidth(2.0f);
    label->setTextSize(midTextSize);
    label->setTextAlign(textAlign);
    widget.push_back(label);
    return label;
  };

  template<typename Scale>
  std::shared_ptr<TextKnob<Scale>> addTextKnob(
    float left,
    float top,
    float width,
    Color highlightColor,
    uint32_t id,
    Scale &scale,
    bool isDecibel = false,
    uint32_t precision = 0,
    int32_t offset = 0)
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
    knob->offset = offset;
    knob->setTextSize(uiTextSize);
    valueWidget.push_back(knob);
    return knob;
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_NestedLongAllpassUI)

public:
  CV_NestedLongAllpassUI() : PluginUI(defaultWidth, defaultHeight)
  {
    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    using ID = ParameterID::ID;

    const auto top0 = 15.0f;
    const auto left0 = 15.0f;
    const auto uiWidth = labelY + barboxWidth + knobX + 2 * margin;

    addGroupLabel(left0, top0, uiWidth, "CV_NestedLongAllpass");

    const auto top1 = 15.0f + labelY;
    const auto top2 = top1 + barboxHeight + 2 * margin;
    const auto top3 = top2 + barboxHeight + 2 * margin;
    const auto top4 = top3 + barboxHeight + 2 * margin;
    const auto left1 = 15.0f + labelY;
    const auto left2 = left1 + barboxWidth + 2 * margin;

    addGroupVerticalLabel(left0, top1, barboxHeight, "Time [s]");
    addBarBox(left1, top1, barboxWidth, barboxHeight, ID::time0, 8);

    addLabel(left2, top1, knobX, "Multiply");
    addTextKnob(
      left2, top1 + labelY, knobX, colorBlue, ID::timeMultiply, Scales::multiply, false,
      4);

    addGroupVerticalLabel(left0, top2, barboxHeight, "OuterFeed");
    auto barboxOuterFeed
      = addBarBox(left1, top2, barboxWidth, barboxHeight, ID::outerFeed0, 8);
    barboxOuterFeed->drawCenterLine = true;

    addLabel(left2, top2, knobX, "Multiply");
    addTextKnob(
      left2, top2 + labelY, knobX, colorBlue, ID::outerFeedMultiply, Scales::multiply,
      false, 4);

    addGroupVerticalLabel(left0, top3, barboxHeight, "InnerFeed");
    auto barboxInnerFeed
      = addBarBox(left1, top3, barboxWidth, barboxHeight, ID::innerFeed0, 8);
    barboxInnerFeed->drawCenterLine = true;

    addLabel(left2, top3, knobX, "Multiply");
    addTextKnob(
      left2, top3 + labelY, knobX, colorBlue, ID::innerFeedMultiply, Scales::multiply,
      false, 4);

    addLabel(left1, top4, knobX, "Smoothness");
    addTextKnob(
      left1 + knobX, top4, knobX, colorBlue, ID::smoothness, Scales::smoothness, false,
      4);
  }
};

UI *createUI() { return new CV_NestedLongAllpassUI(); }

END_NAMESPACE_DISTRHO
