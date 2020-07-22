// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of KSCymbal.
//
// KSCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KSCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KSCymbal.  If not, see <https://www.gnu.org/licenses/>.

#include "../../common/uibase.hpp"
#include "parameter.hpp"

#include <sstream>
#include <tuple>

START_NAMESPACE_DISTRHO

constexpr float uiTextSize = 14.0f;
constexpr float midTextSize = 16.0f;
constexpr float pluginNameTextSize = 22.0f;
constexpr float uiMargin = 20.0f;
constexpr float margin = 5.0f;
constexpr float labelHeight = 20.0f;
constexpr float labelY = 30.0f;
constexpr float knobWidth = 50.0f;
constexpr float knobHeight = 40.0f;
constexpr float knobX = 60.0f; // With margin.
constexpr float knobY = knobHeight + labelY;
constexpr float barboxWidth = 512;
constexpr float barboxHeight = 2.0f * knobY;
constexpr float innerWidth = barboxWidth + labelY + 4 * knobX;
constexpr float innerHeight = labelY + 5 * barboxHeight;
constexpr uint32_t defaultWidth = uint32_t(innerWidth + 2 * uiMargin);
constexpr uint32_t defaultHeight = uint32_t(innerHeight + 2 * uiMargin);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class KSCymbalUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KSCymbalUI)

