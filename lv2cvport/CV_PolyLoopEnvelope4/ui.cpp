// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of CV_PolyLoopEnvelope4.
//
// CV_PolyLoopEnvelope4 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_PolyLoopEnvelope4 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_PolyLoopEnvelope4.  If not, see <https://www.gnu.org/licenses/>.

#include "../../common/uibase.hpp"
#include "parameter.hpp"

#include "gui/envelopeview.hpp"

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
constexpr uint32_t defaultWidth = uint32_t(6 * knobX + 30);
constexpr uint32_t defaultHeight = uint32_t(12 * labelY + 30);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class CV_PolyLoopEnvelope4UI : public PluginUIBase {
protected:
  std::shared_ptr<EnvelopeView> envelopeView;

  void onNanoDisplay() override
  {
    envelopeView->update(param);

    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(palette.background());
    fill();
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_PolyLoopEnvelope4UI)

public:
  CV_PolyLoopEnvelope4UI() : PluginUIBase(defaultWidth, defaultHeight)
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

    const auto top1 = top0 + labelY;
    const auto left1 = left0 + knobX;

    const int labelAlign = ALIGN_LEFT | ALIGN_MIDDLE;

    addLabel(left0, top0, 2 * knobX, labelHeight, midTextSize, "CV_PolyLoopEnvelope4");

    addLabel(left0, top1, knobX, labelHeight, uiTextSize, "Gain", labelAlign);
    addTextKnob(
      left1, top1, knobX, labelHeight, uiTextSize, ID::gain, Scales::level, false, 4);

    addLabel(
      left0, top1 + 1 * labelY, knobX, labelHeight, uiTextSize, "Loop Start", labelAlign);
    addTextKnob(
      left1, top1 + 1 * labelY, knobX, labelHeight, uiTextSize, ID::loopStart,
      Scales::section);

    addLabel(
      left0, top1 + 2 * labelY, knobX, labelHeight, uiTextSize, "Loop End", labelAlign);
    addTextKnob(
      left1, top1 + 2 * labelY, knobX, labelHeight, uiTextSize, ID::loopEnd,
      Scales::section);

    addLabel(
      left0, top1 + 3 * labelY, knobX, labelHeight, uiTextSize, "Rate", labelAlign);
    addTextKnob(
      left1, top1 + 3 * labelY, knobX, labelHeight, uiTextSize, ID::rate, Scales::rate,
      false, 2);

    addLabel(
      left0, top1 + 4 * labelY, knobX, labelHeight, uiTextSize, "Slide [s]", labelAlign);
    addTextKnob(
      left1, top1 + 4 * labelY, knobX, labelHeight, uiTextSize, ID::rateSlideTime,
      Scales::rateSlideTime, false, 5);

    addCheckbox(
      left0, top1 + 5 * labelY, 2 * knobX, labelHeight, uiTextSize, "Rate Key Follow",
      ID::rateKeyFollow);

    envelopeView = std::make_shared<EnvelopeView>(this, fontId, palette);
    envelopeView->setSize(4 * knobX - 4 * margin, 7 * labelY - 2 * margin);
    envelopeView->setAbsolutePos(left1 + knobX + 4 * margin, top0);

    constexpr size_t nEnvelopeSection = 4;

    const auto leftMatrix0 = left0;
    const std::array<float, nEnvelopeSection> leftMatrix = {
      left0 + 1.0f * knobX,
      left0 + 2.0f * knobX,
      left0 + 3.0f * knobX,
      left0 + 4.0f * knobX,
    };
    const auto leftMatrixRelease = left0 + 5.0f * knobX;

    const auto topMatrix0 = top1 + 6 * labelY;
    const auto topMatrix1 = topMatrix0 + labelY;
    const auto topMatrix2 = topMatrix1 + labelY;
    const auto topMatrix3 = topMatrix2 + labelY;
    const auto topMatrix4 = topMatrix3 + labelY;

    addLabel(leftMatrix0, topMatrix1, knobX, labelHeight, uiTextSize, "Decay [s]");
    addLabel(leftMatrix0, topMatrix2, knobX, labelHeight, uiTextSize, "Hold [s]");
    addLabel(leftMatrix0, topMatrix3, knobX, labelHeight, uiTextSize, "Level");
    addLabel(leftMatrix0, topMatrix4, knobX, labelHeight, uiTextSize, "Curve");

    std::string sectionLabel("Section ");
    for (size_t idx = 0; idx < nEnvelopeSection; ++idx) {
      auto indexStr = std::to_string(idx);
      addLabel(
        leftMatrix[idx], topMatrix0, knobX, labelHeight, uiTextSize,
        sectionLabel + indexStr);
      addTextKnob(
        leftMatrix[idx], topMatrix1, knobX, labelHeight, uiTextSize,
        ID::s0DecayTime + idx, Scales::decay, false, 4);
      addTextKnob(
        leftMatrix[idx], topMatrix2, knobX, labelHeight, uiTextSize, ID::s0HoldTime + idx,
        Scales::decay, false, 4);
      addTextKnob(
        leftMatrix[idx], topMatrix3, knobX, labelHeight, uiTextSize, ID::s0Level + idx,
        Scales::level, false, 4);
      addTextKnob(
        leftMatrix[idx], topMatrix4, knobX, labelHeight, uiTextSize, ID::s0Curve + idx,
        Scales::curve, false, 4);
    }

    addLabel(leftMatrixRelease, topMatrix0, knobX, labelHeight, uiTextSize, "Release");
    addTextKnob(
      leftMatrixRelease, topMatrix1, knobX, labelHeight, uiTextSize, ID::releaseTime,
      Scales::decay, false, 4);
    auto knob = addTextKnob(
      leftMatrixRelease, topMatrix4, knobX, labelHeight, uiTextSize, ID::releaseCurve,
      Scales::curve, false, 4);
  }
};

UI *createUI() { return new CV_PolyLoopEnvelope4UI(); }

END_NAMESPACE_DISTRHO
