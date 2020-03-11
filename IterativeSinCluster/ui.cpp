// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of IterativeSinCluster.
//
// IterativeSinCluster is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// IterativeSinCluster is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with IterativeSinCluster.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>
#include <memory>
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
  text(20.0f, 180.0f, "Shift + Left Drag: Fine Adjustment", nullptr);
  text(20.0f, 210.0f, "Ctrl + Left Click: Reset to Default", nullptr);

  text(20.0f, 270.0f, "- Number -", nullptr);
  text(20.0f, 300.0f, "Shares same controls with knob, and:", nullptr);
  text(20.0f, 330.0f, "Right Click: Flip Minimum and Maximum", nullptr);

  text(380.0f, 150.0f, "- Overtone -", nullptr);
  text(380.0f, 180.0f, "Ctrl + Left Click: Reset to Default", nullptr);
  text(380.0f, 210.0f, "Right Drag: Draw Line", nullptr);
  text(380.0f, 270.0f, "Have a nice day!", nullptr);
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
constexpr float checkboxWidth = 60.0f;
constexpr float splashHeight = 40.0f;
constexpr uint32_t defaultWidth = uint32_t(12 * knobX + 4 * margin + 40);
constexpr uint32_t defaultHeight
  = uint32_t(40 + 10 * labelY + 2 * knobY + 1 * knobHeight + 2 * margin);

class IterativeSinClusterUI : public PluginUI {
public:
  IterativeSinClusterUI() : PluginUI(defaultWidth, defaultHeight)
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
    const auto gainLeft = left0 + knobX;
    addGroupLabel(gainLeft, gainTop, 7.0 * knobX, "Gain");
    const auto gainKnobTop = gainTop + labelY;
    addKnob(
      gainLeft + 0.0 * knobX, gainKnobTop, knobWidth, colorBlue, "Boost", ID::gainBoost);
    addKnob(gainLeft + 1.0 * knobX, gainKnobTop, knobWidth, colorBlue, "Gain", ID::gain);
    addKnob(gainLeft + 2.0 * knobX, gainKnobTop, knobWidth, colorBlue, "A", ID::gainA);
    addKnob(gainLeft + 3.0 * knobX, gainKnobTop, knobWidth, colorBlue, "D", ID::gainD);
    addKnob(gainLeft + 4.0 * knobX, gainKnobTop, knobWidth, colorBlue, "S", ID::gainS);
    addKnob(gainLeft + 5.0 * knobX, gainKnobTop, knobWidth, colorBlue, "R", ID::gainR);
    addKnob(
      gainLeft + 6.0 * knobX, gainKnobTop, knobWidth, colorBlue, "Curve",
      ID::gainEnvelopeCurve);

    // Random.
    const float randomTop0 = top0;
    const float randomLeft0 = left0 + 8.0f * knobX + 4.0f * margin;
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

    // Shelving.
    const auto filterTop0 = gainTop + knobY + labelY;
    const auto filterLeft0 = left0;
    const auto filterLeft1 = filterLeft0 + knobX;
    const auto filterLeft2 = filterLeft1 + knobX;
    const auto filterTop1 = filterTop0 + labelY - 2.0f * margin;
    const auto filterTop2 = filterTop1 + labelY - margin;
    const auto filterTop3 = filterTop2 + labelY;
    addGroupLabel(filterLeft1, filterTop0, 2.0f * knobX, "Shelving");
    addLabel(filterLeft0, filterTop2, knobX, "Gain [dB]");
    addLabel(filterLeft0, filterTop3, knobX, "Semi");

    addLabel(filterLeft1, filterTop1, knobX, "Low");
    addTextKnob(
      filterLeft1, filterTop2, knobX, colorBlue, ID::lowShelfGain, Scales::shelvingGain,
      true, 2);
    addTextKnob(
      filterLeft1, filterTop3, knobX, colorBlue, ID::lowShelfPitch, Scales::shelvingPitch,
      false, 3);

    addLabel(filterLeft2, filterTop1, knobX, "High");
    addTextKnob(
      filterLeft2, filterTop2, knobX, colorBlue, ID::highShelfGain, Scales::shelvingGain,
      true, 2);
    addTextKnob(
      filterLeft2, filterTop3, knobX, colorBlue, ID::highShelfPitch,
      Scales::shelvingPitch, false, 3);

