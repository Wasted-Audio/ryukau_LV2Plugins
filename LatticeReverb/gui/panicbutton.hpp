// (c) 2020 Takamitsu Endo
//
// This file is part of LatticeReverb.
//
// LatticeReverb is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LatticeReverb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LatticeReverb.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../common/gui/button.hpp"
#include "../parameter.hpp"

class PanicButton : public ButtonBase<Style::common> {
public:
  using Btn = ButtonBase<Style::common>;

  explicit PanicButton(
    NanoWidget *group,
    PluginUI *ui,
    std::string labelText,
    NanoVG::FontId fontId,
    Palette &palette)
    : ButtonBase<Style::common>(group, ui, labelText, fontId, palette)
  {
  }

  virtual bool onMouse(const Widget::MouseEvent &ev) override
  {
    using ID = ParameterID::ID;

    if (Btn::contains(ev.pos)) {
      Btn::value = ev.press;
      Btn::repaint();
      if (ev.press == true) {
        ui->updateValue(ID::timeMultiply, 0.0f);
        ui->updateValue(ID::innerFeedMultiply, 0.0f);
        ui->updateValue(ID::outerFeedMultiply, 0.0f);

        ui->updateUI(ID::timeMultiply, 0.0f);
        ui->updateUI(ID::innerFeedMultiply, 0.0f);
        ui->updateUI(ID::outerFeedMultiply, 0.0f);
        return true;
      }
    } else if (!ev.press) {
      Btn::value = false;
      Btn::repaint();
    }
    return false;
  }

  virtual bool onMotion(const Widget::MotionEvent &ev) override
  {
    Btn::isMouseEntered = Btn::contains(ev.pos);
    Btn::repaint();
    return false;
  }

  virtual bool onScroll(const Widget::ScrollEvent & /*ev*/) override { return false; }
};
