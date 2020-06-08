// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_PTRTrapezoid.
//
// CV_PTRTrapezoid is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_PTRTrapezoid is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_PTRTrapezoid.  If not, see <https://www.gnu.org/licenses/>.

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
constexpr uint32_t defaultHeight = uint32_t(labelHeight + 7 * labelY + 40);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class CV_PTRTrapezoidUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_PTRTrapezoidUI)

public:
  CV_PTRTrapezoidUI() : PluginUIBase(defaultWidth, defaultHeight)
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
    const auto left1 = 20.0f + knobX;

    addGroupLabel(left0, top0, 2 * knobX, labelHeight, midTextSize, "CV_PTRTrapezoid");

    const int labelAlign = ALIGN_LEFT | ALIGN_MIDDLE;

    addLabel(left0, top0 + labelY, knobX, labelHeight, uiTextSize, "Gain", labelAlign);
    addTextKnob(
      left1, top0 + labelY, knobX, labelHeight, uiTextSize, ID::gain, Scales::gain, false,
      3);

    addLabel(
      left0, top0 + 2 * labelY, knobX, labelHeight, uiTextSize, "Boost", labelAlign);
    addTextKnob(
      left1, top0 + 2 * labelY, knobX, labelHeight, uiTextSize, ID::boost,
      Scales::defaultScale, false, 3);

    addLabel(
      left0, top0 + 3 * labelY, knobX, labelHeight, uiTextSize, "PulseWidth", labelAlign);
    addTextKnob(
      left1, top0 + 3 * labelY, knobX, labelHeight, uiTextSize, ID::pulseWidth,
      Scales::defaultScale, false, 3);

    addLabel(
      left0, top0 + 4 * labelY, knobX, labelHeight, uiTextSize, "Slope", labelAlign);
    addTextKnob(
      left1, top0 + 4 * labelY, knobX, labelHeight, uiTextSize, ID::slope,
      Scales::oscSlope, false, 3);

    addLabel(
      left0, top0 + 5 * labelY, knobX, labelHeight, uiTextSize, "SlopeCV", labelAlign);
    addTextKnob(
      left1, top0 + 5 * labelY, knobX, labelHeight, uiTextSize, ID::slopeMultiply,
      Scales::slopeMultiply, false, 3);

    addLabel(
      left0, top0 + 6 * labelY, knobX, labelHeight, uiTextSize, "Semi", labelAlign);
    addTextKnob(
      left1, top0 + 6 * labelY, knobX, labelHeight, uiTextSize, ID::oscSemi,
      Scales::oscSemi, false, 0, -60);

    addLabel(
      left0, top0 + 7 * labelY, knobX, labelHeight, uiTextSize, "Milli", labelAlign);
    addTextKnob(
      left1, top0 + 7 * labelY, knobX, labelHeight, uiTextSize, ID::oscMilli,
      Scales::oscMilli);
  }
};

UI *createUI() { return new CV_PTRTrapezoidUI(); }

END_NAMESPACE_DISTRHO
