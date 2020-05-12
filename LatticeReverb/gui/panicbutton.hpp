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

class PanicButton : public Button {
public:
  explicit PanicButton(
    NanoWidget *group, PluginUI *ui, std::string labelText, FontId fontId)
    : Button(group, ui, labelText, fontId)
  {
  }

  virtual bool onMouse(const MouseEvent &ev) override
  {
    using ID = ParameterID::ID;

    if (contains(ev.pos)) {
      value = ev.press;

      ui->updateValue(ID::innerFeedMultiply, 0.0f);
      ui->updateValue(ID::outerFeedMultiply, 0.0f);

      ui->updateUI(ID::innerFeedMultiply, 0.0f);
      ui->updateUI(ID::outerFeedMultiply, 0.0f);

      repaint();
      return true;
    } else if (!ev.press) {
      value = false;
      repaint();
    }
    return false;
  }

  virtual bool onMotion(const MotionEvent &ev) override
  {
    isMouseEntered = contains(ev.pos);
    repaint();
    return false;
  }

  virtual bool onScroll(const ScrollEvent & /*ev*/) override { return false; }
};