    // Pitch.
    const auto pitchTop0 = top0 + knobY + labelY;
    const auto pitchLeft0 = left0 + 3.0f * knobX + 4.0f * margin;
    addGroupLabel(pitchLeft0, pitchTop0, 4.0f * knobX, "Pitch");
    addCheckbox(
      pitchLeft0 + 0.0f * knobX, pitchTop0 + labelY - 2.0f * margin, knobX,
      "Add Aliasing", ID::aliasing);
    addCheckbox(
      pitchLeft0 + 1.8f * knobX, pitchTop0 + labelY - 2.0f * margin, knobX,
      "Reverse Semi", ID::negativeSemi);

    const auto pitchTop1 = pitchTop0 + 2.0f * labelY - 3.0f * margin;
    const auto pitchTop2 = pitchTop1 + labelY;
    addLabel(pitchLeft0 + margin, pitchTop1, knobX - 2.0f * margin, "Octave");
    addTextKnob(
      pitchLeft0 + knobX, pitchTop1, knobX, colorBlue, ID::masterOctave,
      Scales::masterOctave);
    addLabel(pitchLeft0 + margin, pitchTop2, knobX - 2.0f * margin, "ET");
    addTextKnob(
      pitchLeft0 + knobX, pitchTop2, knobX, colorBlue, ID::equalTemperament,
      Scales::equalTemperament);

    const auto pitchLeft1 = pitchLeft0 + 2.1f * knobX;
    addLabel(pitchLeft1 + margin, pitchTop1, knobX - 2.0f * margin, "Multiply");
    addTextKnob(
      pitchLeft1 + knobX, pitchTop1, knobX, colorBlue, ID::pitchMultiply,
      Scales::pitchMultiply, false, 3);

    addLabel(pitchLeft1 + margin, pitchTop2, knobX - 2.0f * margin, "Modulo");
    addTextKnob(
      pitchLeft1 + knobX, pitchTop2, knobX, colorBlue, ID::pitchModulo,
      Scales::pitchModulo, false, 3);

    // Misc.
    const auto miscLeft = pitchLeft0 + 4.5f * knobX + 2.0f * margin;
    addKnob(miscLeft, pitchTop0, knobWidth, colorBlue, "Smooth", ID::smoothness);
    std::vector<std::string> nVoiceOptions
      = {"Mono", "2 Voices", "4 Voices", "8 Voices", "16 Voices", "32 Voices"};
    addOptionMenu(
      miscLeft - (checkboxWidth - knobWidth) / 2.0f, pitchTop0 + knobY, checkboxWidth,
      ID::nVoice, nVoiceOptions);

    // Chorus.
    const float chorusTop0 = filterTop0;
    const float chorusLeft0 = randomLeft0 + knobX;
    const float chorusLeft1 = chorusLeft0 + knobX;
    const float chorusLeft2 = chorusLeft1 + knobX;
    addGroupLabel(chorusLeft0, chorusTop0, 3.0f * knobX, "Chorus");

    const float chorusTop1 = chorusTop0 + labelY;
    addKnob(chorusLeft0, chorusTop1, knobWidth, colorBlue, "Mix", ID::chorusMix);
    addKnob(chorusLeft1, chorusTop1, knobWidth, colorBlue, "Freq", ID::chorusFrequency);
    addKnob(chorusLeft2, chorusTop1, knobWidth, colorBlue, "Depth", ID::chorusDepth);

    const float chorusTop2 = chorusTop1 + knobY;
    addKnob(
      chorusLeft0, chorusTop2, knobWidth, colorBlue, "Range0", ID::chorusDelayTimeRange0);
    addKnob(
      chorusLeft1, chorusTop2, knobWidth, colorBlue, "Range1", ID::chorusDelayTimeRange1);
    addKnob(
      chorusLeft2, chorusTop2, knobWidth, colorBlue, "Range2", ID::chorusDelayTimeRange2);

    const float chorusTop3 = chorusTop2 + knobY;
    addKnob(
      chorusLeft0, chorusTop3, knobWidth, colorBlue, "Time0", ID::chorusMinDelayTime0);
    addKnob(
      chorusLeft1, chorusTop3, knobWidth, colorBlue, "Time1", ID::chorusMinDelayTime1);
    addKnob(
      chorusLeft2, chorusTop3, knobWidth, colorBlue, "Time2", ID::chorusMinDelayTime2);

    const float chorusTop4 = chorusTop3 + knobY;
    addRotaryKnob(
      chorusLeft0, chorusTop4, knobWidth, colorBlue, "Phase", ID::chorusPhase);
    addKnob(chorusLeft1, chorusTop4, knobWidth, colorBlue, "Offset", ID::chorusOffset);
    addKnob(
      chorusLeft2, chorusTop4, knobWidth, colorBlue, "Feedback", ID::chorusFeedback);

