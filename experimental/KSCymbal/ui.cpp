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
constexpr float innerWidth = barboxWidth + labelY + 2 * knobX;
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

    addKnob(left1, top1 + 0 * knobY, knobWidth, margin, uiTextSize, "Gain", ID::gain);
    addKnob(left1, top1 + 1 * knobY, knobWidth, margin, uiTextSize, "Boost", ID::boost);

    addKnob(left1, top1 + 2 * knobY, knobWidth, margin, uiTextSize, "Attack", ID::attack);
    addKnob(left1, top1 + 3 * knobY, knobWidth, margin, uiTextSize, "Decay", ID::decay);

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

    // Cymbal

    const auto freqTop1 = top1 + 0 * knobY;
    addLabel(left2, freqTop1, knobWidth, labelHeight, uiTextSize, "MinFreq");
    addTextKnob(
      left3, freqTop1, knobWidth, labelHeight, uiTextSize, ID::minFrequency,
      Scales::frequency, false, 3);

    const auto freqTop2 = top1 + 1 * knobY;
    addLabel(left2, freqTop2, knobWidth, labelHeight, uiTextSize, "MaxFreq");
    addTextKnob(
      left3, freqTop2, knobWidth, labelHeight, uiTextSize, ID::maxFrequency,
      Scales::frequency, false, 3);

    addKnob(
      left2, top1 + 2 * knobY, knobWidth, margin, uiTextSize, "Distance", ID::distance);

    const auto cymbalTop1 = top1 + 3 * knobY;
    addLabel(left2, cymbalTop1, knobWidth, labelHeight, uiTextSize, "Stack");
    addTextKnob(
      left3, cymbalTop1, knobWidth, labelHeight, uiTextSize, ID::stack, Scales::stack,
      false, 0, 2);

    const auto cymbalTop2 = cymbalTop1 + labelY;
    addLabel(left2, cymbalTop2, knobWidth, labelHeight, uiTextSize, "Seed");
    addTextKnob(
      left3, cymbalTop2, knobWidth, labelHeight, uiTextSize, ID::seed, Scales::seed);
  }
};

UI *createUI() { return new KSCymbalUI(); }

END_NAMESPACE_DISTRHO
