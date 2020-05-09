// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_NestedLongAllpass.
//
// CV_NestedLongAllpass is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_NestedLongAllpass is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_NestedLongAllpass.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/constants.hpp"
#include "../../../common/dsp/smoother.hpp"
#include "../parameter.hpp"

#include "delay.hpp"

#include <array>

using namespace SomeDSP;

class DSPCore {
public:
  static const size_t maxVoice = 32;
  GlobalParameter param;

  void setup(double sampleRate);
  void reset();   // Stop sounds.
  void startup(); // Reset phase etc.
  void setParameters();
  void process(const size_t length, const float *in0, float *out0);

private:
  float sampleRate = 44100.0f;
  NestedSchroeder<float, nestingDepth> delay;
  std::array<LinearSmoother<float>, nestingDepth> interpTime;
  std::array<LinearSmoother<float>, nestingDepth> interpOuterFeed;
  std::array<LinearSmoother<float>, nestingDepth> interpInnerFeed;
};
