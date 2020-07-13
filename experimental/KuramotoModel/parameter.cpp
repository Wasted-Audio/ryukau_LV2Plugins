// (c) 2020 Takamitsu Endo
//
// This file is part of KuramotoModel.
//
// KuramotoModel is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KuramotoModel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KuramotoModel.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"
#include "../../common/dsp/constants.hpp"

using namespace SomeDSP;

IntScale<double> Scales::boolScale(1);
LinearScale<double> Scales::defaultScale(0.0, 1.0);

LinearScale<double> Scales::waveform(-1.0, 1.0);

LogScale<double> Scales::gain(0.0, 1.0, 0.5, 0.2);
LogScale<double> Scales::decay(0.0, 8.0, 0.5, 1.0);
LinearScale<double> Scales::pitch(-128.0, 128.0);
LogScale<double> Scales::coupling(0.01, 8.0, 0.25, 0.1);
LogScale<double> Scales::attack(0.002, 0.2, 0.5, 0.1);
LogScale<double> Scales::boost(0.0, 8.0, 0.5, 1.0);

IntScale<double> Scales::octave(16);
IntScale<double> Scales::semitone(168);
IntScale<double> Scales::milli(2000);
IntScale<double> Scales::equalTemperament(119);
IntScale<double> Scales::pitchA4Hz(900);
