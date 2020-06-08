// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_NestedLongAllpass.
//
// CV_NestedLongAllpass is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_NestedLongAllpass is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_NestedLongAllpass.  If not, see <https://www.gnu.org/licenses/>.

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
constexpr float barboxWidth = 4 * knobX;
constexpr float barboxHeight = 2 * knobY;
constexpr uint32_t defaultWidth
  = uint32_t(labelY + barboxWidth + knobX + 2 * margin + 30);
constexpr uint32_t defaultHeight
  = uint32_t(labelY + 3 * barboxHeight + 2 * labelHeight + 6 * margin + 30);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class CV_NestedLongAllpassUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_NestedLongAllpassUI)

public:
  CV_NestedLongAllpassUI() : PluginUIBase(defaultWidth, defaultHeight)
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
    const auto left0 = 15.0f;
    const auto uiWidth = labelY + barboxWidth + knobX + 2 * margin;

    addGroupLabel(left0, top0, uiWidth, labelHeight, midTextSize, "CV_NestedLongAllpass");

    const auto top1 = 15.0f + labelY;
    const auto top2 = top1 + barboxHeight + 2 * margin;
    const auto top3 = top2 + barboxHeight + 2 * margin;
    const auto top4 = top3 + barboxHeight + 2 * margin;
    const auto left1 = 15.0f + labelY;
    const auto left2 = left1 + barboxWidth + 2 * margin;

    addGroupVerticalLabel(
      left0, top1, barboxHeight, labelHeight, midTextSize, "Time [s]");
    addBarBox(left1, top1, barboxWidth, barboxHeight, ID::time0, 8, Scales::time);

    addLabel(left2, top1, knobX, labelHeight, uiTextSize, "Multiply");
    addTextKnob(
      left2, top1 + labelY, knobX, labelHeight, uiTextSize, ID::timeMultiply,
      Scales::multiply, false, 4);

    addGroupVerticalLabel(
      left0, top2, barboxHeight, labelHeight, midTextSize, "OuterFeed");
    auto barboxOuterFeed = addBarBox(
      left1, top2, barboxWidth, barboxHeight, ID::outerFeed0, 8, Scales::feed);
    barboxOuterFeed->sliderZero = 0.5f;

    addLabel(left2, top2, knobX, labelHeight, uiTextSize, "Multiply");
    addTextKnob(
      left2, top2 + labelY, knobX, labelHeight, uiTextSize, ID::outerFeedMultiply,
      Scales::multiply, false, 4);

    addGroupVerticalLabel(
      left0, top3, barboxHeight, labelHeight, midTextSize, "InnerFeed");
    auto barboxInnerFeed = addBarBox(
      left1, top3, barboxWidth, barboxHeight, ID::innerFeed0, 8, Scales::feed);
    barboxInnerFeed->sliderZero = 0.5f;

    addLabel(left2, top3, knobX, labelHeight, uiTextSize, "Multiply");
    addTextKnob(
      left2, top3 + labelY, knobX, labelHeight, uiTextSize, ID::innerFeedMultiply,
      Scales::multiply, false, 4);

    addLabel(left1, top4, knobX, labelHeight, uiTextSize, "Smoothness");
    addTextKnob(
      left1 + knobX, top4, knobX, labelHeight, uiTextSize, ID::smoothness,
      Scales::smoothness, false, 4);
  }
};

UI *createUI() { return new CV_NestedLongAllpassUI(); }

END_NAMESPACE_DISTRHO
