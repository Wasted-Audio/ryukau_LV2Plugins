// (c) 2019-2020 Takamitsu Endo
//
// This file is part of SevenDelay.
//
// SevenDelay is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SevenDelay is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SevenDelay.  If not, see <https://www.gnu.org/licenses/>.

#include "parameter.hpp"
#include "../common/dsp/constants.hpp"

using namespace SomeDSP;

IntScale<double> Scales::boolScale(1);
LinearScale<double> Scales::defaultScale(0.0, 1.0);
LogScale<double> Scales::time(0.0001, maxDelayTime, 0.5, 1.0);
SPolyScale<double> Scales::offset(-1.0, 1.0, 0.8);
LogScale<double> Scales::lfoTimeAmount(0, 1.0, 0.5, 0.07);
LogScale<double> Scales::lfoToneAmount(0, 0.5, 0.5, 0.1);
IntScale<double> Scales::lfoTempoNumerator(255);
IntScale<double> Scales::lfoTempoDenominator(255);
LogScale<double> Scales::lfoFrequencyMultiplier(0.0, 16.0, 0.5, 1.0);
LogScale<double> Scales::lfoFrequency(0.01, 100.0, 0.5, 1.0);
LogScale<double> Scales::lfoShape(0.01, 10.0, 0.5, 1.0);
LinearScale<double> Scales::lfoInitialPhase(0.0, 2.0 * pi);
LogScale<double> Scales::smoothness(0.0, 1.0, 0.3, 0.04);
LogScale<double> Scales::toneCutoff(90.0, maxToneFrequency, 0.5, 1000.0);
LogScale<double> Scales::toneQ(1e-5, 1.0, 0.5, 0.1);
LogScale<double> Scales::toneMix(0.0, 1.0, 0.9, 0.05);
LogScale<double> Scales::dckill(minDCKillFrequency, 120.0, 0.5, 20.0);
LogScale<double> Scales::dckillMix(0.0, 1.0, 0.9, 0.05);

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
      value[ID::time]->setFromNormalized(0.5);
      value[ID::feedback]->setFromNormalized(0.625);
      value[ID::offset]->setFromNormalized(0.5);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.0);
      value[ID::lfoToneAmount]->setFromNormalized(0.0);
      value[ID::lfoFrequency]->setFromNormalized(0.5);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(1.0);
      value[ID::toneQ]->setFromNormalized(0.9);
      value[ID::dckill]->setFromNormalized(0.0);
    } break;

    case preset3_16Invert: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.726374);
      value[ID::feedback]->setFromNormalized(0.625);
      value[ID::offset]->setFromNormalized(0.5);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(1);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.0);
      value[ID::lfoToneAmount]->setFromNormalized(0.0);
      value[ID::lfoFrequency]->setFromNormalized(0.5);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(1.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(1.0);
      value[ID::toneQ]->setFromNormalized(0.9);
      value[ID::dckill]->setFromNormalized(0.0);
    } break;

    case preset3_16PingPong: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.726374);
      value[ID::feedback]->setFromNormalized(0.625);
      value[ID::offset]->setFromNormalized(0.5);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(1);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.0);
      value[ID::lfoToneAmount]->setFromNormalized(0.0);
      value[ID::lfoFrequency]->setFromNormalized(0.5);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(1.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(1.0);
      value[ID::toneQ]->setFromNormalized(0.9);
      value[ID::dckill]->setFromNormalized(0.0);
    } break;

    case presetBend: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.144);
      value[ID::feedback]->setFromNormalized(0.625);
      value[ID::offset]->setFromNormalized(0.5);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.547826);
      value[ID::lfoToneAmount]->setFromNormalized(1.0);
      value[ID::lfoFrequency]->setFromNormalized(0.595652);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(0.196);
      value[ID::toneQ]->setFromNormalized(0.092);
      value[ID::dckill]->setFromNormalized(0.0);
    } break;

    case presetChorus: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.124);
      value[ID::feedback]->setFromNormalized(0.524);
      value[ID::offset]->setFromNormalized(0.5);
      value[ID::wetMix]->setFromNormalized(0.862);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.182609);
      value[ID::lfoToneAmount]->setFromNormalized(0.717391);
      value[ID::lfoFrequency]->setFromNormalized(0.5);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(1.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(0.788);
      value[ID::toneQ]->setFromNormalized(0.308);
      value[ID::dckill]->setFromNormalized(0.0);
    } break;

    case presetCloseToPhaser: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.032);
      value[ID::feedback]->setFromNormalized(0.929);
      value[ID::offset]->setFromNormalized(0.5);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.0);
      value[ID::lfoToneAmount]->setFromNormalized(1.0);
      value[ID::lfoFrequency]->setFromNormalized(0.413043);
      value[ID::lfoShape]->setFromNormalized(0.696);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(0.924);
      value[ID::toneQ]->setFromNormalized(0.952);
      value[ID::dckill]->setFromNormalized(0.068);
    } break;

    case presetFlapping: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.044);
      value[ID::feedback]->setFromNormalized(0.969);
      value[ID::offset]->setFromNormalized(0.5);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.0);
      value[ID::lfoToneAmount]->setFromNormalized(1.0);
      value[ID::lfoFrequency]->setFromNormalized(0.586957);
      value[ID::lfoShape]->setFromNormalized(0.228);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(0.808);
      value[ID::toneQ]->setFromNormalized(1.0);
      value[ID::dckill]->setFromNormalized(0.0);
    } break;

    case presetGhostVibrato: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.08);
      value[ID::feedback]->setFromNormalized(0.925);
      value[ID::offset]->setFromNormalized(0.524233);
      value[ID::wetMix]->setFromNormalized(0.258);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(1);
      value[ID::lfoTimeAmount]->setFromNormalized(0.130435);
      value[ID::lfoToneAmount]->setFromNormalized(0.0);
      value[ID::lfoFrequency]->setFromNormalized(0.643478);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.5);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(1.0);
      value[ID::toneQ]->setFromNormalized(0.984);
      value[ID::dckill]->setFromNormalized(0.0);
    } break;

    case presetHigh: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.028);
      value[ID::feedback]->setFromNormalized(0.965);
      value[ID::offset]->setFromNormalized(0.248);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(1);
      value[ID::lfoTimeAmount]->setFromNormalized(0.0);
      value[ID::lfoToneAmount]->setFromNormalized(0.995652);
      value[ID::lfoFrequency]->setFromNormalized(0.83913);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(1.0);
      value[ID::toneQ]->setFromNormalized(0.052);
      value[ID::dckill]->setFromNormalized(0.308);
    } break;

    case presetLean: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.324);
      value[ID::feedback]->setFromNormalized(1.0);
      value[ID::offset]->setFromNormalized(0.956);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.0);
      value[ID::lfoToneAmount]->setFromNormalized(0.508696);
      value[ID::lfoFrequency]->setFromNormalized(0.730435);
      value[ID::lfoShape]->setFromNormalized(0.768);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(0.308);
      value[ID::toneQ]->setFromNormalized(0.372);
      value[ID::dckill]->setFromNormalized(0.9);
    } break;

    case presetMaxFeedback: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.08);
      value[ID::feedback]->setFromNormalized(1.0);
      value[ID::offset]->setFromNormalized(0.5);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.143478);
      value[ID::lfoToneAmount]->setFromNormalized(0.0);
      value[ID::lfoFrequency]->setFromNormalized(0.5);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.928);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(1.0);
      value[ID::toneQ]->setFromNormalized(0.9);
      value[ID::dckill]->setFromNormalized(0.0);
    } break;

    case presetModeratelyCentered: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.62);
      value[ID::feedback]->setFromNormalized(0.592);
      value[ID::offset]->setFromNormalized(0.408);
      value[ID::wetMix]->setFromNormalized(1.0);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(1);
      value[ID::negativeFeedback]->setFromInt(1);
      value[ID::lfoTimeAmount]->setFromNormalized(0.0);
      value[ID::lfoToneAmount]->setFromNormalized(1.0);
      value[ID::lfoFrequency]->setFromNormalized(0.673913);
      value[ID::lfoShape]->setFromNormalized(0.384);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.824);
      value[ID::inPan]->setFromNormalized(0.66);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.344);
      value[ID::toneCutoff]->setFromNormalized(0.188);
      value[ID::toneQ]->setFromNormalized(0.236);
      value[ID::dckill]->setFromNormalized(0.596);
    } break;

    case presetNoise: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.176);
      value[ID::feedback]->setFromNormalized(1.0);
      value[ID::offset]->setFromNormalized(1.0);
      value[ID::wetMix]->setFromNormalized(0.098);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(1);
      value[ID::lfoTimeAmount]->setFromNormalized(1.0);
      value[ID::lfoToneAmount]->setFromNormalized(1.0);
      value[ID::lfoFrequency]->setFromNormalized(1.0);
      value[ID::lfoShape]->setFromNormalized(0.268);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(1.0);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(0.724);
      value[ID::toneQ]->setFromNormalized(0.132);
      value[ID::dckill]->setFromNormalized(0.08);
    } break;

    case presetPower: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.236);
      value[ID::feedback]->setFromNormalized(0.625);
      value[ID::offset]->setFromNormalized(0.502402);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.504348);
      value[ID::lfoToneAmount]->setFromNormalized(0.0);
      value[ID::lfoFrequency]->setFromNormalized(0.430435);
      value[ID::lfoShape]->setFromNormalized(1.0);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.668);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(1.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(1.0);
      value[ID::toneQ]->setFromNormalized(0.9);
      value[ID::dckill]->setFromNormalized(0.372);
    } break;

    case presetResponse: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.24);
      value[ID::feedback]->setFromNormalized(0.217);
      value[ID::offset]->setFromNormalized(0.5);
      value[ID::wetMix]->setFromNormalized(0.966);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.0);
      value[ID::lfoToneAmount]->setFromNormalized(0.0);
      value[ID::lfoFrequency]->setFromNormalized(0.5);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.784);
      value[ID::inPan]->setFromNormalized(0.264);
      value[ID::outSpread]->setFromNormalized(1.0);
      value[ID::outPan]->setFromNormalized(0.86);
      value[ID::toneCutoff]->setFromNormalized(1.0);
      value[ID::toneQ]->setFromNormalized(0.9);
      value[ID::dckill]->setFromNormalized(0.0);
    } break;

    case presetSlowLFO: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.404);
      value[ID::feedback]->setFromNormalized(0.625);
      value[ID::offset]->setFromNormalized(0.328);
      value[ID::wetMix]->setFromNormalized(0.542);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.547826);
      value[ID::lfoToneAmount]->setFromNormalized(0.0);
      value[ID::lfoFrequency]->setFromNormalized(0.265217);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.564);
      value[ID::outSpread]->setFromNormalized(1.0);
      value[ID::outPan]->setFromNormalized(0.396);
      value[ID::toneCutoff]->setFromNormalized(1.0);
      value[ID::toneQ]->setFromNormalized(0.9);
      value[ID::dckill]->setFromNormalized(0.064);
    } break;

    case presetSomewhere: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.168);
      value[ID::feedback]->setFromNormalized(0.049);
      value[ID::offset]->setFromNormalized(0.384);
      value[ID::wetMix]->setFromNormalized(0.424);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.0);
      value[ID::lfoToneAmount]->setFromNormalized(0.430435);
      value[ID::lfoFrequency]->setFromNormalized(0.969565);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(1.0);
      value[ID::inPan]->setFromNormalized(0.308);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.188);
      value[ID::toneCutoff]->setFromNormalized(0.324);
      value[ID::toneQ]->setFromNormalized(0.9);
      value[ID::dckill]->setFromNormalized(0.164);
    } break;

    case presetTail: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.636);
      value[ID::feedback]->setFromNormalized(0.837);
      value[ID::offset]->setFromNormalized(0.176);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(1);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.0);
      value[ID::lfoToneAmount]->setFromNormalized(0.0);
      value[ID::lfoFrequency]->setFromNormalized(0.5);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.772);
      value[ID::outSpread]->setFromNormalized(1.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(0.064);
      value[ID::toneQ]->setFromNormalized(0.22);
      value[ID::dckill]->setFromNormalized(0.14);
    } break;

    case presetTurnUpTimeKnob: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.156);
      value[ID::feedback]->setFromNormalized(0.829);
      value[ID::offset]->setFromNormalized(0.436);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(0.0);
      value[ID::lfoToneAmount]->setFromNormalized(0.695652);
      value[ID::lfoFrequency]->setFromNormalized(0.169565);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(1.0);
      value[ID::inSpread]->setFromNormalized(1.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(0.508);
      value[ID::toneQ]->setFromNormalized(0.32);
      value[ID::dckill]->setFromNormalized(0.0);
    } break;

    case presetWaitForScratch: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.852);
      value[ID::feedback]->setFromNormalized(1.0);
      value[ID::offset]->setFromNormalized(0.436);
      value[ID::wetMix]->setFromNormalized(1.0);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(0);
      value[ID::lfoTimeAmount]->setFromNormalized(1.0);
      value[ID::lfoToneAmount]->setFromNormalized(0.0);
      value[ID::lfoFrequency]->setFromNormalized(0.1);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(1.0);
      value[ID::toneQ]->setFromNormalized(0.9);
      value[ID::dckill]->setFromNormalized(0.0);
    } break;

    case presetWandering: {
      value[ID::bypass]->setFromInt(0);
      value[ID::time]->setFromNormalized(0.34);
      value[ID::feedback]->setFromNormalized(0.625);
      value[ID::offset]->setFromNormalized(0.5);
      value[ID::wetMix]->setFromNormalized(0.75);
      value[ID::dryMix]->setFromNormalized(1.0);
      value[ID::tempoSync]->setFromInt(0);
      value[ID::negativeFeedback]->setFromInt(1);
      value[ID::lfoTimeAmount]->setFromNormalized(0.221739);
      value[ID::lfoToneAmount]->setFromNormalized(1.0);
      value[ID::lfoFrequency]->setFromNormalized(0.595652);
      value[ID::lfoShape]->setFromNormalized(0.5);
      value[ID::lfoInitialPhase]->setFromNormalized(0.0);
      value[ID::lfoHold]->setFromInt(0);
      value[ID::smoothness]->setFromNormalized(0.3);
      value[ID::inSpread]->setFromNormalized(0.0);
      value[ID::inPan]->setFromNormalized(0.5);
      value[ID::outSpread]->setFromNormalized(0.0);
      value[ID::outPan]->setFromNormalized(0.5);
      value[ID::toneCutoff]->setFromNormalized(0.196);
      value[ID::toneQ]->setFromNormalized(0.092);
      value[ID::dckill]->setFromNormalized(0.188);
    } break;
  }
}
