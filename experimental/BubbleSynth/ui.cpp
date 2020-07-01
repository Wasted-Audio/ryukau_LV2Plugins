// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of BubbleSynth.
//
// BubbleSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BubbleSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BubbleSynth.  If not, see <https://www.gnu.org/licenses/>.

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
constexpr float knobX = 80.0f; // With margin.
constexpr float knobY = knobHeight + labelY;
constexpr float barboxWidth = 4.0f * knobX;
constexpr float barboxHeight = 2.0f * knobY;
constexpr uint32_t defaultWidth = uint32_t(barboxWidth + labelY + 2 * uiMargin);
constexpr uint32_t defaultHeight = uint32_t(labelY + 6 * barboxHeight + 2 * uiMargin);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class BubbleSynthUI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BubbleSynthUI)

public:
  BubbleSynthUI() : PluginUIBase(defaultWidth, defaultHeight)
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

    addGroupLabel(
      left0, top0, labelY + barboxWidth, labelHeight, midTextSize, "BubbleSynth");

    const auto top1 = top0 + labelY;

    addGroupVerticalLabel(left0, top1, barboxHeight, labelHeight, uiTextSize, "Radius");
    addBarBox(
      left0 + labelY, top1, barboxWidth, barboxHeight, ID::radius0, nOscillator,
      Scales::radius);

    addGroupVerticalLabel(
      left0, top1 + 1 * barboxHeight, barboxHeight, labelHeight, uiTextSize, "Gain");
    addBarBox(
      left0 + labelY, top1 + 1 * barboxHeight, barboxWidth, barboxHeight, ID::gain0,
      nOscillator, Scales::gain);

    addGroupVerticalLabel(
      left0, top1 + 2 * barboxHeight, barboxHeight, labelHeight, uiTextSize, "Xi");
    addBarBox(
      left0 + labelY, top1 + 2 * barboxHeight, barboxWidth, barboxHeight, ID::xi0,
      nOscillator, Scales::xi);

    addGroupVerticalLabel(
      left0, top1 + 3 * barboxHeight, barboxHeight, labelHeight, uiTextSize, "Attack");
    addBarBox(
      left0 + labelY, top1 + 3 * barboxHeight, barboxWidth, barboxHeight, ID::attack0,
      nOscillator, Scales::attack);

    addGroupVerticalLabel(
      left0, top1 + 4 * barboxHeight, barboxHeight, labelHeight, uiTextSize, "Decay");
    addBarBox(
      left0 + labelY, top1 + 4 * barboxHeight, barboxWidth, barboxHeight, ID::decay0,
      nOscillator, Scales::decay);

    addGroupVerticalLabel(
      left0, top1 + 5 * barboxHeight, barboxHeight, labelHeight, uiTextSize, "Delay");
    addBarBox(
      left0 + labelY, top1 + 5 * barboxHeight, barboxWidth, barboxHeight, ID::delay0,
      nOscillator, Scales::delay);
  }
};

UI *createUI() { return new BubbleSynthUI(); }

END_NAMESPACE_DISTRHO
