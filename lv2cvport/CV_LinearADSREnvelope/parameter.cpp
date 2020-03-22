// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_LinearADSREnvelope.
//
// CV_LinearADSREnvelope is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_LinearADSREnvelope is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_LinearADSREnvelope.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"
#include "../../common/dsp/constants.hpp"

using namespace SomeDSP;

IntScale<double> Scales::boolScale(1);
LinearScale<double> Scales::defaultScale(0.0, 1.0);

LogScale<double> Scales::attack(0.0001, 16.0, 0.5, 2.0);
LogScale<double> Scales::decay(0.0001, 16.0, 0.5, 4.0);
LogScale<double> Scales::sustain(0.0001, 0.9995, 0.5, 0.3);
LogScale<double> Scales::release(0.001, 16.0, 0.5, 2.0);
