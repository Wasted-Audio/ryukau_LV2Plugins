// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of CV_ExpLoopEnvelope.
//
// CV_ExpLoopEnvelope is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_ExpLoopEnvelope is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_ExpLoopEnvelope.  If not, see <https://www.gnu.org/licenses/>.

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
constexpr uint32_t defaultWidth = uint32_t(10 * knobX + 30);
constexpr uint32_t defaultHeight = uint32_t(11 * labelY + 30);

enum tabIndex { tabMain, tabPadSynth, tabInfo };

class CV_ExpLoopEnvelopeUI : public PluginUIBase {
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

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_ExpLoopEnvelopeUI)

public:
  CV_ExpLoopEnvelopeUI() : PluginUIBase(defaultWidth, defaultHeight)
  {
    param = std::make_unique<GlobalParameter>();

    setGeometryConstraints(defaultWidth, defaultHeight, true, true);

    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);

    using ID = ParameterID::ID;

    const auto top0 = 15.0f;
    const auto left0 = 15.0f;

    const auto top1 = top0 + labelY;
    const auto left1 = left0 + knobX;

    // addGroupLabel(left0, top0, 2 * knobX, labelHeight,
    // uiTextSize,"CV_ExpLoopEnvelope");

    const int labelAlign = ALIGN_LEFT | ALIGN_MIDDLE;

    addLabel(left0, top0, 2 * knobX, labelHeight, midTextSize, "CV_ExpLoopEnvelope");

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
    envelopeView->setSize(8 * knobX - 4 * margin, 7 * labelY - 2 * margin);
    envelopeView->setAbsolutePos(left1 + knobX + 4 * margin, top0);

    const auto leftMatrix0 = left0;
    const auto leftMatrix1 = leftMatrix0 + knobX;
    const auto leftMatrix2 = leftMatrix1 + knobX;
    const auto leftMatrix3 = leftMatrix2 + knobX;
    const auto leftMatrix4 = leftMatrix3 + knobX;
    const auto leftMatrix5 = leftMatrix4 + knobX;
    const auto leftMatrix6 = leftMatrix5 + knobX;
    const auto leftMatrix7 = leftMatrix6 + knobX;
    const auto leftMatrix8 = leftMatrix7 + knobX;
    const auto leftMatrix9 = leftMatrix8 + knobX;

    const auto topMatrix0 = top1 + 6 * labelY;
    const auto topMatrix1 = topMatrix0 + labelY;
    const auto topMatrix2 = topMatrix1 + labelY;
    const auto topMatrix3 = topMatrix2 + labelY;

    addLabel(leftMatrix1, topMatrix0, knobX, labelHeight, uiTextSize, "Section 0");
    addLabel(leftMatrix2, topMatrix0, knobX, labelHeight, uiTextSize, "Section 1");
    addLabel(leftMatrix3, topMatrix0, knobX, labelHeight, uiTextSize, "Section 2");
    addLabel(leftMatrix4, topMatrix0, knobX, labelHeight, uiTextSize, "Section 3");
    addLabel(leftMatrix5, topMatrix0, knobX, labelHeight, uiTextSize, "Section 4");
    addLabel(leftMatrix6, topMatrix0, knobX, labelHeight, uiTextSize, "Section 5");
    addLabel(leftMatrix7, topMatrix0, knobX, labelHeight, uiTextSize, "Section 6");
    addLabel(leftMatrix8, topMatrix0, knobX, labelHeight, uiTextSize, "Section 7");
    addLabel(leftMatrix9, topMatrix0, knobX, labelHeight, uiTextSize, "Release");

    addLabel(leftMatrix0, topMatrix1, knobX, labelHeight, uiTextSize, "Decay [s]");
    addTextKnob(
      leftMatrix1, topMatrix1, knobX, labelHeight, uiTextSize, ID::s0DecayTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix2, topMatrix1, knobX, labelHeight, uiTextSize, ID::s1DecayTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix3, topMatrix1, knobX, labelHeight, uiTextSize, ID::s2DecayTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix4, topMatrix1, knobX, labelHeight, uiTextSize, ID::s3DecayTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix5, topMatrix1, knobX, labelHeight, uiTextSize, ID::s4DecayTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix6, topMatrix1, knobX, labelHeight, uiTextSize, ID::s5DecayTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix7, topMatrix1, knobX, labelHeight, uiTextSize, ID::s6DecayTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix8, topMatrix1, knobX, labelHeight, uiTextSize, ID::s7DecayTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix9, topMatrix1, knobX, labelHeight, uiTextSize, ID::releaseTime,
      Scales::decay, false, 4);

    addLabel(leftMatrix0, topMatrix2, knobX, labelHeight, uiTextSize, "Hold [s]");
    addTextKnob(
      leftMatrix1, topMatrix2, knobX, labelHeight, uiTextSize, ID::s0HoldTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix2, topMatrix2, knobX, labelHeight, uiTextSize, ID::s1HoldTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix3, topMatrix2, knobX, labelHeight, uiTextSize, ID::s2HoldTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix4, topMatrix2, knobX, labelHeight, uiTextSize, ID::s3HoldTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix5, topMatrix2, knobX, labelHeight, uiTextSize, ID::s4HoldTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix6, topMatrix2, knobX, labelHeight, uiTextSize, ID::s5HoldTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix7, topMatrix2, knobX, labelHeight, uiTextSize, ID::s6HoldTime,
      Scales::decay, false, 4);
    addTextKnob(
      leftMatrix8, topMatrix2, knobX, labelHeight, uiTextSize, ID::s7HoldTime,
      Scales::decay, false, 4);

    addLabel(leftMatrix0, topMatrix3, knobX, labelHeight, uiTextSize, "Level");
    addTextKnob(
      leftMatrix1, topMatrix3, knobX, labelHeight, uiTextSize, ID::s0Level, Scales::level,
      false, 4);
    addTextKnob(
      leftMatrix2, topMatrix3, knobX, labelHeight, uiTextSize, ID::s1Level, Scales::level,
      false, 4);
    addTextKnob(
      leftMatrix3, topMatrix3, knobX, labelHeight, uiTextSize, ID::s2Level, Scales::level,
      false, 4);
    addTextKnob(
      leftMatrix4, topMatrix3, knobX, labelHeight, uiTextSize, ID::s3Level, Scales::level,
      false, 4);
    addTextKnob(
      leftMatrix5, topMatrix3, knobX, labelHeight, uiTextSize, ID::s4Level, Scales::level,
      false, 4);
    addTextKnob(
      leftMatrix6, topMatrix3, knobX, labelHeight, uiTextSize, ID::s5Level, Scales::level,
      false, 4);
    addTextKnob(
      leftMatrix7, topMatrix3, knobX, labelHeight, uiTextSize, ID::s6Level, Scales::level,
      false, 4);
    addTextKnob(
      leftMatrix8, topMatrix3, knobX, labelHeight, uiTextSize, ID::s7Level, Scales::level,
      false, 4);
  }
};

UI *createUI() { return new CV_ExpLoopEnvelopeUI(); }

END_NAMESPACE_DISTRHO
