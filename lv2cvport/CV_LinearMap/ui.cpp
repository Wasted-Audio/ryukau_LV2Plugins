// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of CV_LinearMap.
//
// CV_LinearMap is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_LinearMap is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_LinearMap.  If not, see <https://www.gnu.org/licenses/>.

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
constexpr uint32_t defaultWidth = uint32_t(3 * knobX + 30);
constexpr uint32_t defaultHeight = uint32_t(labelHeight + 4 * labelY + 30);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class CV_LinearMapUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_LinearMapUI)

public:
  CV_LinearMapUI() : PluginUIBase(defaultWidth, defaultHeight)
  {
    param = std::make_unique<GlobalParameter>();

    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    using ID = ParameterID::ID;

    const auto top0 = 15.0f;
    const auto top1 = top0 + labelY;
    const auto top2 = top1 + labelY;
    const auto top3 = top2 + labelY;
    const auto left0 = 15.0f;
    const auto left1 = left0 + knobX;
    const auto left2 = left1 + knobX;

    addGroupLabel(left0, top0, 3 * knobX, labelHeight, midTextSize, "CV_LinearMap");

    const int labelAlign = ALIGN_LEFT | ALIGN_MIDDLE;

    addLabel(left1, top1, knobX, labelHeight, uiTextSize, "Integer", labelAlign);
    addLabel(left2, top1, knobX, labelHeight, uiTextSize, "Fraction", labelAlign);

    addLabel(left0, top2, knobX, labelHeight, uiTextSize, "Mul", labelAlign);
    addTextKnob(
      left1, top2, knobX, labelHeight, uiTextSize, ID::mulInt, Scales::integer, false, 0,
      -128);
    addTextKnob(
      left2, top2, knobX, labelHeight, uiTextSize, ID::mulFrac, Scales::fraction, false,
      5);

    addLabel(left0, top3, knobX, labelHeight, uiTextSize, "Add", labelAlign);
    addTextKnob(
      left1, top3, knobX, labelHeight, uiTextSize, ID::addInt, Scales::integer, false, 0,
      -128);
    addTextKnob(
      left2, top3, knobX, labelHeight, uiTextSize, ID::addFrac, Scales::fraction, false,
      5);

    addLabel(
      left0, top0 + 4 * labelY, 3 * knobX, labelHeight, uiTextSize,
      "Output = Add + Mul * Input.", labelAlign);
  }
};

UI *createUI() { return new CV_LinearMapUI(); }

END_NAMESPACE_DISTRHO
