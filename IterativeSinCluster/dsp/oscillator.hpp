// (c) 2019 Takamitsu Endo
//
// This file is part of IterativeSinCluster.
//
// IterativeSinCluster is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// IterativeSinCluster is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with IterativeSinCluster.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <array>
#include <iostream>

#include "constants.hpp"
#include "somemath.hpp"

#include "../../lib/vcl/vectorclass.h"
#include "../../lib/vcl/vectormath_trig.h"

namespace SomeDSP {
namespace BiquadOsc {

constexpr size_t biquadOscSize = 8;

struct alignas(64) Data16x8 {
  std::array<Vec16f, biquadOscSize> frequency;
  std::array<Vec16f, biquadOscSize> gain;
  std::array<Vec16f, biquadOscSize> u1;
  std::array<Vec16f, biquadOscSize> u0;
  std::array<Vec16f, biquadOscSize> k;
  void (*setup)(Data16x8 &, float) = nullptr;
  float (*process)(Data16x8 &) = nullptr;
};

extern void setup_AVX512(Data16x8 &data, float sampleRate);
extern void setup_AVX2(Data16x8 &data, float sampleRate);
extern void setup_SSE41(Data16x8 &data, float sampleRate);
extern void setup_SSE2(Data16x8 &data, float sampleRate);
extern float process_AVX512(Data16x8 &data);
extern float process_AVX2(Data16x8 &data);
extern float process_SSE41(Data16x8 &data);
extern float process_SSE2(Data16x8 &data);

extern void initMethod(Data16x8 &data);

} // namespace BiquadOsc
} // namespace SomeDSP
