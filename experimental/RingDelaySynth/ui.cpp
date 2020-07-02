// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of RingDelaySynth.
//
// RingDelaySynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RingDelaySynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RingDelaySynth.  If not, see <https://www.gnu.org/licenses/>.

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
constexpr float barboxWidth = 16.0f * 20.0f;
constexpr float barboxHeight = 2.0f * knobY;
constexpr float innerWidth = 6 * knobX + barboxWidth + 2 * labelY;
constexpr uint32_t defaultWidth = uint32_t(innerWidth + 2 * uiMargin);
constexpr uint32_t defaultHeight = uint32_t(labelY + 2 * barboxHeight + 2 * uiMargin);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class RingDelaySynthUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RingDelaySynthUI)

public:
  RingDelaySynthUI() : PluginUIBase(defaultWidth, defaultHeight)
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
    const auto left0 = 20.0f;

    addGroupLabel(left0, top0, innerWidth, labelHeight, midTextSize, "RingDelaySynth");

    const auto top1 = top0 + labelY;
    const auto top2 = top1 + knobY;
    const auto top3 = top2 + knobY;

    addKnob(left0 + 0 * knobX, top1, knobWidth, margin, uiTextSize, "Gain", ID::gain);
    addKnob(left0 + 1 * knobX, top1, knobWidth, margin, uiTextSize, "Smooth", ID::smooth);

    addKnob(
      left0 + 0 * knobX, top2, knobWidth, margin, uiTextSize, "NoiseGain", ID::noiseGain);
    addKnob(
      left0 + 1 * knobX, top2, knobWidth, margin, uiTextSize, "Propagation",
      ID::ringPropagation);
    addKnob(
      left0 + 2 * knobX, top2, knobWidth, margin, uiTextSize, "Attack", ID::noiseAttack);
    addKnob(
      left0 + 3 * knobX, top2, knobWidth, margin, uiTextSize, "Decay", ID::noiseDecay);
    addKnob(
      left0 + 4 * knobX, top2, knobWidth, margin, uiTextSize, "LP.Cutoff",
      ID::ringLowpassCutoff);
    addKnob(
      left0 + 5 * knobX, top2, knobWidth, margin, uiTextSize, "HP.Cutoff",
      ID::ringHighpassCutoff);

    addKnob(
      left0 + 0 * knobX, top3, knobWidth, margin, uiTextSize, "PitchDecay",
      ID::pitchDecay);
    addKnob(
      left0 + 1 * knobX, top3, knobWidth, margin, uiTextSize, "PitchMod",
      ID::pitchDecayAmount);
    addKnob(
      left0 + 2 * knobX, top3, knobWidth, margin, uiTextSize, "LP.Freq",
      ID::ksLowpassFrequency);
    addKnob(
      left0 + 3 * knobX, top3, knobWidth, margin, uiTextSize, "LP.Decay",
      ID::ksLowpassDecay);
    addKnob(
      left0 + 4 * knobX, top3, knobWidth, margin, uiTextSize, "LP.Mod",
      ID::ksLowpassDecayAmount);
    addKnob(left0 + 5 * knobX, top3, knobWidth, margin, uiTextSize, "Feed", ID::ksFeed);

    const auto left1 = left0 + 6 * knobX + labelY;

    addGroupVerticalLabel(
      left1, top1, barboxHeight, labelHeight, uiTextSize, "Frequency");
    addBarBox(
      left1 + labelY, top1, barboxWidth, barboxHeight, ID::frequency0, nDelay,
      Scales::frequency);
  }
};

UI *createUI() { return new RingDelaySynthUI(); }

END_NAMESPACE_DISTRHO
