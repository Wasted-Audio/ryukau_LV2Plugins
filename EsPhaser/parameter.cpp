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
#include "dsp/constants.hpp"

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
      value[ID::smoothness]->setFromNormalized(0.44985925258897935);
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
      value[ID::smoothness]->setFromNormalized(0.44985925258897935);
    } break;

    case presetStage1: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.3200003504753113);
      value[ID::frequency]->setFromNormalized(0.14800019562244415);
      value[ID::freqSpread]->setFromNormalized(0.7120001316070557);
      value[ID::feedback]->setFromNormalized(0.872000515460968);
      value[ID::range]->setFromNormalized(0.8599997162818909);
      value[ID::min]->setFromNormalized(0.1359998732805252);
      value[ID::phase]->setFromNormalized(0.3520001471042633);
      value[ID::stereoOffset]->setFromNormalized(0.0);
      value[ID::cascadeOffset]->setFromNormalized(1.0);
      value[ID::stage]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetStage1770: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.2560003995895386);
      value[ID::frequency]->setFromNormalized(0.7640001773834229);
      value[ID::freqSpread]->setFromNormalized(0.160000279545784);
      value[ID::feedback]->setFromNormalized(0.47199997305870056);
      value[ID::range]->setFromNormalized(0.7320000529289246);
      value[ID::min]->setFromNormalized(0.13600000739097595);
      value[ID::phase]->setFromNormalized(0.3520001471042633);
      value[ID::stereoOffset]->setFromNormalized(0.5600001215934753);
      value[ID::cascadeOffset]->setFromNormalized(0.41199982166290283);
      value[ID::stage]->setFromInt(1769);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetStage2261: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.9320003390312195);
      value[ID::frequency]->setFromNormalized(1.0);
      value[ID::freqSpread]->setFromNormalized(0.0);
      value[ID::feedback]->setFromNormalized(0.0);
      value[ID::range]->setFromNormalized(0.8960002064704895);
      value[ID::min]->setFromNormalized(0.9479999542236328);
      value[ID::phase]->setFromNormalized(0.3520001471042633);
      value[ID::stereoOffset]->setFromNormalized(0.5600001215934753);
      value[ID::cascadeOffset]->setFromNormalized(0.0);
      value[ID::stage]->setFromInt(2260);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetStage2982: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.23600055277347565);
      value[ID::frequency]->setFromNormalized(0.7320000529289246);
      value[ID::freqSpread]->setFromNormalized(0.5519999861717224);
      value[ID::feedback]->setFromNormalized(0.5);
      value[ID::range]->setFromNormalized(0.8960002064704895);
      value[ID::min]->setFromNormalized(0.3520001471042633);
      value[ID::phase]->setFromNormalized(0.3520001471042633);
      value[ID::stereoOffset]->setFromNormalized(0.5600001215934753);
      value[ID::cascadeOffset]->setFromNormalized(0.5999999642372131);
      value[ID::stage]->setFromInt(2981);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetStage3515: {
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
      value[ID::frequency]->setFromNormalized(0.0320000946521759);
      value[ID::freqSpread]->setFromNormalized(0.8480003476142883);
      value[ID::feedback]->setFromNormalized(0.9520004987716675);
      value[ID::range]->setFromNormalized(0.0720001757144928);
      value[ID::min]->setFromNormalized(0.8640000820159912);
      value[ID::phase]->setFromNormalized(0.0);
      value[ID::stereoOffset]->setFromNormalized(0.5);
      value[ID::cascadeOffset]->setFromNormalized(0.6745002865791321);
      value[ID::stage]->setFromInt(4095);
      value[ID::smoothness]->setFromNormalized(0.4498592615127564);
    } break;

    case presetStage476: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.7680003643035889);
      value[ID::frequency]->setFromNormalized(0.504000186920166);
      value[ID::freqSpread]->setFromNormalized(0.9680002331733704);
      value[ID::feedback]->setFromNormalized(0.9160006046295166);
      value[ID::range]->setFromNormalized(0.9719998240470886);
      value[ID::min]->setFromNormalized(0.6999999284744263);
      value[ID::phase]->setFromNormalized(0.3520001471042633);
      value[ID::stereoOffset]->setFromNormalized(0.0);
      value[ID::cascadeOffset]->setFromNormalized(0.2639998495578766);
      value[ID::stage]->setFromInt(475);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;

    case presetStage828: {
      value[ID::bypass]->setFromInt(0);
      value[ID::mix]->setFromNormalized(0.836000382900238);
      value[ID::frequency]->setFromNormalized(0.7920001149177551);
      value[ID::freqSpread]->setFromNormalized(0.8640003204345703);
      value[ID::feedback]->setFromNormalized(0.47199997305870056);
      value[ID::range]->setFromNormalized(0.7519999146461487);
      value[ID::min]->setFromNormalized(0.31999996304512024);
      value[ID::phase]->setFromNormalized(0.3520001471042633);
      value[ID::stereoOffset]->setFromNormalized(0.0);
      value[ID::cascadeOffset]->setFromNormalized(0.8719998598098754);
      value[ID::stage]->setFromInt(827);
      value[ID::smoothness]->setFromNormalized(0.0);
    } break;
  }
}
#endif
