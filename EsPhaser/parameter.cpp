// (c) 2019-2020 Takamitsu Endo
//
// This file is part of EsPhaser.
//
// EsPhaser is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// EsPhaser is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with EsPhaser.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"
#include "../common/dsp/constants.hpp"

using namespace SomeDSP;

IntScale<double> Scales::boolScale(1);
LinearScale<double> Scales::defaultScale(0.0, 1.0);

LogScale<double> Scales::frequency(0.0, 16.0, 0.5, 2.0);
LinearScale<double> Scales::feedback(-1.0, 1.0);
LogScale<double> Scales::range(0.0, 128.0, 0.5, 32.0);
LinearScale<double> Scales::phase(0.0, twopi);
LinearScale<double> Scales::cascadeOffset(0.0, twopi);
IntScale<double> Scales::stage(4095);

LogScale<double> Scales::smoothness(0.04, 1.0, 0.5, 0.4);

// Generated from preset dump. This works, but hard coding preset data is seriously bad.
#ifndef TEST_BUILD
void GlobalParameter::loadProgram(uint32_t index)
{
  using ID = ParameterID::ID;

  switch (index) {
    default:
      resetParameter();
      break;

    case presetDefault: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.5);
      value[ID::frequency]->setFromNormalized(0.5);
      value[ID::freqSpread]->setFromNormalized(0.0);
      value[ID::feedback]->setFromNormalized(0.5);
      value[ID::range]->setFromNormalized(1.0);
      value[ID::min]->setFromNormalized(0.0);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.5);
      value[ID::cascadeOffset]->setFromNormalized(0.0625);
      value[ID::stage]->setFromInt(15);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetAutomateCasOffset: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.492);
      value[ID::frequency]->setFromNormalized(0.148);
      value[ID::freqSpread]->setFromNormalized(0.208);
      value[ID::feedback]->setFromNormalized(0.964);
      value[ID::range]->setFromNormalized(1.0);
      value[ID::min]->setFromNormalized(1.0);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.5);
      value[ID::cascadeOffset]->setFromNormalized(0.0);
      value[ID::stage]->setFromInt(4095);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetAutomateMin: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.584);
      value[ID::frequency]->setFromNormalized(0.76);
      value[ID::freqSpread]->setFromNormalized(0.0);
      value[ID::feedback]->setFromNormalized(0.958696);
      value[ID::range]->setFromNormalized(0.048);
      value[ID::min]->setFromNormalized(0.0);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.34);
      value[ID::cascadeOffset]->setFromNormalized(0.0);
      value[ID::stage]->setFromInt(4095);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetFeedback: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.9320003390312195);
      value[ID::frequency]->setFromNormalized(1.0);
      value[ID::freqSpread]->setFromNormalized(0.0);
      value[ID::feedback]->setFromNormalized(0.0);
      value[ID::range]->setFromNormalized(0.8960002064704895);
      value[ID::min]->setFromNormalized(0.9479999542236328);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.5600001215934753);
      value[ID::cascadeOffset]->setFromNormalized(0.0);
      value[ID::stage]->setFromInt(2260);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetHallucinogen95: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.584);
      value[ID::frequency]->setFromNormalized(0.412);
      value[ID::freqSpread]->setFromNormalized(0.552);
      value[ID::feedback]->setFromNormalized(0.906696);
      value[ID::range]->setFromNormalized(0.364);
      value[ID::min]->setFromNormalized(0.0);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.34);
      value[ID::cascadeOffset]->setFromNormalized(0.264);
      value[ID::stage]->setFromInt(114);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetResonator: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.32);
      value[ID::frequency]->setFromNormalized(0.14);
      value[ID::freqSpread]->setFromNormalized(0.0);
      value[ID::feedback]->setFromNormalized(0.008);
      value[ID::range]->setFromNormalized(0.048);
      value[ID::min]->setFromNormalized(0.368);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.34);
      value[ID::cascadeOffset]->setFromNormalized(0.0);
      value[ID::stage]->setFromInt(57);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetSharp: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.8360000848770142);
      value[ID::frequency]->setFromNormalized(0.056000053882598884);
      value[ID::freqSpread]->setFromNormalized(0.7280000448226929);
      value[ID::feedback]->setFromNormalized(0.08800039440393448);
      value[ID::range]->setFromNormalized(0.33999985456466675);
      value[ID::min]->setFromNormalized(0.6800001263618469);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.5);
      value[ID::cascadeOffset]->setFromNormalized(0.4679999947547912);
      value[ID::stage]->setFromInt(24);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetSlowBend: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.3880005478858948);
      value[ID::frequency]->setFromNormalized(0.14400027692317963);
      value[ID::freqSpread]->setFromNormalized(0.432000070810318);
      value[ID::feedback]->setFromNormalized(0.0320000946521759);
      value[ID::range]->setFromNormalized(0.8960002064704895);
      value[ID::min]->setFromNormalized(0.28800007700920105);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.3160001337528229);
      value[ID::cascadeOffset]->setFromNormalized(0.5999999642372131);
      value[ID::stage]->setFromInt(3514);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetStage4096: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.5);
      value[ID::frequency]->setFromNormalized(0.043994);
      value[ID::freqSpread]->setFromNormalized(0.0);
      value[ID::feedback]->setFromNormalized(0.952);
      value[ID::range]->setFromNormalized(0.368);
      value[ID::min]->setFromNormalized(0.932);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.5);
      value[ID::cascadeOffset]->setFromNormalized(0.628);
      value[ID::stage]->setFromInt(4095);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetSubtle: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.3);
      value[ID::frequency]->setFromNormalized(0.412);
      value[ID::freqSpread]->setFromNormalized(0.968);
      value[ID::feedback]->setFromNormalized(0.402695);
      value[ID::range]->setFromNormalized(0.972);
      value[ID::min]->setFromNormalized(0.7);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.0);
      value[ID::cascadeOffset]->setFromNormalized(0.264);
      value[ID::stage]->setFromInt(753);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetThick: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.836000382900238);
      value[ID::frequency]->setFromNormalized(0.7920001149177551);
      value[ID::freqSpread]->setFromNormalized(0.8640003204345703);
      value[ID::feedback]->setFromNormalized(0.47199997305870056);
      value[ID::range]->setFromNormalized(0.7519999146461487);
      value[ID::min]->setFromNormalized(0.31999996304512024);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.0);
      value[ID::cascadeOffset]->setFromNormalized(0.8719998598098754);
      value[ID::stage]->setFromInt(827);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetThisPhaserIsTooResourceHungry: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.5);
      value[ID::frequency]->setFromNormalized(0.42);
      value[ID::freqSpread]->setFromNormalized(0.444);
      value[ID::feedback]->setFromNormalized(0.972);
      value[ID::range]->setFromNormalized(0.476);
      value[ID::min]->setFromNormalized(0.0);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.5);
      value[ID::cascadeOffset]->setFromNormalized(0.0);
      value[ID::stage]->setFromInt(4095);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;
  }
}
#endif
