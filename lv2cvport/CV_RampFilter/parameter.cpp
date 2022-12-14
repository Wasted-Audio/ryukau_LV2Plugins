// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_RampFilter.
//
// CV_RampFilter is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_RampFilter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_RampFilter.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"
#include "../../common/dsp/constants.hpp"

using namespace SomeDSP;

IntScale<double> Scales::boolScale(1);
LogScale<double> Scales::gain(0.0, 16.0, 0.5, 1.0);
LogScale<double> Scales::cutoff(0.0, 48000.0, 0.5, 100.0);
LogScale<double> Scales::resonance(0.0, 1.0, 0.5, 0.1);
LinearScale<double> Scales::bias(-1.0, 1.0);
LinearScale<double> Scales::biasTuning(1.0, 4.0);
LogScale<double> Scales::rampLimit(0.1, 10.0, 0.5, 2.0);
