// (c) 2020 Takamitsu Endo
//
// This file is part of CV_StereoGain.
//
// CV_StereoGain is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_StereoGain is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_StereoGain.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"
#include "../../common/dsp/constants.hpp"

using namespace SomeDSP;

DecibelScale<double> Scales::gain(-100, 100, true);
LogScale<double> Scales::dc(0.0, 1.0, 0.5, 0.2);
