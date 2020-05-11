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

#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

#include "../common/ui.hpp"
#include "parameter.hpp"

#include "../common/gui/TinosBoldItalic.hpp"
#include "../common/gui/barbox.hpp"
#include "../common/gui/button.hpp"
#include "../common/gui/checkbox.hpp"
#include "../common/gui/knob.hpp"
#include "../common/gui/label.hpp"
#include "../common/gui/optionmenu.hpp"
#include "../common/gui/rotaryknob.hpp"
#include "../common/gui/splash.hpp"
#include "../common/gui/tabview.hpp"
#include "../common/gui/textview.hpp"
#include "../common/gui/vslider.hpp"

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
  fillColor(backgroundColor);
  fill();
  strokeColor(isMouseEntered ? highlightColor : foregroundColor);
  strokeWidth(borderWidth);
  stroke();

  // Text.
  fillColor(foregroundColor);
  fontFaceId(fontId);
  textAlign(align);

  fontSize(textSize * 1.5f);
  std::stringstream stream;
  stream << name << " " << std::to_string(MAJOR_VERSION) << "."
         << std::to_string(MINOR_VERSION) << "." << std::to_string(PATCH_VERSION);
  text(20.0f, 50.0f, stream.str().c_str(), nullptr);

  fontSize(textSize);
  text(20.0f, 90.0f, "© 2019-2020 Takamitsu Endo (ryukau@gmail.com)", nullptr);

  text(20.0f, 200.0f, "Have a nice day!", nullptr);
}

START_NAMESPACE_DISTRHO

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

class LatticeReverbUI : public PluginUI {
protected:
  void parameterChanged(uint32_t index, float value) override
  {
    updateUI(index, param.parameterChanged(index, value));
  }

