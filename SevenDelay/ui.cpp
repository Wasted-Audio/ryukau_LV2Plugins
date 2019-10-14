// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019 Takamitsu Endo
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

#include <memory>
#include <unordered_map>
#include <vector>

#include "ui.hpp"

#include "gui/button.hpp"
#include "gui/checkbox.hpp"
#include "gui/knob.hpp"
#include "gui/label.hpp"
#include "gui/splash.hpp"
#include "gui/vslider.hpp"
#include "gui/waveview.hpp"

START_NAMESPACE_DISTRHO

constexpr uint32_t defaultWidth = 960;
constexpr uint32_t defaultHeight = 330;

class SevenDelayUI : public PluginUI {
public:
  SevenDelayUI() : PluginUI(defaultWidth, defaultHeight)
  {
    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

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
    addCheckbox(delayLeft + 10.0f, delayTop2, "Sync", ParameterID::tempoSync);
    addCheckbox(delayLeft + 10.0f, delayTop3, "Negative", ParameterID::negativeFeedback);

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
      defaultHeight - 40.0f);
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
    waveView->shape = param.value[ParameterID::lfoShape]->getRaw();
    waveView->phase = param.value[ParameterID::lfoInitialPhase]->getRaw();
  }

  void updateValue(uint32_t id, float normalized) override
  {
    setParameterValue(id, param.updateValue(id, normalized));
    updateWaveView();
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

  std::vector<std::shared_ptr<Widget>> widget;
  std::vector<std::shared_ptr<ValueWidget>> valueWidget;
  std::shared_ptr<WaveView> waveView;

  void addButton(float left, float top, float width, const char *title, uint32_t id)
  {
    auto button = std::make_shared<ToggleButton>(this, this, title);
    button->id = id;
    button->setSize(width, 30.0f);
    button->setAbsolutePos(left, top);
    button->setForegroundColor(colorFore);
    button->setHighlightColor(colorOrange);
    button->setTextSize(20.0f);
    valueWidget.push_back(button);
  }

  void addCheckbox(float left, float top, const char *title, uint32_t id)
  {
    auto width = 100.0f;
    auto height = 30.0f;

    auto checkbox = std::make_shared<CheckBox>(this, this, title);
    checkbox->id = id;
    checkbox->setSize(width, height);
    checkbox->setAbsolutePos(left, top);
    checkbox->setForegroundColor(colorFore);
    checkbox->setHighlightColor(colorBlue);
    checkbox->setTextSize(16.0f);
    valueWidget.push_back(checkbox);
  }

  void addGroupLabel(int left, int top, float width, const char *name)
  {
    auto height = 30.0f;

    auto label = std::make_shared<Label>(this, name);
    label->setSize(width, height);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->drawUnderline = true;
    label->setBorderWidth(2.0f);
    label->setTextSize(20.0f);
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
    auto knob = std::make_shared<Knob>(this, this);
    knob->id = id;
    knob->setSize(width, width);
    knob->setAbsolutePos(left, top);
    knob->setHighlightColor(highlightColor);
    auto defaultValue = param.value[id]->getDefaultNormalized();
    knob->setDefaultValue(defaultValue);
    knob->setValue(defaultValue);
    valueWidget.push_back(knob);

    float height = width;
    switch (labelPosition) {
      default:
      case LabelPosition::bottom:
        top = top + width;
        height = 30.0f;
        break;

      case LabelPosition::right:
        left = left + width + 10.0;
        width *= 2.0f;
        break;
    }

    auto label = std::make_shared<Label>(this, name);
    label->setSize(width, height);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->setTextSize(16.0f);
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
    float splashHeight)
  {
    auto button = std::make_shared<SplashButton>(this, "SevenDelay");
    button->setSize(buttonWidth, buttonHeight);
    button->setAbsolutePos(buttonLeft, buttonTop);
    button->setForegroundColor(colorFore);
    button->setHighlightColor(colorOrange);
    button->setTextSize(28.0f);
    widget.push_back(button);

    auto credit = std::make_shared<CreditSplash>(this);
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
    const auto labelHeight = 30.0f;

    auto label = std::make_shared<Label>(this, name);
    label->setSize(width, labelHeight);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->setTextSize(16.0f);
    widget.push_back(label);
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SevenDelayUI)
};

UI *createUI() { return new SevenDelayUI(); }

END_NAMESPACE_DISTRHO
