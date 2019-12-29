// (c) 2019 Takamitsu Endo
//
// This file is part of WaveCymbal.
//
// WaveCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// WaveCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with WaveCymbal.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"
#include "dsp/constants.hpp"

using namespace SomeDSP;

IntScale<double> Scales::boolScale(1);
LinearScale<double> Scales::defaultScale(0.0, 1.0);

IntScale<double> Scales::seed(16777215); // 2^24 - 1
LogScale<double> Scales::randomAmount(0.0, 1.0, 0.5, 0.1);
LinearScale<double> Scales::nCymbal(0.0, 3.0);
LogScale<double> Scales::decay(0.0, 16.0, 0.5, 4.0);
LogScale<double> Scales::damping(0.0, 0.999, 0.5, 0.9);
LogScale<double> Scales::minFrequency(0.0, 1000.0, 0.5, 100.0);
LogScale<double> Scales::maxFrequency(10.0, 4000.0, 0.5, 400.0);
LinearScale<double> Scales::bandpassQ(0.0001, 0.9999);
LogScale<double> Scales::distance(0.0, 8.0, 0.5, 0.1);
IntScale<double> Scales::stack(63);
LogScale<double> Scales::pickCombFeedback(0.0, 0.9999, 0.5, 0.7);
LogScale<double> Scales::pickCombTime(0.005, 0.4, 0.5, 0.1);
IntScale<double> Scales::oscType(4);
LogScale<double> Scales::smoothness(0.0, 0.1, 0.2, 0.02);

LogScale<double> Scales::gain(0.0, 4.0, 0.75, 1.0);

