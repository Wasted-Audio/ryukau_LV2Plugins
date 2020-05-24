// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_RampFilter.
//
// CV_RampFilter is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_RampFilter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_RampFilter.  If not, see <https://www.gnu.org/licenses/>.

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

class CV_RampFilterUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_RampFilterUI)

public:
  CV_RampFilterUI() : PluginUIBase(defaultWidth, defaultHeight)
  {
    param = std::make_unique<GlobalParameter>();

    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    using ID = ParameterID::ID;

    const auto top0 = 15.0f;
    const auto left0 = 20.0f;
    const auto left1 = 20.0f + knobX;

    addGroupLabel(left0, top0, 2 * knobX, labelHeight, midTextSize, "CV_RampFilter");

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
      left0, top0 + 4 * labelY, knobX, labelHeight, uiTextSize, "RampLimit", labelAlign);
    addTextKnob(
      left1, top0 + 4 * labelY, knobX, labelHeight, uiTextSize, ID::rampLimit,
      Scales::rampLimit, false, 6);

    addLabel(
      left0, top0 + 5 * labelY, knobX, labelHeight, uiTextSize, "Bias", labelAlign);
    addTextKnob<Style::warning>(
      left1, top0 + 5 * labelY, knobX, labelHeight, uiTextSize, ID::bias, Scales::bias,
      false, 6);

    addLabel(
      left0, top0 + 6 * labelY, knobX, labelHeight, uiTextSize, "BiasTuning", labelAlign);
    addTextKnob<Style::warning>(
      left1, top0 + 6 * labelY, knobX, labelHeight, uiTextSize, ID::biasTuning,
      Scales::biasTuning, false, 6);

    addCheckbox(
      left0, top0 + 7 * labelY, knobX, labelHeight, uiTextSize, "Highpass", ID::highpass);
    addCheckbox(
      left1, top0 + 7 * labelY, knobX, labelHeight, uiTextSize, "OverSampling",
      ID::overSampling);
  }
};

UI *createUI() { return new CV_RampFilterUI(); }

END_NAMESPACE_DISTRHO
