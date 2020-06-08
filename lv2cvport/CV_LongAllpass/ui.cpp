// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_LongAllpass.
//
// CV_LongAllpass is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_LongAllpass is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_LongAllpass.  If not, see <https://www.gnu.org/licenses/>.

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
constexpr uint32_t defaultHeight = uint32_t(labelHeight + 2 * labelY + 30);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class CV_LongAllpassUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_LongAllpassUI)

public:
  CV_LongAllpassUI() : PluginUIBase(defaultWidth, defaultHeight)
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

    addGroupLabel(left0, top0, 2 * knobX, labelHeight, midTextSize, "CV_LongAllpass");

    const int labelAlign = ALIGN_LEFT | ALIGN_MIDDLE;

    addLabel(
      left0, top0 + labelY, knobX, labelHeight, uiTextSize, "Time [s]", labelAlign);
    addTextKnob(
      left1, top0 + labelY, knobX, labelHeight, uiTextSize, ID::time, Scales::time, false,
      5);

    addLabel(
      left0, top0 + 2 * labelY, knobX, labelHeight, uiTextSize, "Feedback", labelAlign);
    addTextKnob(
      left1, top0 + 2 * labelY, knobX, labelHeight, uiTextSize, ID::feedback,
      Scales::feedback, false, 5);
  }
};

UI *createUI() { return new CV_LongAllpassUI(); }

END_NAMESPACE_DISTRHO
