// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_3PoleLP.
//
// CV_3PoleLP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_3PoleLP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_3PoleLP.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"
#include "../../common/dsp/constants.hpp"

using namespace SomeDSP;

IntScale<double> Scales::boolScale(1);
LogScale<double> Scales::gain(0.0, 1.0, 0.5, 0.1);
LogScale<double> Scales::cutoff(0.0, 22000.0, 0.5, 100.0);
LinearScale<double> Scales::resonance(0.0, 1.0);
LogScale<double> Scales::dcBlock(0.9, 1.0, 0.5, 0.999);
LogScale<double> Scales::highpass(0.0, 22000, 0.5, 100.0);
