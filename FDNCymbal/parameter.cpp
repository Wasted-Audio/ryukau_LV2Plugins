// (c) 2019-2020 Takamitsu Endo
//
// This file is part of FDNCymbal.
//
// FDNCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// FDNCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with FDNCymbal.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"
#include "dsp/constants.hpp"

using namespace SomeDSP;

IntScale<double> Scales::boolScale(1);
LinearScale<double> Scales::defaultScale(0.0, 1.0);

IntScale<double> Scales::seed(16777215); // 2^24 - 1
LogScale<double> Scales::fdnTime(0.0001, 0.5, 0.5, 0.1);
LogScale<double> Scales::fdnFeedback(0.0, 4.0, 0.75, 1.0);
LogScale<double> Scales::fdnCascadeMix(0.0, 1.0, 0.5, 0.2);
LogScale<double> Scales::allpassTime(0.0, 0.005, 0.5, 0.001);
LogScale<double> Scales::allpassFeedback(0.0, 0.9999, 0.5, 0.9);
LogScale<double> Scales::allpassHighpassCutoff(1.0, 40.0, 0.5, 10.0);
LogScale<double> Scales::tremoloFrequency(0.1, 20, 0.5, 4.0);
LogScale<double> Scales::tremoloDelayTime(0.00003, 0.001, 0.5, 0.0001);
LogScale<double> Scales::stickDecay(0.01, 4.0, 0.5, 0.1);
LogScale<double> Scales::stickToneMix(0.0, 0.02, 0.5, 0.001);

LogScale<double> Scales::smoothness(0.0, 0.5, 0.2, 0.02);

