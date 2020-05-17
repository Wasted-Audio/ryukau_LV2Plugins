// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of EnvelopedSine.
//
// EnvelopedSine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// EnvelopedSine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with EnvelopedSine.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>
#include <memory>
#include <unordered_map>
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

  text(20.0f, 150.0f, "- Knob -", nullptr);
  text(20.0f, 180.0f, "Shift + Left Drag    |  Fine Adjustment", nullptr);
  text(20.0f, 210.0f, "Ctrl + Left Click    |  Reset to Default", nullptr);

  text(20.0f, 270.0f, "- Number -", nullptr);
  text(20.0f, 300.0f, "Shares same controls with knob, and:", nullptr);
  text(20.0f, 330.0f, "Right Click    |  Flip Min/Max", nullptr);

  text(320.0f, 150.0f, "- Overtone -", nullptr);

  text(320.0f, 180.0f, "Ctrl + Left Click    |  Reset to Default", nullptr);
  text(320.0f, 210.0f, "Right Drag    |  Draw Line", nullptr);
  text(320.0f, 240.0f, "D    |  Reset to Default", nullptr);
  text(320.0f, 270.0f, "Shift + D    |  Toggle Min/Mid/Max", nullptr);
  text(320.0f, 300.0f, "E    |  Emphasize Low", nullptr);
  text(320.0f, 330.0f, "Shift + E    |  Emphasize High", nullptr);
  text(320.0f, 360.0f, "F    |  Low-pass Filter", nullptr);
  text(320.0f, 390.0f, "Shift + F    |  High-pass Filter", nullptr);
  text(320.0f, 420.0f, "I    |  Invert Value", nullptr);
  text(320.0f, 450.0f, "Shift + I    |  Invert Value (Minimum to 0)", nullptr);
  text(320.0f, 480.0f, "N    |  Normalize", nullptr);
  text(320.0f, 510.0f, "Shift + N    |  Normalize (Minimum to 0)", nullptr);

  text(630.0f, 180.0f, "P    |  Permute", nullptr);
  text(630.0f, 210.0f, "R    |  Randomize", nullptr);
  text(630.0f, 240.0f, "Shift + R    |  Sparse Randomize", nullptr);
  text(630.0f, 270.0f, "S    |  Sort Decending Order", nullptr);
  text(630.0f, 300.0f, "Shift + S    |  Sort Ascending Order", nullptr);
  text(630.0f, 330.0f, "T    |  Subtle Randomize", nullptr);
  text(630.0f, 360.0f, ", (Comma)    |  Rotate Back", nullptr);
  text(630.0f, 390.0f, ". (Period)    |  Rotate Forward", nullptr);
  text(630.0f, 420.0f, "1    |  Decrease", nullptr);
  text(630.0f, 450.0f, "2-9    |  Decrease 2n-9n", nullptr);

  text(740.0f, 510.0f, "Have a nice day!", nullptr);
}

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
constexpr uint32_t defaultWidth
  = uint32_t(barboxWidth + 2 * knobX + labelY + 4 * margin + 40);
constexpr uint32_t defaultHeight
  = uint32_t(40 + 2 * labelY + knobY + 4 * barboxY + 2 * margin);