    const float chorusTop5 = chorusTop4 + knobY;
    addCheckbox(
      chorusLeft0 + 0.8f * knobX, chorusTop5 + margin, checkboxWidth, "Key Follow",
      ID::chorusKeyFollow);

    // Note.
    const float noteTop0 = filterTop0 + knobY + labelY;
    addGroupLabel(left0 + knobX, noteTop0, 8.0f * knobX, "Note");

    const float noteTop1 = noteTop0 + labelY;
    addLabel(left0, noteTop1, knobX, "Gain [dB]");
    addTextKnob(
      left0 + 1.0f * knobX, noteTop1, knobX, colorBlue, ID::gain0, Scales::gainDecibel,
      true, 2);
    addTextKnob(
      left0 + 2.0f * knobX, noteTop1, knobX, colorBlue, ID::gain1, Scales::gainDecibel,
      true, 2);
    addTextKnob(
      left0 + 3.0f * knobX, noteTop1, knobX, colorBlue, ID::gain2, Scales::gainDecibel,
      true, 2);
    addTextKnob(
      left0 + 4.0f * knobX, noteTop1, knobX, colorBlue, ID::gain3, Scales::gainDecibel,
      true, 2);
    addTextKnob(
      left0 + 5.0f * knobX, noteTop1, knobX, colorBlue, ID::gain4, Scales::gainDecibel,
      true, 2);
    addTextKnob(
      left0 + 6.0f * knobX, noteTop1, knobX, colorBlue, ID::gain5, Scales::gainDecibel,
      true, 2);
    addTextKnob(
      left0 + 7.0f * knobX, noteTop1, knobX, colorBlue, ID::gain6, Scales::gainDecibel,
      true, 2);
    addTextKnob(
      left0 + 8.0f * knobX, noteTop1, knobX, colorBlue, ID::gain7, Scales::gainDecibel,
      true, 2);

    const float noteTop2 = noteTop1 + labelY;
    addLabel(left0, noteTop2, knobX, "Semi");
    addTextKnob(
      left0 + 1.0f * knobX, noteTop2, knobX, colorBlue, ID::semi0, Scales::oscSemi);
    addTextKnob(
      left0 + 2.0f * knobX, noteTop2, knobX, colorBlue, ID::semi1, Scales::oscSemi);
    addTextKnob(
      left0 + 3.0f * knobX, noteTop2, knobX, colorBlue, ID::semi2, Scales::oscSemi);
    addTextKnob(
      left0 + 4.0f * knobX, noteTop2, knobX, colorBlue, ID::semi3, Scales::oscSemi);
    addTextKnob(
      left0 + 5.0f * knobX, noteTop2, knobX, colorBlue, ID::semi4, Scales::oscSemi);
    addTextKnob(
      left0 + 6.0f * knobX, noteTop2, knobX, colorBlue, ID::semi5, Scales::oscSemi);
    addTextKnob(
      left0 + 7.0f * knobX, noteTop2, knobX, colorBlue, ID::semi6, Scales::oscSemi);
    addTextKnob(
      left0 + 8.0f * knobX, noteTop2, knobX, colorBlue, ID::semi7, Scales::oscSemi);

    const float noteTop3 = noteTop2 + labelY;
    addLabel(left0, noteTop3, knobX, "Milli");
    addTextKnob(
      left0 + 1.0f * knobX, noteTop3, knobX, colorBlue, ID::milli0, Scales::oscMilli);
    addTextKnob(
      left0 + 2.0f * knobX, noteTop3, knobX, colorBlue, ID::milli1, Scales::oscMilli);
    addTextKnob(
      left0 + 3.0f * knobX, noteTop3, knobX, colorBlue, ID::milli2, Scales::oscMilli);
    addTextKnob(
      left0 + 4.0f * knobX, noteTop3, knobX, colorBlue, ID::milli3, Scales::oscMilli);
    addTextKnob(
      left0 + 5.0f * knobX, noteTop3, knobX, colorBlue, ID::milli4, Scales::oscMilli);
    addTextKnob(
      left0 + 6.0f * knobX, noteTop3, knobX, colorBlue, ID::milli5, Scales::oscMilli);
    addTextKnob(
      left0 + 7.0f * knobX, noteTop3, knobX, colorBlue, ID::milli6, Scales::oscMilli);
    addTextKnob(
      left0 + 8.0f * knobX, noteTop3, knobX, colorBlue, ID::milli7, Scales::oscMilli);

    // Chord.
    const float topChord0 = noteTop0 + 4.0f * labelY;
    addGroupLabel(left0 + knobX, topChord0, 4.0f * knobX, "Chord");

