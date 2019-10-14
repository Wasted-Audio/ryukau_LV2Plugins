// (c) 2019 Takamitsu Endo
//
// This file is part of SevenDelay.
//
// SevenDelay is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SevenDelay is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SevenDelay.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cmath>

#include "../dsp/constants.hpp"
#include "Widget.hpp"

class WaveView : public NanoWidget {
public:
  float amount = 0.9;
  float shape = 0.0;
  float phase = 0.0;

  explicit WaveView(NanoWidget *group) : NanoWidget(group) {}

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();

    // Waveform.
    beginPath();
    moveTo(0.0f, height * lfo(0.0f / width));
    const size_t w = (size_t)(width + 1.0);
    for (size_t x = 1; x < w; ++x) lineTo(float(x), height *lfo(float(x) / width));
    strokeColor(colorWave);
    strokeWidth(2.0f);
    stroke();

    // Border.
    beginPath();
    const auto halfBorderWidth = borderWidth / 2.0f;
    rect(halfBorderWidth, halfBorderWidth, width - borderWidth, height - borderWidth);
    strokeColor(colorFore);
    strokeWidth(borderWidth);
    stroke();
  }

  void setForegroundColor(Color color) { colorFore = color; }
  void setWaveformColor(Color color) { colorWave = color; }
  void setBorderWidth(float width) { borderWidth = width < 0.0f ? 0.0f : width; }

protected:
  float lfo(float phase)
  {
    phase = this->phase + phase * 2.0f * float(pi);
    if (phase > 2.0f * float(pi)) phase -= 2.0f * float(pi);
    auto sign = (float(pi) < phase) - (phase < float(pi));
    auto wave = amount * sign * pow(fabs(sin(phase)), shape);
    return (wave + 1.0f) * 0.5f;
  }

  Color colorFore{0, 0, 0};
  Color colorWave{19, 193, 54};

  float borderWidth = 1.0f;
  float textSize = 18.0f;
  FontId fontId = -1;
};
