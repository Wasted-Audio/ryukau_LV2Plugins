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
  explicit EnvelopeView(NanoWidget *group, FontId fontId, Palette &palette)
    : NanoWidget(group), fontId(fontId), pal(palette)
  {
  }

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();

    // Background.
    beginPath();
    rect(0, 0, width, height);
    fillColor(pal.boxBackground());
    fill();

    // Draw envelope.
    auto minDataInt = floorf(minData);
    auto maxDataInt = ceilf(maxData);
    auto range = maxDataInt - minDataInt;
    auto zeroHeight = maxDataInt / range;

    strokeColor(pal.overlay());
    strokeWidth(1.0f);
    beginPath();
    moveTo(0, height * zeroHeight);
    lineTo(width, height * zeroHeight);
    stroke();

    strokeColor(pal.highlightMain());
    strokeWidth(2.0f);
    lineJoin(ROUND);
    beginPath();
    moveTo(marginX, height * zeroHeight);
    for (size_t i = 0; i < data.size(); ++i)
      lineTo(i + marginX, height * (1.0f - (data[i] - minDataInt) / range));
    stroke();

    size_t end = sectionTime.size() > 8 ? 8 : sectionTime.size();
    strokeColor(pal.overlay());
    strokeWidth(1.0f);
    fillColor(pal.overlay());
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
      strokeColor(pal.overlay());
      strokeWidth(1.0f);
      beginPath();
      moveTo(releaseLeft, 0);
      lineTo(releaseLeft, height);
      stroke();

      text(releaseLeft + sectionTextMargin, height - textSize - 2.0f, "R", nullptr);
    }

    // Info text.
    fillColor(pal.overlay());
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

    if (isCvGainReady) {
      fillColor(pal.highlightWarning());
      text(infoLeft, 4 * textSize, "CV Gain Ready", nullptr);
    }

    // Draw border.
    strokeColor(pal.border());
    strokeWidth(2.0f);
    beginPath();
    rect(0, 0, width, height);
    stroke();
  }

  void update(std::unique_ptr<ParameterInterface> const &param)
  {
    using ID = ParameterID::ID;

    auto loopEnd = param->getInt(ID::loopEnd);
    envelope.setLoop(param->getInt(ID::loopStart), loopEnd);

    envelope.set(
      1.0f, param->getFloat(ID::releaseTime), param->getFloat(ID::releaseCurve),
      {
        float(param->getFloat(ID::s0DecayTime)),
        float(param->getFloat(ID::s1DecayTime)),
        float(param->getFloat(ID::s2DecayTime)),
        float(param->getFloat(ID::s3DecayTime)),
      },
      {
        float(param->getFloat(ID::s0HoldTime)),
        float(param->getFloat(ID::s1HoldTime)),
        float(param->getFloat(ID::s2HoldTime)),
        float(param->getFloat(ID::s3HoldTime)),
      },
      {
        float(param->getFloat(ID::s0Level)),
        float(param->getFloat(ID::s1Level)),
        float(param->getFloat(ID::s2Level)),
        float(param->getFloat(ID::s3Level)),
      },
      {
        float(param->getFloat(ID::s0Curve)),
        float(param->getFloat(ID::s1Curve)),
        float(param->getFloat(ID::s2Curve)),
        float(param->getFloat(ID::s3Curve)),
      });

    gain = param->getFloat(ID::gain);
    isCvGainReady = gain == 0;
    if (isCvGainReady) gain = 1; // For convenience when using CV gain port.

    attackTime = envelope.getAttackTime();
    loopTime = envelope.getLoopTime();
    totalTime = attackTime + loopTime + envelope.getReleaseTime();

    float sampleRate = getWidth() / totalTime;
    if (sampleRate > 1e6) sampleRate = 1e6;

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

    if (sectionTime.size() > envelope.nSections) sectionTime.resize(envelope.nSections);

    // Trim data.
    auto rate = param->getFloat(ID::rate);
    attackTime *= rate;
    loopTime *= rate;
    totalTime *= rate;

    maxData = gain * envelope.getMax();
    minData = gain * envelope.getMin();
    if (minData > maxData) std::swap(minData, maxData);
    if (minData > 0) minData = 0;

    repaint();
  }

  void setTextSize(float size) { textSize = size < 0.0f ? 0.0f : size; }

protected:
  FontId fontId = -1;
  Palette &pal;
  float textSize = 14.0f;

  SomeDSP::PolyLoopEnvelope<float> envelope;
  std::vector<float> data;
  std::vector<size_t> sectionTime;
  float gain = 1;
  float maxData = 1;
  float minData = 0;
  float attackTime = 0;
  float loopTime = 1;
  float totalTime = 1;
  bool isCvGainReady = false;
  size_t marginX = 8;
  size_t releaseAt = 0;
  float sectionTextMargin = 8.0f;
};