  void updateUI(uint32_t id, float normalized)
  {
    auto vWidget = valueWidget.find(id);
    if (vWidget != valueWidget.end()) {
      vWidget->second->setValue(normalized);
      repaint();
      return;
    }

    auto aWidget = arrayWidget.find(id);
    if (aWidget != arrayWidget.end()) {
      aWidget->second->setValueFromId(id, normalized);
      repaint();
      return;
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

    for (auto &vPair : valueWidget) {
      if (vPair.second->id >= ParameterID::ID_ENUM_LENGTH) continue;
      vPair.second->setValue(param.value[vPair.second->id]->getNormalized());
    }

    for (auto &aPair : arrayWidget) {
      auto &aWidget = aPair.second;
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
  std::unordered_map<int, std::shared_ptr<ValueWidget>> valueWidget;
  std::unordered_map<int, std::shared_ptr<ArrayWidget>> arrayWidget;
  std::unordered_map<std::string, std::shared_ptr<StateWidget>> stateWidget;

  void dumpParameter()
  {
    std::cout << "{\n";
    for (const auto &value : param.value)
      std::cout << "\"" << value->getName()
                << "\": " << std::to_string(value->getNormalized()) << ",\n";
    std::cout << "}" << std::endl;
  }

  template<typename Scale>
  std::shared_ptr<BarBox<Scale>> addBarBox(
    float left,
    float top,
    float width,
    float height,
    uint32_t id0,
    size_t nElement,
    Scale &scale)
  {
    std::vector<uint32_t> id(nElement);
    for (size_t i = 0; i < id.size(); ++i) id[i] = id0 + i;
    std::vector<double> value(id.size());
    for (size_t i = 0; i < value.size(); ++i)
      value[i] = param.value[id[i]]->getDefaultNormalized();
    std::vector<double> defaultValue(value);

    auto barBox = std::make_shared<BarBox<Scale>>(
      this, this, id, scale, value, defaultValue, fontId);
    barBox->setSize(width, height);
    barBox->setAbsolutePos(left, top);
    barBox->setBorderColor(colorFore);
    barBox->setValueColor(colorBlue);

    for (size_t i = 0; i < value.size(); ++i)
      arrayWidget.emplace(std::make_pair(id0 + i, barBox));
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
    valueWidget.emplace(std::make_pair(id, checkbox));
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

  enum class LabelPosition {
    top,
    left,
    bottom,
    right,
  };

  auto addKnob(
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
    valueWidget.emplace(std::make_pair(id, knob));

    auto label = addKnobLabel(left, top, width, height, name, labelPosition);
    return std::make_tuple(knob, label);
  }

  std::shared_ptr<Label> addKnobLabel(
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
    return label;
  }

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
    valueWidget.emplace(std::make_pair(id, knob));
    return knob;
  }

  std::shared_ptr<TabView> addTabView(
    float left,
    float top,
    float width,
    float hegiht,
    float tabHeight,
    Color highlightColor,
    std::vector<std::string> tabs)
  {
    auto tabview = std::make_shared<TabView>(
      this, tabs, fontId, tabHeight, left, top, width, hegiht);
    tabview->setHighlightColor(highlightColor);
    widget.push_back(tabview);
    return tabview;
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

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LatticeReverbUI)

public:
  LatticeReverbUI() : PluginUI(defaultWidth, defaultHeight)
  {
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
    const auto mulLeft0 = left0;
    const auto mulLeft1 = mulLeft0 + textKnobX + 2 * margin;
    const auto mulLeft2 = mulLeft1 + textKnobX + 2 * margin;

    addGroupLabel(mulLeft0, mulTop0, leftPanelWidth, "Multiplier");

    addLabel(mulLeft1, mulTop1, textKnobX, "Base");
    addLabel(mulLeft2, mulTop1, textKnobX, "Offset");

    addLabel(mulLeft0, mulTop2, textKnobX, "Time");
    addLabel(mulLeft0, mulTop3, textKnobX, "OuterFeed");
    addLabel(mulLeft0, mulTop4, textKnobX, "InnerFeed");

    addTextKnob(
      mulLeft1, mulTop2, textKnobX, colorBlue, ID::timeMultiply, Scales::multiply, false,
      4);
    addTextKnob(
      mulLeft1, mulTop3, textKnobX, colorBlue, ID::outerFeedMultiply, Scales::multiply,
      false, 4);
    addTextKnob(
      mulLeft1, mulTop4, textKnobX, colorBlue, ID::innerFeedMultiply, Scales::multiply,
      false, 4);
    addTextKnob(
      mulLeft2, mulTop2, textKnobX, colorBlue, ID::timeOffsetMultiply, Scales::multiply,
      false, 4);
    addTextKnob(
      mulLeft2, mulTop3, textKnobX, colorBlue, ID::outerFeedOffsetMultiply,
      Scales::multiply, false, 4);
    addTextKnob(
      mulLeft2, mulTop4, textKnobX, colorBlue, ID::innerFeedOffsetMultiply,
      Scales::multiply, false, 4);

    // Mix and Misc.
    const auto miscTop0 = mulTop0 + 6 * labelY;
    const auto miscTop1 = miscTop0 + labelY;

    const auto mixLeft0 = left0;
    const auto mixLeft1 = mixLeft0 + knobX + 2 * margin;

    addGroupLabel(mixLeft0, miscTop0, 2 * knobX + 2 * margin, "Mix");
    addKnob(mixLeft0, miscTop1, knobX, colorBlue, "Dry", ID::dry);
    addKnob(mixLeft1, miscTop1, knobX, colorBlue, "Wet", ID::wet);

    const auto miscLeft0 = left0 + 2 * knobX + 2 * margin + labelHeight;
    const auto miscLeft1 = miscLeft0 + knobX + 2 * margin;

    addGroupLabel(miscLeft0, miscTop0, 2 * knobX + 2 * margin, "Misc.");
    addKnob(miscLeft0, miscTop1, knobX, colorBlue, "Smooth", ID::smoothness);
    addKnob(miscLeft1, miscTop1, knobX, colorBlue, "StereoCross", ID::stereoCross);

    // Right side.
    const auto tabViewLeft = left0 + leftPanelWidth + labelY;

    std::vector<std::string> tabs{"Base", "Offset", "Modulation"};
    auto tabview = addTabView(
      tabViewLeft, top0, tabViewWidth, tabViewHeight, labelY, colorBlue, tabs);

    const auto tabInsideTop0 = top0 + labelY + uiMargin;
    const auto tabInsideTop1 = tabInsideTop0 + barboxHeight + labelHeight;
    const auto tabInsideTop2 = tabInsideTop1 + barboxHeight + labelHeight;
    const auto tabInsideLeft0 = tabViewLeft + uiMargin;
    const auto tabInsideLeft1 = tabInsideLeft0 + labelY;

    // Base tab.
    tabview->addWidget(
      tabBase,
      addGroupVerticalLabel(tabInsideLeft0, tabInsideTop0, barboxHeight, "Time"));
    tabview->addWidget(
      tabBase,
      addBarBox(
        tabInsideLeft1, tabInsideTop0, barboxWidth, barboxHeight, ID::time0, nestingDepth,
        Scales::time));

    tabview->addWidget(
      tabBase,
      addGroupVerticalLabel(tabInsideLeft0, tabInsideTop1, barboxHeight, "OuterFeed"));
    auto barboxOuterFeed = addBarBox(
      tabInsideLeft1, tabInsideTop1, barboxWidth, barboxHeight, ID::outerFeed0,
      nestingDepth, Scales::feed);
    barboxOuterFeed->sliderZero = 0.5f;
    tabview->addWidget(tabBase, barboxOuterFeed);

    tabview->addWidget(
      tabBase,
      addGroupVerticalLabel(tabInsideLeft0, tabInsideTop2, barboxHeight, "InnerFeed"));
    auto barboxInnerFeed = addBarBox(
      tabInsideLeft1, tabInsideTop2, barboxWidth, barboxHeight, ID::innerFeed0,
      nestingDepth, Scales::feed);
    barboxInnerFeed->sliderZero = 0.5f;
    tabview->addWidget(tabBase, barboxInnerFeed);

    // Tab offset.
    tabview->addWidget(
      tabOffset,
      addGroupVerticalLabel(tabInsideLeft0, tabInsideTop0, barboxHeight, "Time"));
    auto barboxTimeOffset = addBarBox(
      tabInsideLeft1, tabInsideTop0, barboxWidth, barboxHeight, ID::timeOffset0,
      nestingDepth, Scales::timeOffset);
    barboxTimeOffset->sliderZero = 0.5f;
    tabview->addWidget(tabOffset, barboxTimeOffset);

    tabview->addWidget(
      tabOffset,
      addGroupVerticalLabel(tabInsideLeft0, tabInsideTop1, barboxHeight, "OuterFeed"));
    auto barboxOuterOffset = addBarBox(
      tabInsideLeft1, tabInsideTop1, barboxWidth, barboxHeight, ID::outerFeedOffset0,
      nestingDepth, Scales::feedOffset);
    barboxOuterOffset->sliderZero = 0.5f;
    tabview->addWidget(tabOffset, barboxOuterOffset);

    tabview->addWidget(
      tabOffset,
      addGroupVerticalLabel(tabInsideLeft0, tabInsideTop2, barboxHeight, "InnerFeed"));
    auto barboxInnerOffset = addBarBox(
      tabInsideLeft1, tabInsideTop2, barboxWidth, barboxHeight, ID::innerFeedOffset0,
      nestingDepth, Scales::feedOffset);
    barboxInnerOffset->sliderZero = 0.5f;
    tabview->addWidget(tabOffset, barboxInnerOffset);

    // Tab modulation
    tabview->addWidget(
      tabModulation,
      addGroupVerticalLabel(tabInsideLeft0, tabInsideTop0, barboxHeight, "Time LFO"));
    tabview->addWidget(
      tabModulation,
      addBarBox(
        tabInsideLeft1, tabInsideTop0, barboxWidth, barboxHeight, ID::timeLfoAmount0,
        nestingDepth, Scales::time));

    const auto tabViewCenter1 = tabInsideTop1 + (barboxHeight - labelHeight) / 2;
    tabview->addWidget(
      tabModulation,
      addLabel(tabInsideLeft0, tabViewCenter1, barboxHeight, "Time LFO Cutoff"));
    tabview->addWidget(
      tabModulation,
      addTextKnob(
        tabInsideLeft0 + 2 * textKnobX, tabViewCenter1, textKnobX, colorBlue,
        ID::timeLfoLowpass, Scales::defaultScale, false, 4));

    tabview->addWidget(
      tabModulation,
      addGroupVerticalLabel(
        tabInsideLeft0, tabInsideTop2, barboxHeight, "Lowpass Cutoff"));
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
      defaultWidth - splashHeight, defaultHeight - splashHeight, "LatticeReverb");
  }
};

UI *createUI() { return new LatticeReverbUI(); }

END_NAMESPACE_DISTRHO
