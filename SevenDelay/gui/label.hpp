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

#include "TinosBoldItalic.hpp"
#include "Widget.hpp"

class Label : public NanoWidget {
public:
  bool drawBorder = false;
  bool drawBackground = false;
  bool drawUnderline = false;

  explicit Label(NanoWidget *group, const char *labelText)
    : NanoWidget(group), labelText(labelText)
  {
    fontId = createFontFromMemory(
      "sans", (unsigned char *)(TinosBoldItalic::TinosBoldItalicData),
      TinosBoldItalic::TinosBoldItalicDataSize, false);
  }

  void onNanoDisplay() override
  {
    resetTransform();
    translate(getAbsoluteX(), getAbsoluteY());

    const auto width = getWidth();
    const auto height = getHeight();

    // Border.
    beginPath();
    const auto halfBorderWidth = borderWidth / 2.0f;
    rect(halfBorderWidth, halfBorderWidth, width - borderWidth, height - borderWidth);
    if (drawBackground) {
      fillColor(colorBack);
      fill();
    }
    if (drawBorder) {
      strokeColor(colorFore);
      strokeWidth(borderWidth);
      stroke();
    }

    if (drawUnderline) {
      beginPath();
      moveTo(0, height / 2);
      lineTo(width / 2 - 60.0f, height / 2);
      moveTo(width / 2 + 60.0f, height / 2);
      lineTo(width, height / 2);
      strokeColor(colorFore);
      strokeWidth(borderWidth);
      stroke();
    }

    // Text.
    if (labelText == nullptr) return;
    fillColor(colorFore);
    fontFaceId(fontId);
    fontSize(textSize);
    textAlign(align);

    float labelX = (align & ALIGN_LEFT) ? 0 : (align & ALIGN_RIGHT) ? width : width / 2;
    float labelY = height / 2;
    text(labelX, labelY, labelText, nullptr);
  }

  void setForegroundColor(Color color) { colorFore = color; }
  void setBackgroundColor(Color color) { colorBack = color; }
  void setBorderWidth(float width) { borderWidth = width < 0.0f ? 0.0f : width; }
  void setTextAlign(int align) { this->align = align; }
  void setTextSize(float size) { textSize = size < 0.0f ? 0.0f : size; }

protected:
  Color colorFore{0, 0, 0};
  Color colorBack{0xff, 0xff, 0xff};

  const char *labelText = nullptr;
  int align = ALIGN_CENTER | ALIGN_MIDDLE;
  float borderWidth = 1.0f;
  float textSize = 18.0f;
  FontId fontId = -1;
};
