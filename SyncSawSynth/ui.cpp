// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of SyncSawSynth.
//
// SyncSawSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SyncSawSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SyncSawSynth.  If not, see <https://www.gnu.org/licenses/>.

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
#include "../common/gui/optionmenu.hpp"
#include "../common/gui/splash.hpp"
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
constexpr float knobHeight = 40.0f;
constexpr float knobX = 60.0f; // With margin.
constexpr float knobY = knobHeight + labelY;
constexpr float checkboxWidth = 60.0f;
constexpr uint32_t defaultWidth = uint32_t(10.0 * knobX + 50.0);
constexpr uint32_t defaultHeight = uint32_t(20.0 + 3.0 * labelY + 6.0 * knobY);

class SyncSawSynthUI : public PluginUI {
public:
  SyncSawSynthUI() : PluginUI(defaultWidth, defaultHeight)
  {
    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    // Oscillators.
    const auto oscWidth = 2.0 * knobWidth + 4.0 * margin;
    const auto oscTop = 10.0;
    const auto oscLeft1 = 10.0;
    addOscillatorSection(
      "Osc1", oscLeft1, oscTop, ParameterID::osc1Gain, ParameterID::osc1Semi,
      ParameterID::osc1Cent, ParameterID::osc1Sync, ParameterID::osc1SyncType,
      ParameterID::osc1PTROrder, ParameterID::osc1Phase, ParameterID::osc1PhaseLock);

    const auto oscLeft2 = oscLeft1 + oscWidth + 10.0;
    addOscillatorSection(
      "Osc2", oscLeft2, oscTop, ParameterID::osc2Gain, ParameterID::osc2Semi,
      ParameterID::osc2Cent, ParameterID::osc2Sync, ParameterID::osc2SyncType,
      ParameterID::osc2PTROrder, ParameterID::osc2Phase, ParameterID::osc2PhaseLock);

    std::vector<std::string> nVoiceOptions
      = {"Mono", "2 Voices", "4 Voices", "8 Voices", "16 Voices", "32 Voices"};
    addOptionMenu(
      oscLeft2 - knobX / 2.0, oscTop + labelY, checkboxWidth, ParameterID::nVoice,
      nVoiceOptions);
    addCheckbox(
      oscLeft2 - knobX / 2.0, oscTop + 2.0 * labelY, checkboxWidth, "Unison",
      ParameterID::unison);

    const auto oscTop2 = 4.0 * labelY + 2.0 * knobY + 2.0 * knobHeight - margin;
    addCheckbox(
      oscLeft2 + margin + knobX,
      oscTop2 + labelY + (knobHeight - labelHeight) / 2.0 - 10.0, checkboxWidth, "Invert",
      ParameterID::osc2Invert);

    // Cross modulation.
    const auto crossTop = oscTop2 + knobY;
    const auto crossLeft = oscLeft1;
    addGroupLabel(crossLeft, crossTop, 2.0 * oscWidth + 10.0, "Modulation");

    const auto crossTop2 = crossTop + labelY;
    const auto crossKnobLeft = margin + crossLeft;
    addKnob(
      crossKnobLeft, crossTop2, 1.5 * knobWidth, colorBlue, "Osc1->Sync1",
      ParameterID::fmOsc1ToSync1);
    addKnob(
      crossKnobLeft + 1.5 * knobWidth + 10.0, crossTop2, 1.5 * knobWidth, colorBlue,
      "Osc1->Freq2", ParameterID::fmOsc1ToFreq2);
    addKnob(
      crossKnobLeft + 3.0 * knobWidth + 20.0, crossTop2, 1.5 * knobWidth, colorBlue,
      "Osc2->Sync1", ParameterID::fmOsc2ToSync1);

    // Modulation envelope and LFO.
    const auto modTop = oscTop;
    const auto modLeft = oscLeft2 + oscWidth + 20.0;
    addGroupLabel(modLeft, modTop, 6.0 * knobX, "Modulation");

    const auto modTop1 = modTop + labelY;
    addKnob(modLeft, modTop1, knobWidth, colorBlue, "Attack", ParameterID::modEnvelopeA);
    addKnob(
      modLeft + 1.0 * knobX, modTop1, knobWidth, colorBlue, "Curve",
      ParameterID::modEnvelopeCurve);
    addKnob(
      modLeft + 2.0 * knobX, modTop1, knobWidth, colorBlue, "To Freq1",
      ParameterID::modEnvelopeToFreq1);
    addKnob(
      modLeft + 3.0 * knobX, modTop1, knobWidth, colorBlue, "To Sync1",
      ParameterID::modEnvelopeToSync1);
    addKnob(
      modLeft + 4.0 * knobX, modTop1, knobWidth, colorBlue, "To Freq2",
      ParameterID::modEnvelopeToFreq2);
    addKnob(
      modLeft + 5.0 * knobX, modTop1, knobWidth, colorBlue, "To Sync2",
      ParameterID::modEnvelopeToSync2);

    const auto modTop2 = modTop1 + knobY;
    addKnob(modLeft, modTop2, knobWidth, colorBlue, "LFO", ParameterID::modLFOFrequency);
    addKnob(
      modLeft + 1.0 * knobX, modTop2, knobWidth, colorBlue, "NoiseMix",
      ParameterID::modLFONoiseMix);
    addKnob(
      modLeft + 2.0 * knobX, modTop2, knobWidth, colorBlue, "To Freq1",
      ParameterID::modLFOToFreq1);
    addKnob(
      modLeft + 3.0 * knobX, modTop2, knobWidth, colorBlue, "To Sync1",
      ParameterID::modLFOToSync1);
    addKnob(
      modLeft + 4.0 * knobX, modTop2, knobWidth, colorBlue, "To Freq2",
      ParameterID::modLFOToFreq2);
    addKnob(
      modLeft + 5.0 * knobX, modTop2, knobWidth, colorBlue, "To Sync2",
      ParameterID::modLFOToSync2);

    // Gain.
    const auto gainTop = modTop2 + knobY + margin;
    const auto gainLeft = modLeft;
    addGroupLabel(gainLeft, gainTop, 6.0 * knobX, "Gain");

    const auto gainKnobTop = gainTop + labelY;
    addKnob(gainLeft, gainKnobTop, knobWidth, colorBlue, "Gain", ParameterID::gain);
    addKnob(
      gainLeft + 1.0 * knobX, gainKnobTop, knobWidth, colorBlue, "A", ParameterID::gainA);
    addKnob(
      gainLeft + 2.0 * knobX, gainKnobTop, knobWidth, colorBlue, "D", ParameterID::gainD);
    addKnob(
      gainLeft + 3.0 * knobX, gainKnobTop, knobWidth, colorBlue, "S", ParameterID::gainS);
    addKnob(
      gainLeft + 4.0 * knobX, gainKnobTop, knobWidth, colorBlue, "R", ParameterID::gainR);
    addKnob(
      gainLeft + 5.0 * knobX, gainKnobTop, knobWidth, colorBlue, "Curve",
      ParameterID::gainEnvelopeCurve);

    // Filter.
    const auto filterTop = gainKnobTop + knobY + margin;
    const auto filterLeft = modLeft;
    addGroupLabel(filterLeft, filterTop, 4.0 * knobX - 10.0, "Filter");
    addCheckbox(
      filterLeft + 4.0 * knobX, filterTop, checkboxWidth, "Dirty Buffer",
      ParameterID::filterDirty);

    const auto filterTop1 = filterTop + labelY;
    addKnob(
      filterLeft, filterTop1, knobWidth, colorBlue, "Cut", ParameterID::filterCutoff);
    addKnob(
      filterLeft + 1.0 * knobX, filterTop1, knobWidth, colorBlue, "Res",
      ParameterID::filterResonance);
    addKnob(
      filterLeft + 2.0 * knobX, filterTop1, knobWidth, colorBlue, "Feed",
      ParameterID::filterFeedback);
    addKnob(
      filterLeft + 3.0 * knobX, filterTop1, knobWidth, colorBlue, "Sat",
      ParameterID::filterSaturation);

    const auto filterMenuWidth = 100.0;
    std::vector<std::string> filterTypeOptions = {"LP", "HP", "BP", "Notch", "Bypass"};
    addOptionMenu(
      filterLeft + 4.0 * knobX, filterTop1, filterMenuWidth, ParameterID::filterType,
      filterTypeOptions);
    std::vector<std::string> filterShaperOptions
      = {"HardClip", "Tanh", "ShaperA", "ShaperB"};
    addOptionMenu(
      filterLeft + 4.0 * knobX, filterTop1 + labelY, filterMenuWidth,
      ParameterID::filterShaper, filterShaperOptions);

    const auto filterTop2 = filterTop1 + knobY;
    addKnob(filterLeft, filterTop2, knobWidth, colorBlue, "A", ParameterID::filterA);
    addKnob(
      filterLeft + 1.0 * knobX, filterTop2, knobWidth, colorBlue, "D",
      ParameterID::filterD);
    addKnob(
      filterLeft + 2.0 * knobX, filterTop2, knobWidth, colorBlue, "S",
      ParameterID::filterS);
    addKnob(
      filterLeft + 3.0 * knobX, filterTop2, knobWidth, colorBlue, "R",
      ParameterID::filterR);
    addKnob(
      filterLeft + 4.0 * knobX, filterTop2, knobWidth, colorBlue, "To Cut",
      ParameterID::filterCutoffAmount);
    addKnob(
      filterLeft + 5.0 * knobX, filterTop2, knobWidth, colorBlue, "To Res",
      ParameterID::filterResonanceAmount);

    const auto filterTop3 = filterTop2 + knobY;
    addKnob(
      filterLeft, filterTop3, knobWidth, colorBlue, "Key->Cut",
      ParameterID::filterKeyToCutoff);
    addKnob(
      filterLeft + 1.0 * knobX, filterTop3, knobWidth, colorBlue, "Key->Feed",
      ParameterID::filterKeyToFeedback);

    // Plugin name.
    const auto splashTop = filterTop2 + knobY + 2.0 * margin;
    const auto splashLeft = modLeft + 2.0 * knobX;
    addSplashScreen(
      splashLeft + 0.25f * knobX, splashTop, 3.5f * knobX, 40.0f, 20.0f, 20.0f,
      defaultWidth - 40.0f, defaultHeight - 40.0f, "SyncSawSynth");
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

  void updateState(std::string /* key */, std::string /* value */) {}

  void programLoaded(uint32_t index) override
  {
    param.loadProgram(index);
    for (auto &vWidget : valueWidget)
      vWidget->setValue(param.value[vWidget->id]->getNormalized());
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

  void addOscillatorSection(
    const char *label,
    float left,
    float top,
    uint32_t idGain,
    uint32_t idSemi,
    uint32_t idCent,
    uint32_t idSync,
    uint32_t idSyncType,
    uint32_t idPTROrder,
    uint32_t idPhase,
    uint32_t idPhaseLock)
  {
    addGroupLabel(left, top, 2.0 * knobX, label);

    top += labelHeight + 10.0;
    auto knobCenterX = margin + left + knobX / 2.0;
    addKnob(knobCenterX, top, knobWidth, colorBlue, "Gain", idGain);

    auto oscTop2 = top + knobY;
    auto knobLeft = margin + left;
    addNumberKnob<SomeDSP::LinearScale<double>>(
      knobLeft, oscTop2, knobWidth, colorBlue, "Semi", idSemi, Scales::semi);
    addKnob(knobLeft + knobX, oscTop2, knobWidth, colorBlue, "Cent", idCent);

    auto oscTop3 = oscTop2 + knobY;
    auto syncKnobSize = 2.0 * knobHeight;
    auto oscMenuWidth = 2.0 * knobX;
    addKnob(
      left + (oscMenuWidth - syncKnobSize) / 2.0, oscTop3, syncKnobSize, colorBlue,
      "Sync", idSync);

    auto oscTop4 = oscTop3 + syncKnobSize + labelY - 10.0;
    std::vector<std::string> syncOptions
      = {"Off", "Ratio", "Fixed-Master", "Fixed-Slave"};
    addOptionMenu(left, oscTop4, oscMenuWidth, idSyncType, syncOptions);

    auto oscTop5 = oscTop4 + labelY - margin;
    std::vector<std::string> ptrOrderOptions
      = {"Order 0",        "Order 1",        "Order 2",        "Order 3",
         "Order 4",        "Order 5",        "Order 6",        "Order 7",
         "Order 8",        "Order 9",        "Order 10",       "Sin",
         "Order 6 double", "Order 7 double", "Order 8 double", "Order 9 double",
         "Order 10 double"};
    addOptionMenu(left, oscTop5, oscMenuWidth, idPTROrder, ptrOrderOptions);

    auto oscTop6 = oscTop5 + labelY;
    addKnob(knobLeft, oscTop6, knobWidth, colorBlue, "Phase", idPhase);
    addCheckbox(
      knobLeft + knobX, oscTop6 + (knobHeight - labelHeight) / 2.0, checkboxWidth, "Lock",
      idPhaseLock);
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

  void addOptionMenu(
    float left,
    float top,
    float width,
    uint32_t id,
    const std::vector<std::string> &items)
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

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyncSawSynthUI)
};

UI *createUI() { return new SyncSawSynthUI(); }

END_NAMESPACE_DISTRHO
