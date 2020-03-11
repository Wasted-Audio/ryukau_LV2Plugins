// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of SevenDelay.
//
// SevenDelay is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SevenDelay is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SevenDelay.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../common/ui.hpp"
#include "parameter.hpp"

#include "../common/gui/TinosBoldItalic.hpp"
#include "../common/gui/button.hpp"
#include "../common/gui/checkbox.hpp"
#include "../common/gui/knob.hpp"
#include "../common/gui/label.hpp"
#include "../common/gui/splash.hpp"
#include "../common/gui/vslider.hpp"
#include "gui/waveview.hpp"

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
  stream << "SevenDelay " << std::to_string(MAJOR_VERSION) << "."
         << std::to_string(MINOR_VERSION) << "." << std::to_string(PATCH_VERSION);
  text(20.0f, 50.0f, stream.str().c_str(), nullptr);

  fontSize(textSize);
  text(20.0f, 90.0f, "© 2019-2020 Takamitsu Endo (ryukau@gmail.com)", nullptr);
  text(20.0f, 150.0f, "Shift + Drag: Fine Adjustment", nullptr);
  text(20.0f, 180.0f, "Ctrl + Click: Reset to Default", nullptr);
  text(20.0f, 240.0f, "Have a nice day!", nullptr);
}

START_NAMESPACE_DISTRHO

constexpr uint32_t defaultWidth = 960;
constexpr uint32_t defaultHeight = 330;
constexpr float pluginNameTextSize = 28.0f;
constexpr float labelHeight = 30.0f;
constexpr float midTextSize = 20.0f;
constexpr float uiTextSize = 16.0f;
constexpr float checkboxWidth = 100.0f;
constexpr float margin = 0.0f;

