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

#include "../common/uibase.hpp"
#include "parameter.hpp"

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
  fillColor(pal.background());
  fill();
  strokeColor(isMouseEntered ? pal.highlightMain() : pal.foreground());
  strokeWidth(borderWidth);
  stroke();

  // Text.
  fillColor(pal.foreground());
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
constexpr uint32_t defaultHeight
  = uint32_t(20.0 + 3.0 * labelY + labelHeight + 6.0 * knobY);

class SyncSawSynthUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyncSawSynthUI)

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
    addGroupLabel(left, top, 2.0 * knobX, labelHeight, midTextSize, label);

    top += labelHeight + 10.0;
    auto knobCenterX = margin + left + knobX / 2.0;
    addKnob(knobCenterX, top, knobWidth, margin, uiTextSize, "Gain", idGain);

    auto oscTop2 = top + knobY;
    auto knobLeft = margin + left;
    addNumberKnob(
      knobLeft, oscTop2, knobWidth, margin, uiTextSize, "Semi", idSemi, Scales::semi);
    addKnob(knobLeft + knobX, oscTop2, knobWidth, margin, uiTextSize, "Cent", idCent);

    auto oscTop3 = oscTop2 + knobY;
    auto syncKnobSize = 2.0 * knobHeight;
    auto oscMenuWidth = 2.0 * knobX;
    addKnob(
      left + (oscMenuWidth - syncKnobSize) / 2.0, oscTop3, syncKnobSize, margin,
      uiTextSize, "Sync", idSync);

    auto oscTop4 = oscTop3 + syncKnobSize + labelY - 10.0;
    std::vector<std::string> syncOptions
      = {"Off", "Ratio", "Fixed-Master", "Fixed-Slave"};
    addOptionMenu(
      left, oscTop4, oscMenuWidth, labelHeight, uiTextSize, idSyncType, syncOptions);

    auto oscTop5 = oscTop4 + labelY - margin;
    std::vector<std::string> ptrOrderOptions
      = {"Order 0",        "Order 1",        "Order 2",        "Order 3",
         "Order 4",        "Order 5",        "Order 6",        "Order 7",
         "Order 8",        "Order 9",        "Order 10",       "Sin",
         "Order 6 double", "Order 7 double", "Order 8 double", "Order 9 double",
         "Order 10 double"};
    addOptionMenu(
      left, oscTop5, oscMenuWidth, labelHeight, uiTextSize, idPTROrder, ptrOrderOptions);

    auto oscTop6 = oscTop5 + labelY;
    addKnob(knobLeft, oscTop6, knobWidth, margin, uiTextSize, "Phase", idPhase);
    addCheckbox(
      knobLeft + knobX, oscTop6 + (knobHeight - labelHeight) / 2.0, checkboxWidth,
      labelHeight, uiTextSize, "Lock", idPhaseLock);
  }