public:
  KSCymbalUI() : PluginUIBase(defaultWidth, defaultHeight)
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

    using ID = ParameterID::ID;

    const auto top0 = 20.0f;
    const auto top1 = top0 + labelY;
    const auto left0 = 20.0f;
    const auto left1 = left0 + knobX;
    const auto left2 = left1 + knobX;
    const auto left3 = left2 + knobX;

    // Knobs.
    addGroupLabel(left1, top0, innerWidth, labelHeight, midTextSize, "KSCymbal");

    addKnob(left0, top1 + 0 * knobY, knobWidth, margin, uiTextSize, "Gain", ID::gain);
    addKnob(left0, top1 + 1 * knobY, knobWidth, margin, uiTextSize, "Boost", ID::boost);

    addKnob(
      left1, top1 + 0 * knobY, knobWidth, margin, uiTextSize, "Attack",
      ID::exciterAttack);
    addKnob(
      left1, top1 + 1 * knobY, knobWidth, margin, uiTextSize, "Gain", ID::exciterGain);

    addKnob(
      left0, top1 + 3 * knobY, knobWidth, margin, uiTextSize, "Rnd.Comb", ID::randomComb);
    addKnob(
      left1, top1 + 3 * knobY, knobWidth, margin, uiTextSize, "Rnd.Freq",
      ID::randomFrequency);

    addKnob(
      left0, top1 + 4 * knobY, knobWidth, margin, uiTextSize, "Distance", ID::distance);

    const auto miscTop1 = top1 + 5 * knobY;
    addLabel(left0, miscTop1, knobWidth, labelHeight, uiTextSize, "Seed");
    addTextKnob(
      left1, miscTop1, knobWidth, labelHeight, uiTextSize, ID::seed, Scales::seed);

    const auto miscTop2 = miscTop1 + labelY;
    addLabel(left0, miscTop2, knobWidth, labelHeight, uiTextSize, "LP Cutoff");
    addTextKnob(
      left1, miscTop2, knobWidth, labelHeight, uiTextSize, ID::lowpassCutoffHz,
      Scales::lowpassCutoffHz, false, 2);

    const auto miscTop3 = miscTop2 + labelY;
    addLabel(left0, miscTop3, knobWidth, labelHeight, uiTextSize, "HP Cutoff");
    addTextKnob(
      left1, miscTop3, knobWidth, labelHeight, uiTextSize, ID::highpassCutoffHz,
      Scales::highpassCutoffHz, false, 2);

    const auto miscTop4 = miscTop3 + labelY;
    addCheckbox(
      left0, miscTop4, knobWidth, labelHeight, uiTextSize, "Retrig.Noise",
      ID::retriggerNoise);
    addCheckbox(
      left0, miscTop4 + labelY, knobWidth, labelHeight, uiTextSize, "Retrig.Comb",
      ID::retriggerComb);
    addCheckbox(
      left0, miscTop4 + 2 * labelY, knobWidth, labelHeight, uiTextSize, "Retrig.Cymbal",
      ID::retriggerCymbal);

    const auto miscTop5 = miscTop4 + 3 * labelY;
    addLabel(left0, miscTop5, knobWidth, labelHeight, uiTextSize, "Unison");
    addTextKnob(
      left1, miscTop5, knobWidth, labelHeight, uiTextSize, ID::nUnison, Scales::nUnison,
      false, 0, 1);

    addCheckbox(
      left0, miscTop5 + labelY, knobWidth, labelHeight, uiTextSize, "Random Detune",
      ID::unisonDetuneRandom);

    // Envelope.
    addKnob(left2, top1 + 0 * knobY, knobWidth, margin, uiTextSize, "A", ID::lowpassA);
    addKnob(left2, top1 + 1 * knobY, knobWidth, margin, uiTextSize, "D", ID::lowpassD);
    addKnob(left2, top1 + 2 * knobY, knobWidth, margin, uiTextSize, "S", ID::lowpassS);
    addKnob(left2, top1 + 3 * knobY, knobWidth, margin, uiTextSize, "R", ID::lowpassR);
    addKnob(
      left2, top1 + 4 * knobY, knobWidth, margin, uiTextSize, "Offset",
      ID::lowpassEnvelopeOffset);

    // Unison.
    const auto unisonTop1 = top1 + 6 * knobY;
    addKnob(left2, unisonTop1, knobWidth, margin, uiTextSize, "Detune", ID::unisonDetune);
    addKnob(
      left2, unisonTop1 + 1 * knobY, knobWidth, margin, uiTextSize, "Pan", ID::unisonPan);
    addKnob(
      left2, unisonTop1 + 2 * knobY, knobWidth, margin, uiTextSize, "Gain",
      ID::unisonGainRandom);

    // Tuning.
    const auto tuningTop1 = miscTop5 + 2 * labelY;
    addLabel(left0, tuningTop1, knobWidth, labelHeight, uiTextSize, "Octave");
    addTextKnob(
      left1, tuningTop1, knobWidth, labelHeight, uiTextSize, ID::octave, Scales::octave,
      false, 0, -12);

    const auto tuningTop2 = tuningTop1 + labelY;
    addLabel(left0, tuningTop2, knobWidth, labelHeight, uiTextSize, "Semi");
    addTextKnob(
      left1, tuningTop2, knobWidth, labelHeight, uiTextSize, ID::semitone,
      Scales::semitone, false, 0, -120);

    const auto tuningTop3 = tuningTop2 + labelY;
    addLabel(left0, tuningTop3, knobWidth, labelHeight, uiTextSize, "Milli");
    auto knobOscMilli = addTextKnob(
      left1, tuningTop3, knobWidth, labelHeight, uiTextSize, ID::milli, Scales::milli,
      false, 0, -1000);
    knobOscMilli->sensitivity = 0.001f;
    knobOscMilli->lowSensitivity = 0.00025f;

    const auto tuningTop4 = tuningTop3 + labelY;
    addLabel(left0, tuningTop4, knobWidth, labelHeight, uiTextSize, "ET");
    addTextKnob(
      left1, tuningTop4, knobWidth, labelHeight, uiTextSize, ID::equalTemperament,
      Scales::equalTemperament, false, 0, 1);

    // Comb.
    addGroupVerticalLabel(
      left3, top1, barboxHeight, labelHeight, uiTextSize, "Comb Time");
    addBarBox(
      left3 + labelY, top1, barboxWidth, barboxHeight, ID::combTime0, nComb,
      Scales::combTime);

    // Cymbal.
    const auto cymbalTop1 = top1 + barboxHeight + labelHeight;
    addGroupVerticalLabel(
      left3, cymbalTop1, barboxHeight, labelHeight, uiTextSize, "Frequency");
    addBarBox(
      left3 + labelY, cymbalTop1, barboxWidth, barboxHeight, ID::frequency0, nDelay,
      Scales::frequency);
  }
};

UI *createUI() { return new KSCymbalUI(); }

END_NAMESPACE_DISTRHO
