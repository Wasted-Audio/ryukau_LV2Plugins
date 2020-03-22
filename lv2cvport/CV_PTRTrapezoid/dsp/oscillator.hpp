// (c) 2019-2020 Takamitsu Endo
//
// This file is part of CV_PTRTrapezoid.
//
// CV_PTRTrapezoid is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_PTRTrapezoid is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_PTRTrapezoid.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <algorithm>
#include <cinttypes>
#include <cmath>

namespace SomeDSP {

class PTRTrapezoidOsc {
public:
  void setup(double sampleRate)
  {
    this->sampleRate = sampleRate;
    setFreq(0);
  }

  void setFreq(double hz) { tick = 2.0 * fabs(hz) / sampleRate; }
  void setPhase(double phase) { this->phase = phase; }
  void addPhase(double phase) { this->phase += phase; }
  void setSlope(double slope) { this->slope = fabs(slope); }
  void setPulseWidth(double pw) { this->pw = std::clamp<double>(pw, 0.0, 1.0); }
  void reset() { phase = 0; }

  double process()
  {
    if (tick <= 0) return 0;
    phase += tick;
    phase -= floor(phase + tick);
    return ptrTpz5(phase, tick, slope, pw);
  }

  double sampleRate;
  double phase = 0;
  double tick = 0;
  double slope = 8;
  double pw = double(0.5); // Pulse width.

protected:
  // tick must be greater than 0.
  static double ptrTpz5(double phase, double tick, double slope, double pw)
  {
    uint32_t order = 5;
    if (order > double(0.25) / tick) order = int(double(0.25) / tick);

    const double ptrLen = order * tick;

    const double maxSlope = double(0.25) / ptrLen;
    if (slope > maxSlope) {
      slope = maxSlope;
    } else {
      const double minSlope = double(1);
      if (slope < minSlope) slope = minSlope;
    }

    const double maxPw = double(1) - double(1) / slope;
    if (pw > maxPw) pw = std::max<double>(double(0), maxPw);

    const double y = double(1) - double(2) * slope * ptrLen;
    const double dc = (y * y + pw * slope * y) / (double(2) * y + slope - double(1));
    if (order == 5)
      return branch<PTRTrapezoidOsc::ptrRamp5>(slope, pw, y, phase, tick) - dc;
    else if (order == 4)
      return branch<PTRTrapezoidOsc::ptrRamp4>(slope, pw, y, phase, tick) - dc;
    else if (order == 3)
      return branch<PTRTrapezoidOsc::ptrRamp3>(slope, pw, y, phase, tick) - dc;
    return branch<PTRTrapezoidOsc::ptrRamp2>(slope, pw, y, phase, tick) - dc;
  }

  template<double (*ptrfunc)(double, double)>
  static double branch(double slope, double pw, double y, double phase, double tick)
  {
    if (phase <= double(0.25) / slope)
      return slope * ptrfunc(phase, tick);
    else if (phase <= double(0.5) / slope)
      return y - slope * ptrfunc(double(0.5) / slope - phase, tick);
    else if (phase <= double(0.5) / slope + pw)
      return y;
    else if (phase <= double(0.75) / slope + pw)
      return y - slope * ptrfunc(phase - double(0.5) / slope - pw, tick);
    else if (phase <= double(1) / slope + pw)
      return slope * ptrfunc(double(1) / slope + pw + -phase, tick);
    return double(0);
  }

  static double ptrRamp0(double phi, double T)
  {
    double n = phi / T;
    if (n >= double(-1)) return double(2) * T * n - double(1);
    return 0.0; // Just in case.
  }

  static double ptrRamp1(double phi, double T)
  {
    double n = phi / T;
    if (n >= double(0)) return double(2) * T * n - T;
    if (n < double(1)) return n * n + T;
    return 0.0; // Just in case.
  }