public:
  SyncSawSynthUI() : PluginUIBase(defaultWidth, defaultHeight)
  {
    param = std::make_unique<GlobalParameter>();

    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    if (palette.fontPath().size() > 0)
      fontId = createFontFromFile("main", palette.fontPath().c_str());

    if (fontId < 0) {
      fontId = createFontFromMemory(
        "main", (unsigned char *)(FontData::TinosBoldItalicData),
        FontData::TinosBoldItalicDataSize, false);
    }

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
      oscLeft2 - knobX / 2.0, oscTop + labelY, checkboxWidth, labelHeight, uiTextSize,
      ParameterID::nVoice, nVoiceOptions);
    addCheckbox(
      oscLeft2 - knobX / 2.0, oscTop + 2.0 * labelY, checkboxWidth, labelHeight,
      uiTextSize, "Unison", ParameterID::unison);

    const auto oscTop2 = 4.0 * labelY + 2.0 * knobY + 2.0 * knobHeight - margin;
    addCheckbox(
      oscLeft2 + margin + knobX,
      oscTop2 + labelY + (knobHeight - labelHeight) / 2.0 - 10.0, checkboxWidth,
      labelHeight, uiTextSize, "Invert", ParameterID::osc2Invert);

    // Cross modulation.
    const auto crossTop = oscTop2 + knobY + labelHeight;
    const auto crossLeft = oscLeft1;
    addGroupLabel(
      crossLeft, crossTop, 2.0 * oscWidth + 10.0, labelHeight, midTextSize, "Modulation");

    const auto crossTop2 = crossTop + labelY;
    const auto crossKnobLeft = margin + crossLeft;
    addKnob(
      crossKnobLeft, crossTop2, 1.5 * knobWidth, margin, uiTextSize, "Osc1->Sync1",
      ParameterID::fmOsc1ToSync1);
    addKnob(
      crossKnobLeft + 1.5 * knobWidth + 10.0, crossTop2, 1.5 * knobWidth, margin,
      uiTextSize, "Osc1->Freq2", ParameterID::fmOsc1ToFreq2);
    addKnob(
      crossKnobLeft + 3.0 * knobWidth + 20.0, crossTop2, 1.5 * knobWidth, margin,
      uiTextSize, "Osc2->Sync1", ParameterID::fmOsc2ToSync1);

    // Modulation envelope and LFO.
    const auto modTop = oscTop;
    const auto modLeft = oscLeft2 + oscWidth + 20.0;
    addGroupLabel(modLeft, modTop, 6.0 * knobX, labelHeight, midTextSize, "Modulation");

    const auto modTop1 = modTop + labelY;
    addKnob(
      modLeft, modTop1, knobWidth, margin, uiTextSize, "Attack",
      ParameterID::modEnvelopeA);
    addKnob(
      modLeft + 1.0 * knobX, modTop1, knobWidth, margin, uiTextSize, "Curve",
      ParameterID::modEnvelopeCurve);
    addKnob(
      modLeft + 2.0 * knobX, modTop1, knobWidth, margin, uiTextSize, "To Freq1",
      ParameterID::modEnvelopeToFreq1);
    addKnob(
      modLeft + 3.0 * knobX, modTop1, knobWidth, margin, uiTextSize, "To Sync1",
      ParameterID::modEnvelopeToSync1);
    addKnob(
      modLeft + 4.0 * knobX, modTop1, knobWidth, margin, uiTextSize, "To Freq2",
      ParameterID::modEnvelopeToFreq2);
    addKnob(
      modLeft + 5.0 * knobX, modTop1, knobWidth, margin, uiTextSize, "To Sync2",
      ParameterID::modEnvelopeToSync2);

    const auto modTop2 = modTop1 + knobY;
    addKnob(
      modLeft, modTop2, knobWidth, margin, uiTextSize, "LFO",
      ParameterID::modLFOFrequency);
    addKnob(
      modLeft + 1.0 * knobX, modTop2, knobWidth, margin, uiTextSize, "NoiseMix",
      ParameterID::modLFONoiseMix);
    addKnob(
      modLeft + 2.0 * knobX, modTop2, knobWidth, margin, uiTextSize, "To Freq1",
      ParameterID::modLFOToFreq1);
    addKnob(
      modLeft + 3.0 * knobX, modTop2, knobWidth, margin, uiTextSize, "To Sync1",
      ParameterID::modLFOToSync1);
    addKnob(
      modLeft + 4.0 * knobX, modTop2, knobWidth, margin, uiTextSize, "To Freq2",
      ParameterID::modLFOToFreq2);
    addKnob(
      modLeft + 5.0 * knobX, modTop2, knobWidth, margin, uiTextSize, "To Sync2",
      ParameterID::modLFOToSync2);

    const auto modTop3 = modTop2 + knobY;
    addCheckbox(
      modLeft, modTop3, knobX, labelHeight, uiTextSize, "Sync",
      ParameterID::lfoTempoSync);

    auto knobLfoTempoNumerator = addTextKnob(
      modLeft + knobX, modTop3, knobX - 1, labelHeight, uiTextSize,
      ParameterID::lfoTempoNumerator, Scales::lfoTempoNumerator, false, 0, 1);
    knobLfoTempoNumerator->sensitivity = 0.001;
    knobLfoTempoNumerator->lowSensitivity = 0.00025;

    auto knobLfoTempoDenominator = addTextKnob(
      modLeft + 2.0 * knobX, modTop3, knobX - 1, labelHeight, uiTextSize,
      ParameterID::lfoTempoDenominator, Scales::lfoTempoDenominator, false, 0, 1);
    knobLfoTempoDenominator->sensitivity = 0.001;
    knobLfoTempoNumerator->lowSensitivity = 0.00025;

    // Gain.
    const auto gainTop = modTop2 + knobY + labelHeight + margin;
    const auto gainLeft = modLeft;
    addGroupLabel(gainLeft, gainTop, 6.0 * knobX, labelHeight, midTextSize, "Gain");

    const auto gainKnobTop = gainTop + labelY;
    addKnob(
      gainLeft, gainKnobTop, knobWidth, margin, uiTextSize, "Gain", ParameterID::gain);
    addKnob(
      gainLeft + 1.0 * knobX, gainKnobTop, knobWidth, margin, uiTextSize, "A",
      ParameterID::gainA);
    addKnob(
      gainLeft + 2.0 * knobX, gainKnobTop, knobWidth, margin, uiTextSize, "D",
      ParameterID::gainD);
    addKnob(
      gainLeft + 3.0 * knobX, gainKnobTop, knobWidth, margin, uiTextSize, "S",
      ParameterID::gainS);
    addKnob(
      gainLeft + 4.0 * knobX, gainKnobTop, knobWidth, margin, uiTextSize, "R",
      ParameterID::gainR);
    addKnob(
      gainLeft + 5.0 * knobX, gainKnobTop, knobWidth, margin, uiTextSize, "Curve",
      ParameterID::gainEnvelopeCurve);

    // Filter.
    const auto filterTop = gainKnobTop + knobY + margin;
    const auto filterLeft = modLeft;
    addGroupLabel(
      filterLeft, filterTop, 4.0 * knobX - 10.0, labelHeight, midTextSize, "Filter");
    addCheckbox(
      filterLeft + 4.0 * knobX, filterTop, checkboxWidth, labelHeight, uiTextSize,
      "Dirty Buffer", ParameterID::filterDirty);

    const auto filterTop1 = filterTop + labelY;
    addKnob(
      filterLeft, filterTop1, knobWidth, margin, uiTextSize, "Cut",
      ParameterID::filterCutoff);
    addKnob(
      filterLeft + 1.0 * knobX, filterTop1, knobWidth, margin, uiTextSize, "Res",
      ParameterID::filterResonance);
    addKnob(
      filterLeft + 2.0 * knobX, filterTop1, knobWidth, margin, uiTextSize, "Feed",
      ParameterID::filterFeedback);
    addKnob(
      filterLeft + 3.0 * knobX, filterTop1, knobWidth, margin, uiTextSize, "Sat",
      ParameterID::filterSaturation);

    const auto filterMenuWidth = 100.0;
    std::vector<std::string> filterTypeOptions = {"LP", "HP", "BP", "Notch", "Bypass"};
    addOptionMenu(
      filterLeft + 4.0 * knobX, filterTop1, filterMenuWidth, labelHeight, uiTextSize,
      ParameterID::filterType, filterTypeOptions);
    std::vector<std::string> filterShaperOptions
      = {"HardClip", "Tanh", "ShaperA", "ShaperB"};
    addOptionMenu(
      filterLeft + 4.0 * knobX, filterTop1 + labelY, filterMenuWidth, labelHeight,
      uiTextSize, ParameterID::filterShaper, filterShaperOptions);

    const auto filterTop2 = filterTop1 + knobY;
    addKnob(
      filterLeft, filterTop2, knobWidth, margin, uiTextSize, "A", ParameterID::filterA);
    addKnob(
      filterLeft + 1.0 * knobX, filterTop2, knobWidth, margin, uiTextSize, "D",
      ParameterID::filterD);
    addKnob(
      filterLeft + 2.0 * knobX, filterTop2, knobWidth, margin, uiTextSize, "S",
      ParameterID::filterS);
    addKnob(
      filterLeft + 3.0 * knobX, filterTop2, knobWidth, margin, uiTextSize, "R",
      ParameterID::filterR);
    addKnob(
      filterLeft + 4.0 * knobX, filterTop2, knobWidth, margin, uiTextSize, "To Cut",
      ParameterID::filterCutoffAmount);
    addKnob(
      filterLeft + 5.0 * knobX, filterTop2, knobWidth, margin, uiTextSize, "To Res",
      ParameterID::filterResonanceAmount);

    const auto filterTop3 = filterTop2 + knobY;
    addKnob(
      filterLeft, filterTop3, knobWidth, margin, uiTextSize, "Key->Cut",
      ParameterID::filterKeyToCutoff);
    addKnob(
      filterLeft + 1.0 * knobX, filterTop3, knobWidth, margin, uiTextSize, "Key->Feed",
      ParameterID::filterKeyToFeedback);

    // Plugin name.
    const auto splashTop = filterTop2 + knobY + 2.0 * margin;
    const auto splashLeft = modLeft + 2.0 * knobX;
    addSplashScreen(
      splashLeft + 0.25f * knobX, splashTop, 3.5f * knobX, 40.0f, 20.0f, 20.0f,
      defaultWidth - 40.0f, defaultHeight - 40.0f, pluginNameTextSize, "SyncSawSynth");
  }
};

UI *createUI() { return new SyncSawSynthUI(); }

END_NAMESPACE_DISTRHO