class EnvelopedSineUI : public PluginUI {
public:
  EnvelopedSineUI() : PluginUI(defaultWidth, defaultHeight)
  {
    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    using ID = ParameterID::ID;

    const auto top0 = 20.0f;
    const auto left0 = 20.0f;

    // Gain.
    const auto gainTop = top0;
    const auto gainLeft = left0;
    addGroupLabel(gainLeft, gainTop, 2.0 * knobX, "Gain");
    const auto gainKnobTop = gainTop + labelY;
    addKnob(
      gainLeft + 0.0 * knobX, gainKnobTop, knobWidth, colorBlue, "Boost", ID::gainBoost);
    addKnob(gainLeft + 1.0 * knobX, gainKnobTop, knobWidth, colorBlue, "Gain", ID::gain);

    // Pitch.
    const auto pitchTop0 = gainTop + labelY + knobY;
    const auto pitchLeft0 = left0;
    addGroupLabel(pitchLeft0, pitchTop0, 2.0f * knobX, "Pitch");
    addCheckbox(
      pitchLeft0, pitchTop0 + labelY - 2.0f * margin, knobX, "Add Aliasing",
      ID::aliasing);

    const auto pitchLabelWidth = knobX - 2.0f * margin;
    const auto pitchLeft1 = pitchLeft0 + margin;
    const auto pitchLeft2 = pitchLeft0 + knobX;

    const auto pitchTop1 = pitchTop0 + 2.0f * labelY - 3.0f * margin;
    addLabel(pitchLeft1, pitchTop1, pitchLabelWidth, "Octave");
    addTextKnob(
      pitchLeft2, pitchTop1, knobX, colorBlue, ID::masterOctave, Scales::masterOctave);

    const auto pitchTop2 = pitchTop1 + labelY;
    addLabel(pitchLeft1, pitchTop2, pitchLabelWidth, "Multiply");
    addTextKnob(
      pitchLeft2, pitchTop2, knobX, colorBlue, ID::pitchMultiply, Scales::pitchMultiply,
      false, 3);

    const auto pitchTop3 = pitchTop2 + labelY;
    addLabel(pitchLeft1, pitchTop3, pitchLabelWidth, "Modulo");
    addTextKnob(
      pitchLeft2, pitchTop3, knobX, colorBlue, ID::pitchModulo, Scales::pitchModulo,
      false, 3);

    const auto pitchTop4 = pitchTop3 + labelY;
    addLabel(pitchLeft1, pitchTop4, pitchLabelWidth, "Expand");
    addTextKnob(
      pitchLeft2, pitchTop4, knobX, colorBlue, ID::overtoneExpand, Scales::overtoneExpand,
      false, 3);

    const auto pitchTop5 = pitchTop4 + labelY;
    addLabel(pitchLeft1, pitchTop5, pitchLabelWidth, "Shift");
    addTextKnob(
      pitchLeft2, pitchTop5, knobX, colorBlue, ID::overtoneShift, Scales::overtoneShift,
      false, 3);

    // Random.
    const auto randomTop0 = pitchTop0 + labelHeight + 6.0 * labelY - margin;
    const auto randomLeft0 = left0;
    const auto randomLeft1 = randomLeft0 + knobX;
    addGroupLabel(randomLeft0, randomTop0, 2.0f * knobX, "Random");
    addCheckbox(
      randomLeft0, randomTop0 + labelY - 2.0f * margin, knobX, "Retrigger",
      ID::randomRetrigger);

    const auto randomTop1 = randomTop0 + 2.0f * labelY - 3.0f * margin;
    addLabel(randomLeft0, randomTop1, knobX - 2.0f * margin, "Seed");
    addTextKnob(
      randomLeft1 - 2.0f * margin, randomTop1, knobX, colorBlue, ID::seed, Scales::seed);

    const auto randomTop2 = randomTop1 + labelY;
    addKnob(randomLeft0, randomTop2, knobWidth, colorBlue, "To Gain", ID::randomGain);
    addKnob(
      randomLeft1, randomTop2, knobWidth, colorBlue, "To Pitch", ID::randomFrequency);

    const auto randomTop3 = randomTop2 + knobY;
    addKnob(randomLeft0, randomTop3, knobWidth, colorBlue, "To Attack", ID::randomAttack);
    addKnob(randomLeft1, randomTop3, knobWidth, colorBlue, "To Decay", ID::randomDecay);

    const auto randomTop4 = randomTop3 + knobY;
    addKnob(
      randomLeft0, randomTop4, knobWidth, colorBlue, "To Sat.", ID::randomSaturation);
    addKnob(randomLeft1, randomTop4, knobWidth, colorBlue, "To Phase", ID::randomPhase);

    // Misc.
    const auto miscTop = randomTop4 + knobY;
    const auto miscLeft = left0;
    addGroupLabel(miscLeft, miscTop, 2.0f * knobX, "Misc.");

    addKnob(miscLeft, miscTop + labelY, knobWidth, colorBlue, "Smooth", ID::smoothness);

    const auto miscLeft0 = miscLeft + knobX - (checkboxWidth - knobWidth) / 2.0f;
    const auto miscTop0 = miscTop + labelY;
    std::vector<std::string> nVoiceOptions
      = {"Mono", "2 Voices", "4 Voices", "8 Voices", "16 Voices", "32 Voices"};
    addOptionMenu(miscLeft0, miscTop0, checkboxWidth, ID::nVoice, nVoiceOptions);
    addCheckbox(miscLeft0, miscTop0 + labelY, checkboxWidth, "Unison", ID::unison);

    // Modifier.
    const auto modTop = top0 + 4.0f * (barboxY + margin);
    const auto modLeft = left0 + 2.0f * knobX + 4.0f * margin + labelY;
    addGroupLabel(modLeft, modTop, 4.0f * knobX, "Modifier");

    const auto modTop0 = modTop + labelY;
    addKnob(modLeft, modTop0, knobWidth, colorBlue, "Attack*", ID::attackMultiplier);
    addKnob(
      modLeft + knobX, modTop0, knobWidth, colorBlue, "Decay*", ID::decayMultiplier);
    addKnob(
      modLeft + 2.0f * knobX, modTop0, knobWidth, colorBlue, "Gain^", ID::gainPower);
    addKnob(
      modLeft + 3.0f * knobX, modTop0, knobWidth, colorBlue, "Sat. Mix",
      ID::saturationMix);

    const auto modTop1 = modTop0 + knobY;
    addCheckbox(modLeft + 0.4f * knobX, modTop1, checkboxWidth, "Declick", ID::declick);

    // Phaser.
    const auto phaserTop = modTop;
    const auto phaserLeft = modLeft + 4.0f * knobX + 4.0f * margin;
    addGroupLabel(phaserLeft, phaserTop, 7.0f * knobX + labelY, "Phaser");

    const auto phaserTop0 = phaserTop + labelY;
    addKnob(phaserLeft, phaserTop0, knobWidth, colorBlue, "Mix", ID::phaserMix);
    addKnob(
      phaserLeft + knobX, phaserTop0, knobWidth, colorBlue, "Freq", ID::phaserFrequency);
    addKnob(
      phaserLeft + 2.0f * knobX, phaserTop0, knobWidth, colorRed, "Feedback",
      ID::phaserFeedback);
    addKnob(
      phaserLeft + 3.0f * knobX, phaserTop0, knobWidth, colorBlue, "Range",
      ID::phaserRange);
    addKnob(
      phaserLeft + 4.0f * knobX, phaserTop0, knobWidth, colorBlue, "Min", ID::phaserMin);
    addKnob(
      phaserLeft + 5.0f * knobX, phaserTop0, knobWidth, colorBlue, "Offset",
      ID::phaserOffset);
    addRotaryKnob(
      phaserLeft + 6.0f * knobX, phaserTop0, knobWidth + labelY, colorBlue, "Phase",
      ID::phaserPhase);

    const auto phaserTop1 = phaserTop0 + knobY;
    std::vector<std::string> phaserStageItems{
      "Stage 1",  "Stage 2",  "Stage 3",  "Stage 4",  "Stage 5",  "Stage 6",
      "Stage 7",  "Stage 8",  "Stage 9",  "Stage 10", "Stage 11", "Stage 12",
      "Stage 13", "Stage 14", "Stage 15", "Stage 16"};
    addOptionMenu(
      phaserLeft - margin, phaserTop1, knobX, ID::phaserStage, phaserStageItems);

    // Attack.
    const auto attackTop = top0;
    const auto attackLeft = left0 + 2.0f * knobX + 4.0f * margin;
    addGroupVerticalLabel(attackLeft, attackTop, barboxHeight, "Attack");

    const auto attackLeft0 = attackLeft + labelY;
    addBarBox(
      attackLeft0, attackTop, barboxWidth, barboxHeight, ID::attack0, nOvertone,
      Scales::envelopeA);

    // Decay.
    const auto decayTop = attackTop + barboxY + margin;
    const auto decayLeft = attackLeft;
    addGroupVerticalLabel(decayLeft, decayTop, barboxHeight, "Decay");

    const auto decayLeft0 = decayLeft + labelY;
    addBarBox(
      decayLeft0, decayTop, barboxWidth, barboxHeight, ID::decay0, nOvertone,
      Scales::envelopeD);

    // Overtone.
    const auto overtoneTop = decayTop + barboxY + margin;
    const auto overtoneLeft = attackLeft;
    addGroupVerticalLabel(overtoneLeft, overtoneTop, barboxHeight, "Gain");

    const auto overtoneLeft0 = overtoneLeft + labelY;
    addBarBox(
      overtoneLeft0, overtoneTop, barboxWidth, barboxHeight, ID::overtone0, nOvertone,
      Scales::gainDecibel);

    // Saturation.
    const auto saturationTop = overtoneTop + barboxY + margin;
    const auto saturationLeft = attackLeft;
    addGroupVerticalLabel(saturationLeft, saturationTop, barboxHeight, "Saturation");

    const auto saturationLeft0 = saturationLeft + labelY;
    addBarBox(
      saturationLeft0, saturationTop, barboxWidth, barboxHeight, ID::saturation0,
      nOvertone, Scales::saturation);

    // Plugin name.
    const auto splashTop = defaultHeight - splashHeight - 20.0f;
    const auto splashLeft = left0;
    addSplashScreen(
      splashLeft, splashTop, 2.5f * knobX, splashHeight, 20.0f, 20.0f,
      defaultWidth - splashHeight, defaultHeight - splashHeight, "EnvelopedSine");
  }

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
    setParameterValue(id, param.updateValue(id, normalized));
    repaint();
    // dumpParameter(); // Used to make preset. There may be better way to do this.
  }

  void updateState(std::string /* key */, std::string /* value */) {}

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

  void addButton(float left, float top, float width, const char *title, uint32_t id)
  {
    auto button = std::make_shared<ToggleButton>(this, this, title, fontId);
    button->id = id;
    button->setSize(width, labelHeight);
    button->setAbsolutePos(left, top);
    button->setForegroundColor(colorFore);
    button->setHighlightColor(colorOrange);
    button->setTextSize(midTextSize);
    valueWidget.emplace(std::make_pair(id, button));
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
    valueWidget.emplace(std::make_pair(id, checkbox));
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

  void addGroupVerticalLabel(int left, int top, float width, const char *name)
  {
    auto label = std::make_shared<VLabel>(this, name, fontId);
    label->setSize(width, labelHeight);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->drawBorder = false;
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
    valueWidget.emplace(std::make_pair(id, knob));

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
    valueWidget.emplace(std::make_pair(id, knob));

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
    valueWidget.emplace(std::make_pair(id, knob));

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
    valueWidget.emplace(std::make_pair(id, menu));
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
    valueWidget.emplace(std::make_pair(id, slider));

    top += sliderHeight + 10.0;

    auto label = std::make_shared<Label>(this, name, fontId);
    label->setSize(width, labelHeight);
    label->setAbsolutePos(left, top);
    label->setForegroundColor(colorFore);
    label->setTextSize(uiTextSize);
    widget.push_back(label);
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopedSineUI)
};

UI *createUI() { return new EnvelopedSineUI(); }

END_NAMESPACE_DISTRHO