// Generated from preset dump. This works, but hard coding preset data is seriously bad.
void GlobalParameter::loadProgram(uint32_t index)
{
  using ID = ParameterID::ID;

  switch (index) {
    default:
      resetParameter();
      break;

    case presetDefault: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(6583421);
      value[ID::randomAmount]->setFromNormalized(1.0);
      value[ID::nCymbal]->setFromInt(3);
      value[ID::stack]->setFromInt(24);
      value[ID::decay]->setFromNormalized(0.5);
      value[ID::distance]->setFromNormalized(0.5);
      value[ID::minFrequency]->setFromNormalized(0.5);
      value[ID::maxFrequency]->setFromNormalized(0.5);
      value[ID::bandpassQ]->setFromNormalized(0.5);
      value[ID::damping]->setFromNormalized(0.5);
      value[ID::pulsePosition]->setFromNormalized(0.5);
      value[ID::pulseWidth]->setFromNormalized(0.5);
      value[ID::pickCombFeedback]->setFromNormalized(0.5);
      value[ID::pickCombTime]->setFromNormalized(0.25);
      value[ID::retrigger]->setFromInt(0);
      value[ID::cutoffMap]->setFromInt(0);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(2);
      value[ID::smoothness]->setFromNormalized(0.6999999999999998);
      value[ID::gain]->setFromNormalized(0.4);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetDontTouchFeedback: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(11012605);
      value[ID::randomAmount]->setFromNormalized(0.45600003004074097);
      value[ID::nCymbal]->setFromInt(2);
      value[ID::stack]->setFromInt(4);
      value[ID::decay]->setFromNormalized(0.11200007796287537);
      value[ID::distance]->setFromNormalized(0.09599999338388443);
      value[ID::minFrequency]->setFromNormalized(0.3560000956058502);
      value[ID::maxFrequency]->setFromNormalized(0.2920001447200775);
      value[ID::bandpassQ]->setFromNormalized(0.8680000305175781);
      value[ID::damping]->setFromNormalized(0.9360007047653202);
      value[ID::pulsePosition]->setFromNormalized(0.3879999816417694);
      value[ID::pulseWidth]->setFromNormalized(0.5);
      value[ID::pickCombFeedback]->setFromNormalized(0.3599994480609894);
      value[ID::pickCombTime]->setFromNormalized(0.04199992865324022);
      value[ID::retrigger]->setFromInt(1);
      value[ID::cutoffMap]->setFromInt(1);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(3);
      value[ID::smoothness]->setFromNormalized(0.6999999999999998);
      value[ID::gain]->setFromNormalized(0.09827584028244019);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetIsThis: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(3764849);
      value[ID::randomAmount]->setFromNormalized(1.0);
      value[ID::nCymbal]->setFromInt(3);
      value[ID::stack]->setFromInt(45);
      value[ID::decay]->setFromNormalized(0.5);
      value[ID::distance]->setFromNormalized(0.2200000286102295);
      value[ID::minFrequency]->setFromNormalized(0.5);
      value[ID::maxFrequency]->setFromNormalized(0.5);
      value[ID::bandpassQ]->setFromNormalized(0.5);
      value[ID::damping]->setFromNormalized(0.3560000061988831);
      value[ID::pulsePosition]->setFromNormalized(0.5);
      value[ID::pulseWidth]->setFromNormalized(0.5);
      value[ID::pickCombFeedback]->setFromNormalized(0.5);
      value[ID::pickCombTime]->setFromNormalized(0.11399995535612106);
      value[ID::retrigger]->setFromInt(1);
      value[ID::cutoffMap]->setFromInt(0);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(1);
      value[ID::smoothness]->setFromNormalized(0.6999999999999998);
      value[ID::gain]->setFromNormalized(0.636206865310669);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetItsHappning: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(11750803);
      value[ID::randomAmount]->setFromNormalized(1.0);
      value[ID::nCymbal]->setFromInt(0);
      value[ID::stack]->setFromInt(54);
      value[ID::decay]->setFromNormalized(0.2680000066757202);
      value[ID::distance]->setFromNormalized(0.23200000822544098);
      value[ID::minFrequency]->setFromNormalized(0.2240000814199448);
      value[ID::maxFrequency]->setFromNormalized(0.25999999046325684);
      value[ID::bandpassQ]->setFromNormalized(0.21600009500980377);
      value[ID::damping]->setFromNormalized(0.18400073051452637);
      value[ID::pulsePosition]->setFromNormalized(0.18800009787082672);
      value[ID::pulseWidth]->setFromNormalized(0.9320000410079956);
      value[ID::pickCombFeedback]->setFromNormalized(0.928000271320343);
      value[ID::pickCombTime]->setFromNormalized(0.04200000315904618);
      value[ID::retrigger]->setFromInt(0);
      value[ID::cutoffMap]->setFromInt(0);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(2);
      value[ID::smoothness]->setFromNormalized(0.6999999999999998);
      value[ID::gain]->setFromNormalized(0.19137930870056152);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetNotEvenClose: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(9737537);
      value[ID::randomAmount]->setFromNormalized(1.0);
      value[ID::nCymbal]->setFromInt(2);
      value[ID::stack]->setFromInt(41);
      value[ID::decay]->setFromNormalized(0.5);
      value[ID::distance]->setFromNormalized(0.18400011956691742);
      value[ID::minFrequency]->setFromNormalized(0.5);
      value[ID::maxFrequency]->setFromNormalized(0.5);
      value[ID::bandpassQ]->setFromNormalized(0.5);
      value[ID::damping]->setFromNormalized(0.9599999189376829);
      value[ID::pulsePosition]->setFromNormalized(0.1759999543428421);
      value[ID::pulseWidth]->setFromNormalized(0.1679999828338623);
      value[ID::pickCombFeedback]->setFromNormalized(0.5);
      value[ID::pickCombTime]->setFromNormalized(0.25);
      value[ID::retrigger]->setFromInt(1);
      value[ID::cutoffMap]->setFromInt(0);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(1);
      value[ID::smoothness]->setFromNormalized(0.6999999999999998);
      value[ID::gain]->setFromNormalized(0.28448277711868286);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetNothingLikeCymbal: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(13898286);
      value[ID::randomAmount]->setFromNormalized(0.5080000162124634);
      value[ID::nCymbal]->setFromInt(1);
      value[ID::stack]->setFromInt(47);
      value[ID::decay]->setFromNormalized(0.7080003619194031);
      value[ID::distance]->setFromNormalized(0.2160000503063202);
      value[ID::minFrequency]->setFromNormalized(0.04800000786781312);
      value[ID::maxFrequency]->setFromNormalized(0.28399986028671265);
      value[ID::bandpassQ]->setFromNormalized(0.8159999251365662);
      value[ID::damping]->setFromNormalized(0.952000379562378);
      value[ID::pulsePosition]->setFromNormalized(0.8200005292892456);
      value[ID::pulseWidth]->setFromNormalized(0.04000024497509003);
      value[ID::pickCombFeedback]->setFromNormalized(0.20399993658065793);
      value[ID::pickCombTime]->setFromNormalized(0.27400022745132446);
      value[ID::retrigger]->setFromInt(1);
      value[ID::cutoffMap]->setFromInt(0);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(1);
      value[ID::smoothness]->setFromNormalized(0.6999999999999998);
      value[ID::gain]->setFromNormalized(0.4534482955932617);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetReverbDevsNightmare: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(9737540);
      value[ID::randomAmount]->setFromNormalized(1.0);
      value[ID::nCymbal]->setFromInt(0);
      value[ID::stack]->setFromInt(63);
      value[ID::decay]->setFromNormalized(0.8400002717971802);
      value[ID::distance]->setFromNormalized(0.5);
      value[ID::minFrequency]->setFromNormalized(0.5);
      value[ID::maxFrequency]->setFromNormalized(0.5);
      value[ID::bandpassQ]->setFromNormalized(1.0);
      value[ID::damping]->setFromNormalized(1.0);
      value[ID::pulsePosition]->setFromNormalized(1.0);
      value[ID::pulseWidth]->setFromNormalized(0.5720000863075256);
      value[ID::pickCombFeedback]->setFromNormalized(0.2240000218153);
      value[ID::pickCombTime]->setFromNormalized(0.4659999907016754);
      value[ID::retrigger]->setFromInt(1);
      value[ID::cutoffMap]->setFromInt(0);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(2);
      value[ID::smoothness]->setFromNormalized(0.6999999999999998);
      value[ID::gain]->setFromNormalized(0.4);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetRubbingTheSurface: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(9804646);
      value[ID::randomAmount]->setFromNormalized(1.0);
      value[ID::nCymbal]->setFromInt(2);
      value[ID::stack]->setFromInt(24);
      value[ID::decay]->setFromNormalized(0.5800000429153442);
      value[ID::distance]->setFromNormalized(0.13600009679794312);
      value[ID::minFrequency]->setFromNormalized(0.6159999966621399);
      value[ID::maxFrequency]->setFromNormalized(0.21599999070167544);
      value[ID::bandpassQ]->setFromNormalized(0.1080000102519989);
      value[ID::damping]->setFromNormalized(0.07200013101100922);
      value[ID::pulsePosition]->setFromNormalized(0.7199999690055847);
      value[ID::pulseWidth]->setFromNormalized(0.5720000267028809);
      value[ID::pickCombFeedback]->setFromNormalized(0.4000000953674317);
      value[ID::pickCombTime]->setFromNormalized(0.25);
      value[ID::retrigger]->setFromInt(1);
      value[ID::cutoffMap]->setFromInt(0);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(2);
      value[ID::smoothness]->setFromNormalized(0.6999999999999998);
      value[ID::gain]->setFromNormalized(0.7120689749717712);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetSoFuturistic: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(15039139);
      value[ID::randomAmount]->setFromNormalized(1.0);
      value[ID::nCymbal]->setFromInt(1);
      value[ID::stack]->setFromInt(29);
      value[ID::decay]->setFromNormalized(0.12800003588199615);
      value[ID::distance]->setFromNormalized(0.0);
      value[ID::minFrequency]->setFromNormalized(0.0);
      value[ID::maxFrequency]->setFromNormalized(0.0);
      value[ID::bandpassQ]->setFromNormalized(0.796000063419342);
      value[ID::damping]->setFromNormalized(0.7759999632835387);
      value[ID::pulsePosition]->setFromNormalized(0.5080001354217529);
      value[ID::pulseWidth]->setFromNormalized(0.9319999814033508);
      value[ID::pickCombFeedback]->setFromNormalized(0.2439998388290405);
      value[ID::pickCombTime]->setFromNormalized(0.2460002452135086);
      value[ID::retrigger]->setFromInt(0);
      value[ID::cutoffMap]->setFromInt(1);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(3);
      value[ID::smoothness]->setFromNormalized(0.6999999999999998);
      value[ID::gain]->setFromNormalized(0.42241376638412476);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetSomeMachineNoise: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(10542846);
      value[ID::randomAmount]->setFromNormalized(1.0);
      value[ID::nCymbal]->setFromInt(3);
      value[ID::stack]->setFromInt(24);
      value[ID::decay]->setFromNormalized(0.5);
      value[ID::distance]->setFromNormalized(0.2200000137090683);
      value[ID::minFrequency]->setFromNormalized(0.22000004351139069);
      value[ID::maxFrequency]->setFromNormalized(0.05600001662969587);
      value[ID::bandpassQ]->setFromNormalized(0.5);
      value[ID::damping]->setFromNormalized(0.5);
      value[ID::pulsePosition]->setFromNormalized(0.50799959897995);
      value[ID::pulseWidth]->setFromNormalized(0.5);
      value[ID::pickCombFeedback]->setFromNormalized(0.6800000667572021);
      value[ID::pickCombTime]->setFromNormalized(0.1340000331401825);
      value[ID::retrigger]->setFromInt(1);
      value[ID::cutoffMap]->setFromInt(0);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(2);
      value[ID::smoothness]->setFromNormalized(0.6999999999999998);
      value[ID::gain]->setFromNormalized(0.34310346841812134);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetThatWavyMetalPlate: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(10945497);
      value[ID::randomAmount]->setFromNormalized(1.0);
      value[ID::nCymbal]->setFromInt(3);
      value[ID::stack]->setFromInt(42);
      value[ID::decay]->setFromNormalized(0.8679999709129333);
      value[ID::distance]->setFromNormalized(0.5);
      value[ID::minFrequency]->setFromNormalized(0.2680000066757202);
      value[ID::maxFrequency]->setFromNormalized(0.5);
      value[ID::bandpassQ]->setFromNormalized(0.9560002088546753);
      value[ID::damping]->setFromNormalized(0.7080000042915346);
      value[ID::pulsePosition]->setFromNormalized(0.2120000123977661);
      value[ID::pulseWidth]->setFromNormalized(0.07999999821186066);
      value[ID::pickCombFeedback]->setFromNormalized(0.3640001714229583);
      value[ID::pickCombTime]->setFromNormalized(0.722000002861023);
      value[ID::retrigger]->setFromInt(1);
      value[ID::cutoffMap]->setFromInt(0);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(4);
      value[ID::smoothness]->setFromNormalized(0.25999999046325684);
      value[ID::gain]->setFromNormalized(0.26724135875701904);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetWhat: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(4234610);
      value[ID::randomAmount]->setFromNormalized(1.0);
      value[ID::nCymbal]->setFromInt(3);
      value[ID::stack]->setFromInt(34);
      value[ID::decay]->setFromNormalized(0.3639998435974121);
      value[ID::distance]->setFromNormalized(0.18400011956691742);
      value[ID::minFrequency]->setFromNormalized(0.6640000343322754);
      value[ID::maxFrequency]->setFromNormalized(0.8520000576972961);
      value[ID::bandpassQ]->setFromNormalized(0.22000020742416382);
      value[ID::damping]->setFromNormalized(0.7119997739791869);
      value[ID::pulsePosition]->setFromNormalized(0.49599990248680115);
      value[ID::pulseWidth]->setFromNormalized(0.5679998993873596);
      value[ID::pickCombFeedback]->setFromNormalized(0.6600000262260437);
      value[ID::pickCombTime]->setFromNormalized(0.7899999618530273);
      value[ID::retrigger]->setFromInt(1);
      value[ID::cutoffMap]->setFromInt(0);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(1);
      value[ID::smoothness]->setFromNormalized(0.6999999999999998);
      value[ID::gain]->setFromNormalized(0.36724138259887695);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetWhy: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(14763946);
      value[ID::randomAmount]->setFromNormalized(1.0);
      value[ID::nCymbal]->setFromInt(2);
      value[ID::stack]->setFromInt(42);
      value[ID::decay]->setFromNormalized(0.17600001394748688);
      value[ID::distance]->setFromNormalized(0.25599998235702515);
      value[ID::minFrequency]->setFromNormalized(0.5);
      value[ID::maxFrequency]->setFromNormalized(0.6120002269744873);
      value[ID::bandpassQ]->setFromNormalized(0.5);
      value[ID::damping]->setFromNormalized(0.7399998903274537);
      value[ID::pulsePosition]->setFromNormalized(0.47999998927116394);
      value[ID::pulseWidth]->setFromNormalized(0.5);
      value[ID::pickCombFeedback]->setFromNormalized(0.40799999237060547);
      value[ID::pickCombTime]->setFromNormalized(0.025999985635280626);
      value[ID::retrigger]->setFromInt(1);
      value[ID::cutoffMap]->setFromInt(0);
      value[ID::excitation]->setFromInt(1);
      value[ID::collision]->setFromInt(1);
      value[ID::oscType]->setFromInt(4);
      value[ID::smoothness]->setFromNormalized(0.6999999999999998);
      value[ID::gain]->setFromNormalized(0.4534482955932617);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;
  }
}
