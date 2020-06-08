// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_3PoleLP.
//
// CV_3PoleLP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_3PoleLP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_3PoleLP.  If not, see <https://www.gnu.org/licenses/>.

#include "../../common/uibase.hpp"
#include "parameter.hpp"

#include <sstream>
#include <tuple>

START_NAMESPACE_DISTRHO

constexpr float uiTextSize = 14.0f;
constexpr float midTextSize = 16.0f;
constexpr float pluginNameTextSize = 22.0f;
constexpr float margin = 5.0f;
constexpr float labelHeight = 20.0f;
constexpr float labelY = 30.0f;
constexpr float knobWidth = 50.0f;
constexpr float knobHeight = 40.0f;
constexpr float knobX = 80.0f; // With margin.
constexpr float knobY = knobHeight + labelY;
constexpr uint32_t defaultWidth = uint32_t(2 * knobX + 40);
constexpr uint32_t defaultHeight = uint32_t(labelHeight + 7 * labelY + 30);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class CV_3PoleLPUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_3PoleLPUI)

public:
  CV_3PoleLPUI() : PluginUIBase(defaultWidth, defaultHeight)
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

    const auto top0 = 15.0f;
    const auto left0 = 20.0f;
    const auto left1 = 20.0f + knobX;

    addGroupLabel(left0, top0, 2 * knobX, labelHeight, midTextSize, "CV_3PoleLP");

    const int labelAlign = ALIGN_LEFT | ALIGN_MIDDLE;

    addLabel(left0, top0 + labelY, knobX, labelHeight, uiTextSize, "Gain", labelAlign);
    addTextKnob(
      left1, top0 + labelY, knobX, labelHeight, uiTextSize, ID::gain, Scales::gain, false,
      6);

    addLabel(
      left0, top0 + 2 * labelY, knobX, labelHeight, uiTextSize, "Cutoff [Hz]",
      labelAlign);
    addTextKnob(
      left1, top0 + 2 * labelY, knobX, labelHeight, uiTextSize, ID::cutoff,
      Scales::cutoff, false, 3);

    addLabel(
      left0, top0 + 3 * labelY, knobX, labelHeight, uiTextSize, "Resonance", labelAlign);
    addTextKnob(
      left1, top0 + 3 * labelY, knobX, labelHeight, uiTextSize, ID::resonance,
      Scales::resonance, false, 6);

    addLabel(
      left0, top0 + 4 * labelY, knobX, labelHeight, uiTextSize, "DC Block", labelAlign);
    addTextKnob(
      left1, top0 + 4 * labelY, knobX, labelHeight, uiTextSize, ID::dcBlock,
      Scales::dcBlock, false, 6);

    addLabel(
      left0, top0 + 5 * labelY, knobX, labelHeight, uiTextSize, "Highpass", labelAlign);
    addTextKnob(
      left1, top0 + 5 * labelY, knobX, labelHeight, uiTextSize, ID::highpass,
      Scales::highpass, false, 6);

    addCheckbox(
      left0, top0 + 6 * labelY, knobX, labelHeight, uiTextSize, "UniformPeak",
      ID::uniformPeak);
    addCheckbox(
      left0, top0 + 7 * labelY, knobX, labelHeight, uiTextSize, "UniformGain",
      ID::uniformGain);
  }
};

UI *createUI() { return new CV_3PoleLPUI(); }

END_NAMESPACE_DISTRHO