  static double ptrRamp2(double phi, double T)
  {
    double n = phi / T;
    if (n >= double(1)) return double(2) * T * n - double(2) * T;
    if (n < double(1)) return (T * n * n * n) / double(3);
    if (n < double(2))
      return -(T * n * n * n) / double(3) + double(2) * T * n * n - double(2) * T * n
        + (double(2) * T) / double(3);
    return 0.0; // Just in case.
  }

  static double ptrRamp3(double phi, double T)
  {
    double n = phi / T;
    if (n >= double(2)) return double(2) * T * n - double(3) * T;
    if (n < double(1)) return (T * n * n * n * n) / double(12);
    if (n < double(2))
      return -(T * n * n * n * n) / double(6) + T * n * n * n
        - (double(3) * T * n * n) / double(2) + T * n - T / double(4);
    if (n < double(3))
      return (T * n * n * n * n) / double(12) - T * n * n * n
        + (double(9) * T * n * n) / double(2) - double(7) * T * n
        + (double(15) * T) / double(4);
    return 0.0; // Just in case.
  }

  static double ptrRamp4(double phi, double T)
  {
    double n = phi / T;
    if (n >= double(3)) return double(2) * T * n - double(4) * T;
    if (n < double(1)) return (T * n * n * n * n * n) / double(60);
    if (n < double(2))
      return -(T * n * n * n * n * n) / double(20) + (T * n * n * n * n) / double(3)
        - (double(2) * T * n * n * n) / double(3) + (double(2) * T * n * n) / double(3)
        - (T * n) / double(3) + T / double(15);
    if (n < double(3))
      return (T * n * n * n * n * n) / double(20)
        - (double(2) * T * n * n * n * n) / double(3)
        + (double(10) * T * n * n * n) / double(3) - (double(22) * T * n * n) / double(3)
        + (double(23) * T * n) / double(3) - (double(47) * T) / double(15);
    if (n < double(4))
      return -(T * n * n * n * n * n) / double(60) + (T * n * n * n * n) / double(3)
        - (double(8) * T * n * n * n) / double(3) + (double(32) * T * n * n) / double(3)
        - (double(58) * T * n) / double(3) + (double(196) * T) / double(15);
    return 0.0; // Just in case.
  }

  static double ptrRamp5(double phi, double T)
  {
    double n = phi / T;
    if (n >= double(4)) return double(2) * T * n - double(5) * T;
    if (n < double(1)) return (T * n * n * n * n * n * n) / double(360);
    if (n < double(2))
      return -(T * n * n * n * n * n * n) / double(90)
        + (T * n * n * n * n * n) / double(12)
        - (double(5) * T * n * n * n * n) / double(24)
        + (double(5) * T * n * n * n) / double(18) - (double(5) * T * n * n) / double(24)
        + (T * n) / double(12) - T / double(72);
    if (n < double(3))
      return (T * n * n * n * n * n * n) / double(60)
        - (T * n * n * n * n * n) / double(4)
        + (double(35) * T * n * n * n * n) / double(24)
        - (double(25) * T * n * n * n) / double(6)
        + (double(155) * T * n * n) / double(24) - (double(21) * T * n) / double(4)
        + (double(127) * T) / double(72);
    if (n < double(4))
      return -(T * n * n * n * n * n * n) / double(90)
        + (T * n * n * n * n * n) / double(4)
        - (double(55) * T * n * n * n * n) / double(24)
        + (double(65) * T * n * n * n) / double(6)
        - (double(655) * T * n * n) / double(24) + (double(141) * T * n) / double(4)
        - (double(1331) * T) / double(72);
    if (n < double(5))
      return (T * n * n * n * n * n * n) / double(360)
        - (T * n * n * n * n * n) / double(12)
        + (double(25) * T * n * n * n * n) / double(24)
        - (double(125) * T * n * n * n) / double(18)
        + (double(625) * T * n * n) / double(24) - (double(601) * T * n) / double(12)
        + (double(2765) * T) / double(72);
    return 0.0; // Just in case.
  }
};

} // namespace SomeDSP
