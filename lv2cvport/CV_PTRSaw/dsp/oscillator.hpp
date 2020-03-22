// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_PTRSaw.
//
// CV_PTRSaw is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_PTRSaw is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_PTRSaw.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cinttypes>
#include <cmath>

namespace SomeDSP {

class PTRSyncSaw {
public:
  void setup(double sampleRate)
  {
    this->sampleRate = sampleRate;
    setOscFreq(0);
    setSyncFreq(0);
  }

  void setOscFreq(double hz) { oscTick = fabs(hz) / sampleRate; }
  void setSyncFreq(double hz) { syncTick = fabs(hz) / sampleRate; }
  void setPhase(double phase) { oscPhase = phase - floor(phase); }

  void reset()
  {
    oscPhase = 0;
    height = 0;
    syncPhase = 0;
    lastSig = 0;
  }

  double process(double modOsc = 0, double modSync = 0)
  {
    syncPhase += syncTick + modSync;
    if (syncPhase >= double(1) || syncPhase < 0) {
      syncPhase -= floor(syncPhase);

      oscPhase = syncPhase;
      if (syncTick != 0) {
        auto ratio = oscTick / syncTick;
        height = ratio - floor(ratio);
      } else {
        height = lastSig;
        lastSig = ptrSaw10Double(oscPhase, oscTick, height);
        lastSig
          = lastSig > double(1) ? double(1) : lastSig < double(-1) ? double(-1) : lastSig;
        return lastSig;
      }
    } else {
      oscPhase += oscTick + modOsc;
      if (oscPhase >= double(1) || oscPhase < 0) {
        height = double(1);
        oscPhase -= floor(oscPhase);
      }
    }
    lastSig = ptrSaw10Double(oscPhase, oscTick, height);
    if (!std::isfinite(lastSig)) lastSig = 0;
    return lastSig;
  }

protected:
  double sampleRate = 44100;
  double oscPhase = 0; // Range in [0, 1)
  double oscTick = 0;  // sec/sample
  double height = 1;   // Range in [0, 1]. Sample value at phase reset of hardsync.
  double syncPhase = 0;
  double syncTick = 0;
  double lastSig = 0;