LogScale<double> Scales::gain(0.0, 4.0, 0.75, 0.5);

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
      value[ID::retriggerTime]->setFromInt(1);
      value[ID::retriggerStick]->setFromInt(0);
      value[ID::retriggerTremolo]->setFromInt(0);
      value[ID::fdn]->setFromInt(1);
      value[ID::fdnTime]->setFromNormalized(0.2);
      value[ID::fdnFeedback]->setFromNormalized(0.5);
      value[ID::fdnCascadeMix]->setFromNormalized(0.5);
      value[ID::allpassMix]->setFromNormalized(0.75);
      value[ID::allpass1Saturation]->setFromInt(1);
      value[ID::allpass1Time]->setFromNormalized(0.5);
      value[ID::allpass1Feedback]->setFromNormalized(0.75);
      value[ID::allpass1HighpassCutoff]->setFromNormalized(0.5);
      value[ID::allpass2Time]->setFromNormalized(0.5);
      value[ID::allpass2Feedback]->setFromNormalized(0.5);
      value[ID::allpass2HighpassCutoff]->setFromNormalized(0.5);
      value[ID::tremoloMix]->setFromNormalized(0.2);
      value[ID::tremoloDepth]->setFromNormalized(0.8);
      value[ID::tremoloFrequency]->setFromNormalized(0.5);
      value[ID::tremoloDelayTime]->setFromNormalized(0.25000000000000006);
      value[ID::randomTremoloDepth]->setFromNormalized(0.35);
      value[ID::randomTremoloFrequency]->setFromNormalized(0.35);
      value[ID::randomTremoloDelayTime]->setFromNormalized(0.35);
      value[ID::stick]->setFromInt(1);
      value[ID::stickDecay]->setFromNormalized(0.5);
      value[ID::stickToneMix]->setFromNormalized(0.5);
      value[ID::smoothness]->setFromNormalized(0.1);
      value[ID::gain]->setFromNormalized(0.5);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case preset0: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(0);
      value[ID::retriggerTime]->setFromInt(1);
      value[ID::retriggerStick]->setFromInt(0);
      value[ID::retriggerTremolo]->setFromInt(0);
      value[ID::fdn]->setFromInt(1);
      value[ID::fdnTime]->setFromNormalized(0.4959999918937683);
      value[ID::fdnFeedback]->setFromNormalized(0.3240000009536743);
      value[ID::fdnCascadeMix]->setFromNormalized(0.5);
      value[ID::allpassMix]->setFromNormalized(0.75);
      value[ID::allpass1Saturation]->setFromInt(1);
      value[ID::allpass1Time]->setFromNormalized(0.800000011920929);
      value[ID::allpass1Feedback]->setFromNormalized(0.75);
      value[ID::allpass1HighpassCutoff]->setFromNormalized(0.5);
      value[ID::allpass2Time]->setFromNormalized(0.5);
      value[ID::allpass2Feedback]->setFromNormalized(0.5);
      value[ID::allpass2HighpassCutoff]->setFromNormalized(0.5);
      value[ID::tremoloMix]->setFromNormalized(0.2);
      value[ID::tremoloDepth]->setFromNormalized(0.8);
      value[ID::tremoloFrequency]->setFromNormalized(0.5);
      value[ID::tremoloDelayTime]->setFromNormalized(0.25000000000000006);
      value[ID::randomTremoloDepth]->setFromNormalized(0.35);
      value[ID::randomTremoloFrequency]->setFromNormalized(0.35);
      value[ID::randomTremoloDelayTime]->setFromNormalized(0.35);
      value[ID::stick]->setFromInt(1);
      value[ID::stickDecay]->setFromNormalized(0.7359999418258667);
      value[ID::stickToneMix]->setFromNormalized(0.22800002992153168);
      value[ID::smoothness]->setFromNormalized(0.1);
      value[ID::gain]->setFromNormalized(0.5);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case preset11415258: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(11415258);
      value[ID::retriggerTime]->setFromInt(1);
      value[ID::retriggerStick]->setFromInt(0);
      value[ID::retriggerTremolo]->setFromInt(0);
      value[ID::fdn]->setFromInt(1);
      value[ID::fdnTime]->setFromNormalized(0.2);
      value[ID::fdnFeedback]->setFromNormalized(0.5);
      value[ID::fdnCascadeMix]->setFromNormalized(0.5);
      value[ID::allpassMix]->setFromNormalized(0.75);
      value[ID::allpass1Saturation]->setFromInt(1);
      value[ID::allpass1Time]->setFromNormalized(0.5);
      value[ID::allpass1Feedback]->setFromNormalized(0.75);
      value[ID::allpass1HighpassCutoff]->setFromNormalized(0.5);
      value[ID::allpass2Time]->setFromNormalized(0.30000004172325134);
      value[ID::allpass2Feedback]->setFromNormalized(0.8920000195503238);
      value[ID::allpass2HighpassCutoff]->setFromNormalized(0.5);
      value[ID::tremoloMix]->setFromNormalized(0.2);
      value[ID::tremoloDepth]->setFromNormalized(0.8);
      value[ID::tremoloFrequency]->setFromNormalized(0.5);
      value[ID::tremoloDelayTime]->setFromNormalized(0.25000000000000006);
      value[ID::randomTremoloDepth]->setFromNormalized(0.35);
      value[ID::randomTremoloFrequency]->setFromNormalized(0.35);
      value[ID::randomTremoloDelayTime]->setFromNormalized(0.35);
      value[ID::stick]->setFromInt(1);
      value[ID::stickDecay]->setFromNormalized(0.0);
      value[ID::stickToneMix]->setFromNormalized(1.0);
      value[ID::smoothness]->setFromNormalized(0.1);
      value[ID::gain]->setFromNormalized(0.5);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case preset5711006: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(5711006);
      value[ID::retriggerTime]->setFromInt(1);
      value[ID::retriggerStick]->setFromInt(0);
      value[ID::retriggerTremolo]->setFromInt(0);
      value[ID::fdn]->setFromInt(1);
      value[ID::fdnTime]->setFromNormalized(0.12799999117851257);
      value[ID::fdnFeedback]->setFromNormalized(0.5360000729560852);
      value[ID::fdnCascadeMix]->setFromNormalized(0.09199995547533037);
      value[ID::allpassMix]->setFromNormalized(0.75);
      value[ID::allpass1Saturation]->setFromInt(1);
      value[ID::allpass1Time]->setFromNormalized(0.6880000233650208);
      value[ID::allpass1Feedback]->setFromNormalized(0.75);
      value[ID::allpass1HighpassCutoff]->setFromNormalized(0.5);
      value[ID::allpass2Time]->setFromNormalized(0.5);
      value[ID::allpass2Feedback]->setFromNormalized(0.5);
      value[ID::allpass2HighpassCutoff]->setFromNormalized(0.5);
      value[ID::tremoloMix]->setFromNormalized(0.2);
      value[ID::tremoloDepth]->setFromNormalized(0.8);
      value[ID::tremoloFrequency]->setFromNormalized(0.5);
      value[ID::tremoloDelayTime]->setFromNormalized(0.25000000000000006);
      value[ID::randomTremoloDepth]->setFromNormalized(0.35);
      value[ID::randomTremoloFrequency]->setFromNormalized(0.35);
      value[ID::randomTremoloDelayTime]->setFromNormalized(0.35);
      value[ID::stick]->setFromInt(1);
      value[ID::stickDecay]->setFromNormalized(0.5);
      value[ID::stickToneMix]->setFromNormalized(0.5);
      value[ID::smoothness]->setFromNormalized(0.1);
      value[ID::gain]->setFromNormalized(0.5);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetFDNTweak: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(3227983);
      value[ID::retriggerTime]->setFromInt(1);
      value[ID::retriggerStick]->setFromInt(0);
      value[ID::retriggerTremolo]->setFromInt(0);
      value[ID::fdn]->setFromInt(1);
      value[ID::fdnTime]->setFromNormalized(0.5839999914169312);
      value[ID::fdnFeedback]->setFromNormalized(0.6200000047683716);
      value[ID::fdnCascadeMix]->setFromNormalized(0.26800012588500977);
      value[ID::allpassMix]->setFromNormalized(0.8180000185966492);
      value[ID::allpass1Saturation]->setFromInt(1);
      value[ID::allpass1Time]->setFromNormalized(0.4680000543594361);
      value[ID::allpass1Feedback]->setFromNormalized(0.598000168800354);
      value[ID::allpass1HighpassCutoff]->setFromNormalized(0.5560001134872437);
      value[ID::allpass2Time]->setFromNormalized(0.699999988079071);
      value[ID::allpass2Feedback]->setFromNormalized(0.3320000469684602);
      value[ID::allpass2HighpassCutoff]->setFromNormalized(0.7079998254776001);
      value[ID::tremoloMix]->setFromNormalized(0.3120000958442688);
      value[ID::tremoloDepth]->setFromNormalized(0.8);
      value[ID::tremoloFrequency]->setFromNormalized(0.5);
      value[ID::tremoloDelayTime]->setFromNormalized(0.25000000000000006);
      value[ID::randomTremoloDepth]->setFromNormalized(0.35);
      value[ID::randomTremoloFrequency]->setFromNormalized(0.35);
      value[ID::randomTremoloDelayTime]->setFromNormalized(0.35);
      value[ID::stick]->setFromInt(1);
      value[ID::stickDecay]->setFromNormalized(0.7319999933242798);
      value[ID::stickToneMix]->setFromNormalized(0.5);
      value[ID::smoothness]->setFromNormalized(0.1);
      value[ID::gain]->setFromNormalized(0.5);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetGlassy: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(3362196);
      value[ID::retriggerTime]->setFromInt(1);
      value[ID::retriggerStick]->setFromInt(0);
      value[ID::retriggerTremolo]->setFromInt(0);
      value[ID::fdn]->setFromInt(1);
      value[ID::fdnTime]->setFromNormalized(0.17600004374980927);
      value[ID::fdnFeedback]->setFromNormalized(0.24400001764297485);
      value[ID::fdnCascadeMix]->setFromNormalized(0.4159999787807465);
      value[ID::allpassMix]->setFromNormalized(1.0);
      value[ID::allpass1Saturation]->setFromInt(0);
      value[ID::allpass1Time]->setFromNormalized(0.11600007861852649);
      value[ID::allpass1Feedback]->setFromNormalized(0.9759999513626098);
      value[ID::allpass1HighpassCutoff]->setFromNormalized(0.5);
      value[ID::allpass2Time]->setFromNormalized(0.26400014758110046);
      value[ID::allpass2Feedback]->setFromNormalized(0.9639999270439145);
      value[ID::allpass2HighpassCutoff]->setFromNormalized(0.3319999873638153);
      value[ID::tremoloMix]->setFromNormalized(0.2);
      value[ID::tremoloDepth]->setFromNormalized(0.8);
      value[ID::tremoloFrequency]->setFromNormalized(0.5);
      value[ID::tremoloDelayTime]->setFromNormalized(0.25000000000000006);
      value[ID::randomTremoloDepth]->setFromNormalized(0.35);
      value[ID::randomTremoloFrequency]->setFromNormalized(0.35);
      value[ID::randomTremoloDelayTime]->setFromNormalized(0.35);
      value[ID::stick]->setFromInt(1);
      value[ID::stickDecay]->setFromNormalized(0.28800001740455633);
      value[ID::stickToneMix]->setFromNormalized(0.0);
      value[ID::smoothness]->setFromNormalized(0.1);
      value[ID::gain]->setFromNormalized(0.5);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetLongExcitation: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(11415258);
      value[ID::retriggerTime]->setFromInt(1);
      value[ID::retriggerStick]->setFromInt(0);
      value[ID::retriggerTremolo]->setFromInt(0);
      value[ID::fdn]->setFromInt(1);
      value[ID::fdnTime]->setFromNormalized(0.2);
      value[ID::fdnFeedback]->setFromNormalized(0.5);
      value[ID::fdnCascadeMix]->setFromNormalized(0.5);
      value[ID::allpassMix]->setFromNormalized(0.75);
      value[ID::allpass1Saturation]->setFromInt(1);
      value[ID::allpass1Time]->setFromNormalized(0.5);
      value[ID::allpass1Feedback]->setFromNormalized(0.75);
      value[ID::allpass1HighpassCutoff]->setFromNormalized(0.5);
      value[ID::allpass2Time]->setFromNormalized(0.5);
      value[ID::allpass2Feedback]->setFromNormalized(0.5);
      value[ID::allpass2HighpassCutoff]->setFromNormalized(0.5);
      value[ID::tremoloMix]->setFromNormalized(0.1920001208782196);
      value[ID::tremoloDepth]->setFromNormalized(0.740000307559967);
      value[ID::tremoloFrequency]->setFromNormalized(0.41200003027915955);
      value[ID::tremoloDelayTime]->setFromNormalized(0.25000000000000006);
      value[ID::randomTremoloDepth]->setFromNormalized(0.6100000739097595);
      value[ID::randomTremoloFrequency]->setFromNormalized(0.5979999303817749);
      value[ID::randomTremoloDelayTime]->setFromNormalized(0.35);
      value[ID::stick]->setFromInt(1);
      value[ID::stickDecay]->setFromNormalized(0.8040000200271606);
      value[ID::stickToneMix]->setFromNormalized(0.5);
      value[ID::smoothness]->setFromNormalized(0.1);
      value[ID::gain]->setFromNormalized(0.5);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetRandomShortTime: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(11952129);
      value[ID::retriggerTime]->setFromInt(0);
      value[ID::retriggerStick]->setFromInt(0);
      value[ID::retriggerTremolo]->setFromInt(0);
      value[ID::fdn]->setFromInt(1);
      value[ID::fdnTime]->setFromNormalized(0.2);
      value[ID::fdnFeedback]->setFromNormalized(0.5);
      value[ID::fdnCascadeMix]->setFromNormalized(0.5);
      value[ID::allpassMix]->setFromNormalized(0.75);
      value[ID::allpass1Saturation]->setFromInt(1);
      value[ID::allpass1Time]->setFromNormalized(0.03600005805492402);
      value[ID::allpass1Feedback]->setFromNormalized(0.8220000863075256);
      value[ID::allpass1HighpassCutoff]->setFromNormalized(0.5);
      value[ID::allpass2Time]->setFromNormalized(0.05600006133317948);
      value[ID::allpass2Feedback]->setFromNormalized(0.7840001583099367);
      value[ID::allpass2HighpassCutoff]->setFromNormalized(0.5);
      value[ID::tremoloMix]->setFromNormalized(0.2);
      value[ID::tremoloDepth]->setFromNormalized(0.8);
      value[ID::tremoloFrequency]->setFromNormalized(0.5);
      value[ID::tremoloDelayTime]->setFromNormalized(0.25000000000000006);
      value[ID::randomTremoloDepth]->setFromNormalized(0.35);
      value[ID::randomTremoloFrequency]->setFromNormalized(0.35);
      value[ID::randomTremoloDelayTime]->setFromNormalized(0.35);
      value[ID::stick]->setFromInt(1);
      value[ID::stickDecay]->setFromNormalized(0.5);
      value[ID::stickToneMix]->setFromNormalized(1.0);
      value[ID::smoothness]->setFromNormalized(0.1);
      value[ID::gain]->setFromNormalized(0.5);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetRetriggerStickOscillator: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(15307574);
      value[ID::retriggerTime]->setFromInt(1);
      value[ID::retriggerStick]->setFromInt(1);
      value[ID::retriggerTremolo]->setFromInt(0);
      value[ID::fdn]->setFromInt(1);
      value[ID::fdnTime]->setFromNormalized(0.33199992775917053);
      value[ID::fdnFeedback]->setFromNormalized(0.5);
      value[ID::fdnCascadeMix]->setFromNormalized(0.5);
      value[ID::allpassMix]->setFromNormalized(1.0);
      value[ID::allpass1Saturation]->setFromInt(1);
      value[ID::allpass1Time]->setFromNormalized(0.5);
      value[ID::allpass1Feedback]->setFromNormalized(0.0);
      value[ID::allpass1HighpassCutoff]->setFromNormalized(0.0);
      value[ID::allpass2Time]->setFromNormalized(0.9120000004768372);
      value[ID::allpass2Feedback]->setFromNormalized(0.6239998936653139);
      value[ID::allpass2HighpassCutoff]->setFromNormalized(0.5);
      value[ID::tremoloMix]->setFromNormalized(0.2);
      value[ID::tremoloDepth]->setFromNormalized(0.8);
      value[ID::tremoloFrequency]->setFromNormalized(0.5);
      value[ID::tremoloDelayTime]->setFromNormalized(0.25000000000000006);
      value[ID::randomTremoloDepth]->setFromNormalized(0.35);
      value[ID::randomTremoloFrequency]->setFromNormalized(0.35);
      value[ID::randomTremoloDelayTime]->setFromNormalized(0.35);
      value[ID::stick]->setFromInt(1);
      value[ID::stickDecay]->setFromNormalized(0.5);
      value[ID::stickToneMix]->setFromNormalized(0.5);
      value[ID::smoothness]->setFromNormalized(0.1);
      value[ID::gain]->setFromNormalized(0.5);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetSmooth: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(7522945);
      value[ID::retriggerTime]->setFromInt(0);
      value[ID::retriggerStick]->setFromInt(0);
      value[ID::retriggerTremolo]->setFromInt(0);
      value[ID::fdn]->setFromInt(1);
      value[ID::fdnTime]->setFromNormalized(0.41599997878074646);
      value[ID::fdnFeedback]->setFromNormalized(0.5360000729560852);
      value[ID::fdnCascadeMix]->setFromNormalized(0.09199995547533037);
      value[ID::allpassMix]->setFromNormalized(0.6220000386238098);
      value[ID::allpass1Saturation]->setFromInt(0);
      value[ID::allpass1Time]->setFromNormalized(0.18800005316734317);
      value[ID::allpass1Feedback]->setFromNormalized(0.19399996101856226);
      value[ID::allpass1HighpassCutoff]->setFromNormalized(0.5);
      value[ID::allpass2Time]->setFromNormalized(0.11200007051229478);
      value[ID::allpass2Feedback]->setFromNormalized(0.05600006878376008);
      value[ID::allpass2HighpassCutoff]->setFromNormalized(0.5);
      value[ID::tremoloMix]->setFromNormalized(0.1719999760389328);
      value[ID::tremoloDepth]->setFromNormalized(0.18400001525878906);
      value[ID::tremoloFrequency]->setFromNormalized(0.9240000247955322);
      value[ID::tremoloDelayTime]->setFromNormalized(0.7219998836517334);
      value[ID::randomTremoloDepth]->setFromNormalized(0.862000048160553);
      value[ID::randomTremoloFrequency]->setFromNormalized(0.5500000715255737);
      value[ID::randomTremoloDelayTime]->setFromNormalized(0.6899999380111694);
      value[ID::stick]->setFromInt(1);
      value[ID::stickDecay]->setFromNormalized(0.4159999489784241);
      value[ID::stickToneMix]->setFromNormalized(0.35200002789497375);
      value[ID::smoothness]->setFromNormalized(0.3479999601840973);
      value[ID::gain]->setFromNormalized(0.49625468254089355);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetTooMuchDelayTime: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(9603320);
      value[ID::retriggerTime]->setFromInt(1);
      value[ID::retriggerStick]->setFromInt(0);
      value[ID::retriggerTremolo]->setFromInt(0);
      value[ID::fdn]->setFromInt(1);
      value[ID::fdnTime]->setFromNormalized(0.6719999313354492);
      value[ID::fdnFeedback]->setFromNormalized(0.5);
      value[ID::fdnCascadeMix]->setFromNormalized(0.5);
      value[ID::allpassMix]->setFromNormalized(0.75);
      value[ID::allpass1Saturation]->setFromInt(1);
      value[ID::allpass1Time]->setFromNormalized(0.5);
      value[ID::allpass1Feedback]->setFromNormalized(0.75);
      value[ID::allpass1HighpassCutoff]->setFromNormalized(0.5);
      value[ID::allpass2Time]->setFromNormalized(0.5);
      value[ID::allpass2Feedback]->setFromNormalized(0.5);
      value[ID::allpass2HighpassCutoff]->setFromNormalized(0.5);
      value[ID::tremoloMix]->setFromNormalized(1.0);
      value[ID::tremoloDepth]->setFromNormalized(0.0);
      value[ID::tremoloFrequency]->setFromNormalized(0.6040000319480896);
      value[ID::tremoloDelayTime]->setFromNormalized(1.0);
      value[ID::randomTremoloDepth]->setFromNormalized(0.0);
      value[ID::randomTremoloFrequency]->setFromNormalized(0.0);
      value[ID::randomTremoloDelayTime]->setFromNormalized(0.0);
      value[ID::stick]->setFromInt(1);
      value[ID::stickDecay]->setFromNormalized(0.5);
      value[ID::stickToneMix]->setFromNormalized(0.5);
      value[ID::smoothness]->setFromNormalized(0.1);
      value[ID::gain]->setFromNormalized(0.5);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;

    case presetTurnRight: {
      value[ID::bypass]->setFromInt(0);
      value[ID::seed]->setFromInt(15039140);
      value[ID::retriggerTime]->setFromInt(1);
      value[ID::retriggerStick]->setFromInt(0);
      value[ID::retriggerTremolo]->setFromInt(0);
      value[ID::fdn]->setFromInt(1);
      value[ID::fdnTime]->setFromNormalized(1.0);
      value[ID::fdnFeedback]->setFromNormalized(0.28800004720687866);
      value[ID::fdnCascadeMix]->setFromNormalized(1.0);
      value[ID::allpassMix]->setFromNormalized(1.0);
      value[ID::allpass1Saturation]->setFromInt(1);
      value[ID::allpass1Time]->setFromNormalized(1.0);
      value[ID::allpass1Feedback]->setFromNormalized(0.75);
      value[ID::allpass1HighpassCutoff]->setFromNormalized(1.0);
      value[ID::allpass2Time]->setFromNormalized(1.0);
      value[ID::allpass2Feedback]->setFromNormalized(1.0);
      value[ID::allpass2HighpassCutoff]->setFromNormalized(1.0);
      value[ID::tremoloMix]->setFromNormalized(0.11200007796287537);
      value[ID::tremoloDepth]->setFromNormalized(1.0);
      value[ID::tremoloFrequency]->setFromNormalized(0.23200008273124692);
      value[ID::tremoloDelayTime]->setFromNormalized(1.0);
      value[ID::randomTremoloDepth]->setFromNormalized(1.0);
      value[ID::randomTremoloFrequency]->setFromNormalized(1.0);
      value[ID::randomTremoloDelayTime]->setFromNormalized(1.0);
      value[ID::stick]->setFromInt(1);
      value[ID::stickDecay]->setFromNormalized(1.0);
      value[ID::stickToneMix]->setFromNormalized(1.0);
      value[ID::smoothness]->setFromNormalized(0.1);
      value[ID::gain]->setFromNormalized(0.5861423015594482);
      value[ID::pitchBend]->setFromNormalized(0.5);
    } break;
  }
}
