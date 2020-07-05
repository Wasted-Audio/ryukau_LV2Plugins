// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of KuramotoModel.
//
// KuramotoModel is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KuramotoModel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KuramotoModel.  If not, see <https://www.gnu.org/licenses/>.

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
constexpr float innerWidth = barboxWidth + labelY + 2 * knobX;
constexpr float innerHeight = labelY + 5 * barboxHeight;
constexpr uint32_t defaultWidth = uint32_t(innerWidth + 2 * uiMargin);
constexpr uint32_t defaultHeight = uint32_t(innerHeight + 2 * uiMargin);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class KuramotoModelUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KuramotoModelUI)

public:
  KuramotoModelUI() : PluginUIBase(defaultWidth, defaultHeight)
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

    // Knobs.
    addGroupLabel(left1, top0, innerWidth, labelHeight, midTextSize, "KuramotoModel");

    addKnob(left1, top1, knobWidth, margin, uiTextSize, "Gain", ID::boost);
    addKnob(left1, top1 + 2 * knobY, knobWidth, margin, uiTextSize, "Attack", ID::attack);
    addKnob(
      left1, top1 + 3 * knobY, knobWidth, margin, uiTextSize, "Decay.Mul",
      ID::decayMultiply);

    // Tuning.
    const auto tuningTop1 = top1 + 4 * knobY;
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

    const auto tuningTop5 = tuningTop4 + labelY;
    addLabel(left0, tuningTop5, knobWidth, labelHeight, uiTextSize, "A4 [Hz]");
    addTextKnob(
      left1, tuningTop5, knobWidth, labelHeight, uiTextSize, ID::pitchA4Hz,
      Scales::pitchA4Hz, false, 0, 100);

    // BarBox.
    addGroupVerticalLabel(left2, top1, barboxHeight, labelHeight, uiTextSize, "Gain");
    addBarBox(
      left2 + labelY, top1, barboxWidth, barboxHeight, ID::gain0, nOscillator,
      Scales::gain);

    addGroupVerticalLabel(
      left2, top1 + 1 * barboxHeight, barboxHeight, labelHeight, uiTextSize, "Decay");
    addBarBox(
      left2 + labelY, top1 + 1 * barboxHeight, barboxWidth, barboxHeight, ID::decay0,
      nOscillator, Scales::decay);

    addGroupVerticalLabel(
      left2, top1 + 2 * barboxHeight, barboxHeight, labelHeight, uiTextSize, "Pitch");
    auto barboxPitch = addBarBox(
      left2 + labelY, top1 + 2 * barboxHeight, barboxWidth, barboxHeight, ID::pitch0,
      nOscillator, Scales::pitch);
    barboxPitch->sliderZero = 0.5f;

    addGroupVerticalLabel(
      left2, top1 + 3 * barboxHeight, barboxHeight, labelHeight, uiTextSize, "Coupling");
    addBarBox(
      left2 + labelY, top1 + 3 * barboxHeight, barboxWidth, barboxHeight, ID::coupling0,
      nOscillator, Scales::coupling);

    addGroupVerticalLabel(
      left2, top1 + 4 * barboxHeight, barboxHeight, labelHeight, uiTextSize,
      "Coupling Decay");
    addBarBox(
      left2 + labelY, top1 + 4 * barboxHeight, barboxWidth, barboxHeight,
      ID::couplingDecay0, nOscillator, Scales::defaultScale);
  }
};

UI *createUI() { return new KuramotoModelUI(); }

END_NAMESPACE_DISTRHO