  double ptrSaw10Double(double phi, double T, double h = 1)
  {
    double n = phi / T;
    if (n >= double(9)) return double(2) * T * n - double(10) * T - double(1);
    if (n < double(1))
      return -(h * n * n * n * n * n * n * n * n * n * n) / double(1814400)
        + double(2) * T * n + double(2) * h - double(10) * T - double(1);
    if (n < double(2))
      return (h * n * n * n * n * n * n * n * n * n * n) / double(201600)
        - (h * n * n * n * n * n * n * n * n * n) / double(18144)
        + (h * n * n * n * n * n * n * n * n) / double(4032)
        - (h * n * n * n * n * n * n * n) / double(1512)
        + (h * n * n * n * n * n * n) / double(864)
        - (h * n * n * n * n * n) / double(720) + (h * n * n * n * n) / double(864)
        - (h * n * n * n) / double(1512) + (h * n * n) / double(4032)
        - (h * n) / double(18144) + double(2) * T * n
        + (double(362881) * h) / double(181440) - double(10) * T - double(1);
    if (n < double(3))
      return -(h * n * n * n * n * n * n * n * n * n * n) / double(50400)
        + (h * n * n * n * n * n * n * n * n * n) / double(2268)
        - (double(17) * h * n * n * n * n * n * n * n * n) / double(4032)
        + (double(5) * h * n * n * n * n * n * n * n) / double(216)
        - (double(71) * h * n * n * n * n * n * n) / double(864)
        + (double(143) * h * n * n * n * n * n) / double(720)
        - (double(287) * h * n * n * n * n) / double(864)
        + (double(575) * h * n * n * n) / double(1512)
        - (double(1151) * h * n * n) / double(4032) + (double(329) * h * n) / double(2592)
        + double(2) * T * n + (double(358273) * h) / double(181440) - double(10) * T
        - double(1);
    if (n < double(4))
      return (h * n * n * n * n * n * n * n * n * n * n) / double(21600)
        - (h * n * n * n * n * n * n * n * n * n) / double(648)
        + (double(13) * h * n * n * n * n * n * n * n * n) / double(576)
        - (double(289) * h * n * n * n * n * n * n * n) / double(1512)
        + (double(901) * h * n * n * n * n * n * n) / double(864)
        - (double(2773) * h * n * n * n * n * n) / double(720)
        + (double(8461) * h * n * n * n * n) / double(864)
        - (double(3667) * h * n * n * n) / double(216)
        + (double(11083) * h * n * n) / double(576)
        - (double(233893) * h * n) / double(18144) + double(2) * T * n
        + (double(1066861) * h) / double(181440) - double(10) * T - double(1);
    if (n < double(5))
      return -(h * n * n * n * n * n * n * n * n * n * n) / double(14400)
        + (h * n * n * n * n * n * n * n * n * n) / double(324)
        - (double(35) * h * n * n * n * n * n * n * n * n) / double(576)
        + (double(1055) * h * n * n * n * n * n * n * n) / double(1512)
        - (double(4475) * h * n * n * n * n * n * n) / double(864)
        + (double(18731) * h * n * n * n * n * n) / double(720)
        - (double(77555) * h * n * n * n * n) / double(864)
        + (double(45485) * h * n * n * n) / double(216)
        - (double(185525) * h * n * n) / double(576)
        + (double(5271131) * h * n) / double(18144) + double(2) * T * n
        - (double(4190647) * h) / double(36288) - double(10) * T - double(1);
    if (n < double(6))
      return (h * n * n * n * n * n * n * n * n * n * n) / double(14400)
        - (double(5) * h * n * n * n * n * n * n * n * n * n) / double(1296)
        + (double(55) * h * n * n * n * n * n * n * n * n) / double(576)
        - (double(2095) * h * n * n * n * n * n * n * n) / double(1512)
        + (double(11275) * h * n * n * n * n * n * n) / double(864)
        - (double(60019) * h * n * n * n * n * n) / double(720)
        + (double(316195) * h * n * n * n * n) / double(864)
        - (double(235765) * h * n * n * n) / double(216)
        + (double(1220725) * h * n * n) / double(576)
        - (double(43947619) * h * n) / double(18144) + double(2) * T * n
        + (double(45028103) * h) / double(36288) - double(10) * T - double(1);
    if (n < double(7))
      return -(h * n * n * n * n * n * n * n * n * n * n) / double(21600)
        + (h * n * n * n * n * n * n * n * n * n) / double(324)
        - (double(53) * h * n * n * n * n * n * n * n * n) / double(576)
        + (double(2441) * h * n * n * n * n * n * n * n) / double(1512)
        - (double(15941) * h * n * n * n * n * n * n) / double(864)
        + (double(103277) * h * n * n * n * n * n) / double(720)
        - (double(663581) * h * n * n * n * n) / double(864)
        + (double(604043) * h * n * n * n) / double(216)
        - (double(3818123) * h * n * n) / double(576)
        + (double(167683997) * h * n) / double(18144) + double(2) * T * n
        - (double(1044649181) * h) / double(181440) - double(10) * T - double(1);
    if (n < double(8))
      return (h * n * n * n * n * n * n * n * n * n * n) / double(50400)
        - (h * n * n * n * n * n * n * n * n * n) / double(648)
        + (double(31) * h * n * n * n * n * n * n * n * n) / double(576)
        - (double(1675) * h * n * n * n * n * n * n * n) / double(1512)
        + (double(12871) * h * n * n * n * n * n * n) / double(864)
        - (double(98407) * h * n * n * n * n * n) / double(720)
        + (double(748207) * h * n * n * n * n) / double(864)
        - (double(807745) * h * n * n * n) / double(216)
        + (double(6064393) * h * n * n) / double(576)
        - (double(316559287) * h * n) / double(18144) + double(2) * T * n
        + (double(2345053807) * h) / double(181440) - double(10) * T - double(1);
    if (n < double(9))
      return -(h * n * n * n * n * n * n * n * n * n * n) / double(201600)
        + (h * n * n * n * n * n * n * n * n * n) / double(2268)
        - (double(71) * h * n * n * n * n * n * n * n * n) / double(4032)
        + (double(629) * h * n * n * n * n * n * n * n) / double(1512)
        - (double(5561) * h * n * n * n * n * n * n) / double(864)
        + (double(49049) * h * n * n * n * n * n) / double(720)
        - (double(431441) * h * n * n * n * n) / double(864)
        + (double(3782969) * h * n * n * n) / double(1512)
        - (double(33046721) * h * n * n) / double(4032)
        + (double(287420489) * h * n) / double(18144) + double(2) * T * n
        - (double(2486784401) * h) / double(181440) - double(10) * T - double(1);
    if (n < double(10))
      return (h * n * n * n * n * n * n * n * n * n * n) / double(1814400)
        - (h * n * n * n * n * n * n * n * n * n) / double(18144)
        + (double(5) * h * n * n * n * n * n * n * n * n) / double(2016)
        - (double(25) * h * n * n * n * n * n * n * n) / double(378)
        + (double(125) * h * n * n * n * n * n * n) / double(108)
        - (double(125) * h * n * n * n * n * n) / double(9)
        + (double(3125) * h * n * n * n * n) / double(27)
        - (double(125000) * h * n * n * n) / double(189)
        + (double(156250) * h * n * n) / double(63)
        - (double(3125000) * h * n) / double(567) + double(2) * T * n
        + (double(3125000) * h) / double(567) - double(10) * T - double(1);
    return 0.0; // Just in case.
  }
};

} // namespace SomeDSP
