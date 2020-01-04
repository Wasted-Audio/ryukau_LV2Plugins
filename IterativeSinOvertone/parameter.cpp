// (c) 2019 Takamitsu Endo
//
// This file is part of IterativeSinOvertone.
//
// IterativeSinOvertone is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// IterativeSinOvertone is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with IterativeSinOvertone.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"
#include "dsp/constants.hpp"

using namespace SomeDSP;

IntScale<double> Scales::boolScale(1);
LinearScale<double> Scales::defaultScale(0.0, 1.0);

LogScale<double> Scales::envelopeA(0.0, 16.0, 0.5, 0.5);
LogScale<double> Scales::envelopeD(0.0, 4.0, 0.5, 0.5);
DecibelScale<double> Scales::gainDecibel(-40.0, 0.0, true);
LogScale<double> Scales::saturation(0.5, 5.0, 0.5, 2.0);

LogScale<double> Scales::gain(0.0, 32.0, 0.5, 8.0);
LinearScale<double> Scales::gainBoost(1.0, 8.0);

LinearScale<double> Scales::masterOctave(-4.0, 4.0);
LinearScale<double> Scales::equalTemperament(1.0, 60.0);
LinearScale<double> Scales::pitchMultiply(0.001, 4.0);
LinearScale<double> Scales::pitchModulo(0.0, 60.0);

IntScale<double> Scales::seed(16777215); // 2^24 - 1
LinearScale<double> Scales::randomGain(0.0, 4.0);
LogScale<double> Scales::randomFrequency(0.0, 1.0, 0.5, 0.1);
LinearScale<double> Scales::randomAttack(0.0, 1.0);
LinearScale<double> Scales::randomDecay(0.0, 1.0);
LinearScale<double> Scales::randomSaturation(0.0, 1.0);
LinearScale<double> Scales::randomPhase(0.0, 1.0);

LogScale<double> Scales::overtoneExpand(0.01, 4.0, 0.4, 1.0);
LinearScale<double> Scales::overtoneShift(0.0, 64.0);

LogScale<double> Scales::envelopeMultiplier(0.001, 4.0, 0.4, 1.0);
LinearScale<double> Scales::gainPower(0.001, 16.0);

IntScale<double> Scales::nVoice(5);
LogScale<double> Scales::smoothness(0.0, 0.5, 0.1, 0.04);

// Generated from preset dump. This works, but hard coding preset data is seriously bad.
#ifndef TEST_BUILD
void GlobalParameter::loadProgram(uint32_t index)
{
  using ID = ParameterID::ID;

  switch (index) {
    default:
    case presetDefault:
      resetParameter();
      break;
  }
}
#endif
