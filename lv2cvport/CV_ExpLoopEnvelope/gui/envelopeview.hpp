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

#pragma once

#include "../dsp/envelope.hpp"
#include "../parameter.hpp"

#include "Widget.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

class EnvelopeView : public NanoWidget {
public:
  explicit EnvelopeView(NanoWidget *group, FontId fontId)
    : NanoWidget(group), fontId(fontId)
  {
  }

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();

    // Draw envelope.
    auto minDataInt = floorf(minData);
    auto maxDataInt = ceilf(maxData);
    auto range = maxDataInt - minDataInt;
    auto zeroHeight = maxDataInt / range;

    strokeColor(colorZero);
    strokeWidth(1.0f);
    beginPath();
    moveTo(0, height * zeroHeight);
    lineTo(width, height * zeroHeight);
    stroke();

    strokeColor(colorEnvelope);
    strokeWidth(2.0f);
    lineJoin(ROUND);
    beginPath();
    moveTo(marginX, height * zeroHeight);
    for (size_t i = 0; i < data.size(); ++i)
      lineTo(i + marginX, height * (1.0f - (data[i] - minDataInt) / range));
    stroke();

    size_t end = sectionTime.size() > 8 ? 8 : sectionTime.size();
    strokeColor(colorSplitter);
    strokeWidth(1.0f);
    fillColor(colorSplitter);
    fontFaceId(fontId);
    fontSize(textSize);
    textAlign(ALIGN_LEFT | ALIGN_MIDDLE);
    for (size_t i = 0; i < end; ++i) {
      auto left = sectionTime[i] + marginX;
      if (left >= width) continue;
      beginPath();
      moveTo(left, 0);
      lineTo(left, height);
      stroke();

      std::string sectionText = std::to_string(i);
      text(
        left + sectionTextMargin, i * (textSize + 2.0f) + textSize, sectionText.c_str(),
        nullptr);
    }

    auto releaseLeft = releaseAt + marginX;
    if (releaseLeft < width) {
      strokeColor(colorSplitter);
      strokeWidth(1.0f);
      beginPath();
      moveTo(releaseLeft, 0);
      lineTo(releaseLeft, height);
      stroke();

      text(releaseLeft + sectionTextMargin, height - textSize - 2.0f, "R", nullptr);
    }

    // Info text.
    fillColor(colorSplitter);
    fontSize(textSize);
    textAlign(ALIGN_RIGHT | ALIGN_TOP);
    const auto infoLeft = width - 4.0f;
    std::ostringstream osMinMax;
    osMinMax.precision(4);
    osMinMax << std::fixed << "Min=" << std::to_string(gain * minData)
             << ", Max=" << std::to_string(gain * maxData);
    std::string textMinMax = osMinMax.str();
    text(infoLeft, 0, textMinMax.c_str(), nullptr);

    std::string textAttackTime = "Attack=" + std::to_string(attackTime);
    text(infoLeft, textSize, textAttackTime.c_str(), nullptr);

    std::string textLoopTime = "Loop=" + std::to_string(loopTime);
    text(infoLeft, 2 * textSize, textLoopTime.c_str(), nullptr);

    std::string textTotalTime = "Total=" + std::to_string(totalTime);
    text(infoLeft, 3 * textSize, textTotalTime.c_str(), nullptr);

    // Draw border.
    strokeColor(colorFore);
    strokeWidth(2.0f);
    beginPath();
    rect(0, 0, width, height);
    stroke();
  }

  void update(GlobalParameter &param)
  {
    using ID = ParameterID::ID;

    auto loopEnd = param.value[ID::loopEnd]->getInt();
    // if (loopEnd >= 8) loopEnd = 7;
    envelope.setLoop(param.value[ID::loopStart]->getInt(), loopEnd);

    envelope.set(1.0f, param.value[ID::releaseTime]->getFloat());

    envelope.setDecayTime(
      param.value[ID::s0DecayTime]->getFloat(), param.value[ID::s1DecayTime]->getFloat(),
      param.value[ID::s2DecayTime]->getFloat(), param.value[ID::s3DecayTime]->getFloat(),
      param.value[ID::s4DecayTime]->getFloat(), param.value[ID::s5DecayTime]->getFloat(),
      param.value[ID::s6DecayTime]->getFloat(), param.value[ID::s7DecayTime]->getFloat());
    envelope.setHoldTime(
      param.value[ID::s0HoldTime]->getFloat(), param.value[ID::s1HoldTime]->getFloat(),
      param.value[ID::s2HoldTime]->getFloat(), param.value[ID::s3HoldTime]->getFloat(),
      param.value[ID::s4HoldTime]->getFloat(), param.value[ID::s5HoldTime]->getFloat(),
      param.value[ID::s6HoldTime]->getFloat(), param.value[ID::s7HoldTime]->getFloat());
    envelope.setLevel(
      param.value[ID::s0Level]->getFloat(), param.value[ID::s1Level]->getFloat(),
      param.value[ID::s2Level]->getFloat(), param.value[ID::s3Level]->getFloat(),
      param.value[ID::s4Level]->getFloat(), param.value[ID::s5Level]->getFloat(),
      param.value[ID::s6Level]->getFloat(), param.value[ID::s7Level]->getFloat());

    gain = param.value[ID::gain]->getFloat();
    attackTime = envelope.getAttackTime();
    loopTime = envelope.getLoopTime();
    totalTime = attackTime + loopTime + envelope.getReleaseTime();

    auto sampleRate = getWidth() / totalTime;

    auto offset = 2 * marginX;
    data.resize(getWidth() >= offset ? (getWidth() - offset) : 0);
    sectionTime.resize(0);

    envelope.setup(sampleRate);
    envelope.setLoop(loopEnd, loopEnd); // Do not loop while rendering.
    envelope.trigger();
    releaseAt = size_t((attackTime + loopTime) * sampleRate);
    int16_t prevState = -1;
    for (size_t i = 0; i < data.size(); ++i) {
      if (i == releaseAt) envelope.release();
      if (envelope.getState() != prevState) {
        prevState = envelope.getState();
        sectionTime.push_back(i);
      }
      data[i] = 0.95f * gain * envelope.process();
    }
    envelope.terminate();

    // Trim data.
    auto rate = param.value[ID::rate]->getFloat();
    attackTime *= rate;
    loopTime *= rate;
    totalTime *= rate;

    maxData = gain * envelope.getMax();
    minData = gain * envelope.getMin();
    if (minData > maxData) std::swap(minData, maxData);
    if (minData > 0) minData = 0;

    repaint();
  }

  void setForegroundColor(Color color) { colorFore = color; }
  void setBackgroundColor(Color color) { colorBack = color; }
  void setTextSize(float size) { textSize = size < 0.0f ? 0.0f : size; }

protected:
  Color colorFore{0, 0, 0};
  Color colorBack{0xff, 0xff, 0xff};
  Color colorEnvelope{0x10, 0x77, 0xcc};
  Color colorSplitter{0x10, 0x77, 0xcc, 0x88};
  Color colorZero{0xdd, 0xdd, 0xdd};

  FontId fontId = -1;
  float textSize = 14.0f;

  SomeDSP::ExpLoopEnvelope<float> envelope;
  std::vector<float> data;
  std::vector<size_t> sectionTime;
  float gain = 1;
  float maxData = 1;
  float minData = 0;
  float attackTime = 0;
  float loopTime = 1;
  float totalTime = 1;
  size_t marginX = 8;
  size_t releaseAt = 0;
  float sectionTextMargin = 8.0f;
};
