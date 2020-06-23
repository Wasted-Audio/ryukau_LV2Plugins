// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_Gate16.
//
// CV_Gate16 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_Gate16 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_Gate16.  If not, see <https://www.gnu.org/licenses/>.

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
constexpr float barboxHeight = labelHeight + 5 * labelY;
constexpr uint32_t defaultWidth = uint32_t(5 * knobX + 10 * margin + 40);
constexpr uint32_t defaultHeight
  = uint32_t(5 * labelY + 2 * barboxHeight + 2 * margin + 40);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class CV_Gate16UI : public PluginUIBase {
protected:
  void onNanoDisplay() override
  {
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_Gate16UI)

public:
  CV_Gate16UI() : PluginUIBase(defaultWidth, defaultHeight)
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

    // Gain section.
    addGroupLabel(left0, top0, barboxWidth, labelHeight, midTextSize, "Gain");
    addLabel(left0, top0 + labelY, knobX, labelHeight, uiTextSize, "MasterGain");
    addTextKnob(
      left0 + knobX, top0 + labelY, knobX, labelHeight, uiTextSize, ID::masterGain,
      Scales::masterGain, false, 4);
    auto barboxGain = addBarBox(
      left0, top0 + 2 * labelY, barboxWidth, barboxHeight, ID::gain1, 16, Scales::gain);
    barboxGain->indexOffset = 1;

    // Delay section.
    const auto top1 = top0 + 8 * labelY + 2 * margin;
    addGroupLabel(left0, top1, barboxWidth, labelHeight, midTextSize, "Delay");

    addLabel(left0, top1 + labelY, knobX, labelHeight, uiTextSize, "Multiply");
    addTextKnob(
      left0 + knobX, top1 + labelY, knobX, labelHeight, uiTextSize, ID::delayMultiply,
      Scales::delayMultiply, false, 4);

    addCheckbox(
      left0 + 2.5 * knobX, top1 + labelY, knobX, labelHeight, uiTextSize, "TempoSync",
      ID::delayTempoSync);

    auto barboxDelay = addBarBox(
      left0, top1 + 2 * labelY, barboxWidth, barboxHeight, ID::delay1, 16, Scales::delay);
    barboxDelay->indexOffset = 1;
    barboxDelay->altScrollSensitivity = 1.0 / 64.0;

    // Plugin name.
    const auto top2 = top1 + 8 * labelY + 2 * margin;
    std::stringstream ssPluginName;
    ssPluginName << "CV_Gate16  " << std::to_string(MAJOR_VERSION) << "."
                 << std::to_string(MINOR_VERSION) << "." << std::to_string(PATCH_VERSION);
    addTextView(left0, top2, barboxWidth, labelY, midTextSize, ssPluginName.str());

    // Type section.
    const auto left1 = left0 + barboxWidth + 4 * margin;
    const auto left2 = left1 + 6 * margin;
    std::string typeLabel("Type");
    addGroupLabel(left1, top0, labelY + knobX, labelHeight, midTextSize, "Type");
    std::vector<std::string> typeItems{"Trigger", "Gate", "DC"};
    for (size_t idx = 0; idx < nGate; ++idx) {
      const auto fixed = idx + 1;
      addLabel(
        left1, top0 + fixed * labelY, 4 * margin, labelHeight, uiTextSize,
        std::to_string(fixed), ALIGN_RIGHT | ALIGN_MIDDLE);
      addOptionMenu(
        left2, top0 + fixed * labelY, knobX, labelHeight, uiTextSize, ID::type1 + idx,
        typeItems);
    }
  }
};

UI *createUI() { return new CV_Gate16UI(); }

END_NAMESPACE_DISTRHO
