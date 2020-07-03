// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of NoiseTester.
//
// NoiseTester is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// NoiseTester is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with NoiseTester.  If not, see <https://www.gnu.org/licenses/>.

#include "../../common/uibase.hpp"
#include "parameter.hpp"

#include <sstream>
#include <tuple>

START_NAMESPACE_DISTRHO

constexpr float uiTextSize = 14.0f;
constexpr float midTextSize = 16.0f;
constexpr float pluginNameTextSize = 22.0f;
constexpr float uiMargin = 15.0f;
constexpr float margin = 5.0f;
constexpr float labelHeight = 20.0f;
constexpr float labelY = 30.0f;
constexpr float knobWidth = 50.0f;
constexpr float knobHeight = 40.0f;
constexpr float knobX = 60.0f; // With margin.
constexpr float knobY = knobHeight + labelY;
constexpr float barboxWidth = 16.0f * 20.0f;
constexpr float barboxHeight = 2.0f * knobY;
constexpr float innerWidth = 6 * knobX;
constexpr uint32_t defaultWidth = uint32_t(innerWidth + 2 * uiMargin);
constexpr uint32_t defaultHeight = uint32_t(2 * knobY + 2 * uiMargin);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class NoiseTesterUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseTesterUI)

public:
  NoiseTesterUI() : PluginUIBase(defaultWidth, defaultHeight)
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

    const auto top0 = uiMargin;
    const auto left0 = uiMargin;

    addKnob(left0 + 0 * knobX, top0, knobWidth, margin, uiTextSize, "Gain", ID::gain);
    addKnob(left0 + 1 * knobX, top0, knobWidth, margin, uiTextSize, "Smooth", ID::smooth);
    addKnob(left0 + 2 * knobX, top0, knobWidth, margin, uiTextSize, "Attack", ID::attack);
    addKnob(
      left0 + 3 * knobX, top0, knobWidth, margin, uiTextSize, "Release", ID::release);

    const auto top1 = top0 + knobY;

    addLabel(left0 + 0 * knobX, top1, knobX, labelHeight, uiTextSize, "Seed");
    addTextKnob(
      left0 + 1 * knobX, top1, knobX, labelHeight, uiTextSize, ID::seed, Scales::seed);

    addLabel(left0 + 0 * knobX, top1 + labelY, knobX, labelHeight, uiTextSize, "Type");
    std::vector<std::string> typeItems{"Gaussian", "Uniform"};
    addOptionMenu(
      left0 + 1 * knobX, top1 + labelY, knobX, labelHeight, uiTextSize, ID::type,
      typeItems);

    addCheckbox(
      left0 + 3 * knobX, top1, knobX, labelHeight, uiTextSize, "Hardclip", ID::hardclip);
  }
};

UI *createUI() { return new NoiseTesterUI(); }

END_NAMESPACE_DISTRHO