    const float topChord1 = topChord0 + labelY;
    addLabel(left0, topChord1, knobX, "Gain [dB]");
    addTextKnob(
      left0 + 1.0f * knobX, topChord1, knobX, colorBlue, ID::chordGain0,
      Scales::gainDecibel, true, 2);
    addTextKnob(
      left0 + 2.0f * knobX, topChord1, knobX, colorBlue, ID::chordGain1,
      Scales::gainDecibel, true, 2);
    addTextKnob(
      left0 + 3.0f * knobX, topChord1, knobX, colorBlue, ID::chordGain2,
      Scales::gainDecibel, true, 2);
    addTextKnob(
      left0 + 4.0f * knobX, topChord1, knobX, colorBlue, ID::chordGain3,
      Scales::gainDecibel, true, 2);

    const float topChord2 = topChord1 + labelY;
    addLabel(left0, topChord2, knobX, "Semi");
    addTextKnob(
      left0 + 1.0f * knobX, topChord2, knobX, colorBlue, ID::chordSemi0, Scales::oscSemi);
    addTextKnob(
      left0 + 2.0f * knobX, topChord2, knobX, colorBlue, ID::chordSemi1, Scales::oscSemi);
    addTextKnob(
      left0 + 3.0f * knobX, topChord2, knobX, colorBlue, ID::chordSemi2, Scales::oscSemi);
    addTextKnob(
      left0 + 4.0f * knobX, topChord2, knobX, colorBlue, ID::chordSemi3, Scales::oscSemi);

    const float topChord3 = topChord2 + labelY;
    addLabel(left0, topChord3, knobX, "Milli");
    addTextKnob(
      left0 + 1.0f * knobX, topChord3, knobX, colorBlue, ID::chordMilli0,
      Scales::oscMilli);
    addTextKnob(
      left0 + 2.0f * knobX, topChord3, knobX, colorBlue, ID::chordMilli1,
      Scales::oscMilli);
    addTextKnob(
      left0 + 3.0f * knobX, topChord3, knobX, colorBlue, ID::chordMilli2,
      Scales::oscMilli);
    addTextKnob(
      left0 + 4.0f * knobX, topChord3, knobX, colorBlue, ID::chordMilli3,
      Scales::oscMilli);

    const float topChord4 = topChord3 + labelY;
    addLabel(left0, topChord4 + (knobX - labelY) / 2, knobX, "Pan");
    addKnob(
      left0 + 1.0f * knobX, topChord4 - margin, knobX, colorBlue, nullptr, ID::chordPan0);
    addKnob(
      left0 + 2.0f * knobX, topChord4 - margin, knobX, colorBlue, nullptr, ID::chordPan1);
    addKnob(
      left0 + 3.0f * knobX, topChord4 - margin, knobX, colorBlue, nullptr, ID::chordPan2);
    addKnob(
      left0 + 4.0f * knobX, topChord4 - margin, knobX, colorBlue, nullptr, ID::chordPan3);

    // Overtone.
    const float topOvertone0 = topChord0;
    const float leftOvertone = left0 + 5.0f * knobX + 2.0f * margin;
    addGroupLabel(leftOvertone, topOvertone0, 4.0f * knobX, "Overtone");

    std::vector<uint32_t> sBoxId(16);
    for (size_t i = 0; i < sBoxId.size(); ++i) sBoxId[i] = ID::overtone0 + i;

    std::vector<double> sBoxValue(sBoxId.size());
    for (size_t i = 0; i < sBoxValue.size(); ++i)
      sBoxValue[i] = param.value[sBoxId[i]]->getDefaultNormalized();

    std::vector<double> sBoxDefaultValue(sBoxValue);

    auto sliderBox
      = std::make_shared<BarBox>(this, this, sBoxId, sBoxValue, sBoxDefaultValue, fontId);
    sliderBox->setSize(4.0f * knobX, 2.0f * knobY);
    sliderBox->setAbsolutePos(leftOvertone, topOvertone0 + labelY);
    sliderBox->setBorderColor(colorFore);
    sliderBox->setValueColor(colorBlue);
    arrayWidget.push_back(sliderBox);

    // Plugin name.
    const auto splashTop = defaultHeight - splashHeight - 20.0f;
    const auto splashLeft = defaultWidth - 3.0f * knobX - 15.0f;
    addSplashScreen(
      splashLeft, splashTop, 3.0f * knobX, splashHeight, 20.0f, 20.0f,
      defaultWidth - splashHeight, defaultHeight - splashHeight, "IterativeSinCluster");
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

  void updateState(std::string /* key */, std::string /* value */) {}

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

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IterativeSinClusterUI)
};

UI *createUI() { return new IterativeSinClusterUI(); }

END_NAMESPACE_DISTRHO