class SevenDelayUI : public PluginUI {
public:
  SevenDelayUI() : PluginUI(defaultWidth, defaultHeight)
  {
    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    const auto normalWidth = 80.0f;
    const auto normalHeight = normalWidth + 40.0f;
    const auto smallWidth = 40.0f;
    const auto smallHeight = 50.0f;
    const auto interval = 100.0f;

    // Delay.
    const auto delayTop1 = 50.0f;
    const auto delayLeft = 20.0f;
    addGroupLabel(delayLeft, 10.0f, 480.0f, "Delay");
    addKnob(delayLeft, delayTop1, normalWidth, colorBlue, "Time", ParameterID::time);
    addKnob(
      1.0f * interval + delayLeft, delayTop1, normalWidth, colorBlue, "Feedback",
      ParameterID::feedback);
    addKnob(
      2.0f * interval + delayLeft, delayTop1, normalWidth, colorBlue, "Stereo",
      ParameterID::offset);
    addKnob(
      3.0f * interval + delayLeft, delayTop1, normalWidth, colorBlue, "Wet",
      ParameterID::wetMix);
    addKnob(
      4.0f * interval + delayLeft, delayTop1, normalWidth, colorGreen, "Dry",
      ParameterID::dryMix);

    const auto delayTop2 = delayTop1 + normalHeight;
    const auto delayTop3 = delayTop2 + smallHeight;
    const auto delayTop4 = delayTop3 + smallHeight;
    addCheckbox(
      delayLeft + 10.0f, delayTop2, checkboxWidth, "Sync", ParameterID::tempoSync);
    addCheckbox(
      delayLeft + 10.0f, delayTop3, checkboxWidth, "Negative",
      ParameterID::negativeFeedback);

    addKnob(
      1.0f * interval + delayLeft, delayTop2, smallWidth, colorBlue, "In Spread",
      ParameterID::inSpread, LabelPosition::right);
    addKnob(
      1.0f * interval + delayLeft, delayTop3, smallWidth, colorBlue, "Out Spread",
      ParameterID::outSpread, LabelPosition::right);
    addKnob(
      2.3f * interval + delayLeft, delayTop2, smallWidth, colorBlue, "In Pan",
      ParameterID::inPan, LabelPosition::right);
    addKnob(
      2.3f * interval + delayLeft, delayTop3, smallWidth, colorBlue, "Out Pan",
      ParameterID::outPan, LabelPosition::right);
    addKnob(
      2.3f * interval + delayLeft, delayTop4, smallWidth, colorBlue, "DC Kill",
      ParameterID::dckill, LabelPosition::right);

    addKnob(
      3.6f * interval + delayLeft, delayTop2, smallWidth, colorBlue, "Allpass Cut",
      ParameterID::toneCutoff, LabelPosition::right);
    addKnob(
      3.6f * interval + delayLeft, delayTop3, smallWidth, colorBlue, "Allpass Q",
      ParameterID::toneQ, LabelPosition::right);
    addKnob(
      3.6f * interval + delayLeft, delayTop4, smallWidth, colorBlue, "Smooth",
      ParameterID::smoothness, LabelPosition::right);

    // LFO.
    // 750 - 520 = 230 / 3 = 66 + 10
    const auto lfoLeft1 = 520.0f;
    addGroupLabel(520.0f, 10.0f, 420.0f, "LFO");
    addVSlider(lfoLeft1, 50.0f, colorBlue, "To Time", ParameterID::lfoTimeAmount);
    addVSlider(
      lfoLeft1 + 75.0f, 50.0f, colorBlue, "To Allpass", ParameterID::lfoToneAmount);
    addVSlider(
      lfoLeft1 + 150.0f, 50.0f, colorGreen, "Frequency", ParameterID::lfoFrequency);
    const auto lfoLeft2 = lfoLeft1 + 230.0f;
    addKnob(lfoLeft2, 50.0f, normalWidth, colorBlue, "Shape", ParameterID::lfoShape);
    addKnob(
      interval + lfoLeft2, 50.0f, normalWidth, colorBlue, "Phase",
      ParameterID::lfoInitialPhase);

    const auto waveViewLeft = 760.0f;
    const auto waveViewTop = 170.0f;
    const auto waveViewWidth = 180.0f;
    const auto waveViewHeight = 110.0f;
    addButton(
      waveViewLeft, waveViewTop + waveViewHeight + 10.0f, waveViewWidth, "LFO Hold",
      ParameterID::lfoHold);

    waveView = std::make_shared<WaveView>(this);
    waveView->setSize(waveViewWidth, waveViewHeight);
    waveView->setAbsolutePos(waveViewLeft, waveViewTop);

    // Plugin name.
    const auto nameLeft = delayLeft;
    const auto nameTop = defaultHeight - 50.0f;
    const auto nameWidth = 180.0f;
    addSplashScreen(
      nameLeft, nameTop, nameWidth, 40.0f, 200.0f, 20.0f, defaultWidth - 400.0f,
      defaultHeight - 40.0f, "SevenDelay");
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
    updateWaveView();
    repaint();
  }

  void updateWaveView()
  {
    waveView->shape = param.value[ParameterID::lfoShape]->getInt();
    waveView->phase = param.value[ParameterID::lfoInitialPhase]->getInt();
  }

  void updateValue(uint32_t id, float normalized) override
  {
    setParameterValue(id, param.updateValue(id, normalized));
    updateWaveView();
    repaint();
    // dumpParameter(); // Used to make preset. There may be better way to do this.
  }

  void updateState(std::string /* key */, std::string /* value */) {}

  void programLoaded(uint32_t index) override
  {
    param.loadProgram(index);
    for (auto &vWidget : valueWidget)
      vWidget->setValue(param.value[vWidget->id]->getNormalized());
    updateWaveView();
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
  std::shared_ptr<WaveView> waveView;

  void dumpParameter()
  {
    std::cout << "{\n";
    for (const auto &value : param.value) {
      const auto val = dynamic_cast<IntValue *>(value.get());
      if (val == nullptr) {
        std::cout << "\"" << value->getName()
                  << "\": " << std::to_string(value->getNormalized()) << ",\n";
      } else {
        std::cout << "\"" << value->getName()
                  << "\": " << std::to_string(uint32_t(value->getInt())) << ",\n";
      }
    }
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
  }

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

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SevenDelayUI)
};

UI *createUI() { return new SevenDelayUI(); }

END_NAMESPACE_DISTRHO
