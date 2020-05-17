// (c) 2020 Takamitsu Endo
//
// This file is part of CV_PolyLoopEnvelope2.
//
// CV_PolyLoopEnvelope2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_PolyLoopEnvelope2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_PolyLoopEnvelope2.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"
#include "../../common/dsp/constants.hpp"

using namespace SomeDSP;

IntScale<double> Scales::boolScale(1);
LinearScale<double> Scales::defaultScale(0.0, 1.0);

LogScale<double> Scales::rate(0.01, 440.0, 0.5, 1.0);
LogScale<double> Scales::rateSlideTime(0.0, 1.0, 0.5, 0.04);
IntScale<double> Scales::section(2);
LinearScale<double> Scales::level(-1.0, 1.0);
LogScale<double> Scales::decay(0.0, 16.0, 0.5, 2.0);
LinearScale<double> Scales::curve(-16.0, 16.0);
