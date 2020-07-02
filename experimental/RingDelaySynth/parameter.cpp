// (c) 2020 Takamitsu Endo
//
// This file is part of RingDelaySynth.
//
// RingDelaySynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RingDelaySynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RingDelaySynth.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"
#include "../../common/dsp/constants.hpp"

using namespace SomeDSP;

IntScale<double> Scales::boolScale(1);
LinearScale<double> Scales::defaultScale(0.0, 1.0);

LogScale<double> Scales::frequency(20.0, 10000.0, 0.5, 400.0);

LogScale<double> Scales::gain(0.0, 1.0, 0.5, 0.2);
LogScale<double> Scales::attack(0.0002, 0.01, 0.5, 0.001);
LogScale<double> Scales::decay(0.001, 4.0, 0.5, 1.0);
LogScale<double> Scales::ringHighpassCutoff(20.0, 1000.0, 0.5, 60.0);
LinearScale<double> Scales::feed(-1.0, 1.0);
LogScale<double> Scales::smooth(0.0, 1.0, 0.5, 0.1);
