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

#include "../parameter.hpp"
#include "constants.hpp"
#include "delay.hpp"
#include "envelope.hpp"
#include "iir.hpp"
#include "noise.hpp"
#include "oscillator.hpp"
#include "smoother.hpp"

#include <array>
#include <cmath>
#include <memory>

using namespace SomeDSP;

constexpr size_t nPitch = 8;
constexpr size_t nChord = 4;
constexpr size_t nOvertone = 16;
constexpr size_t biquadOscSize = nPitch * nOvertone;

enum class NoteState { active, release, rest };

class Note {
public:
  NoteState state = NoteState::rest;

  float sampleRate = 44100;

  int32_t id = -1;
  float normalizedKey = 0;
  float velocity = 0;
  float gain = 0;
  float frequency = 0;

  std::array<BiquadOsc::Data16x8, nChord> oscillator;
  std::array<float, nChord> chordPan{};

  ExpADSREnvelope<float> gainEnvelope;
  float gainEnvCurve = 0;

  Note();
  void setup(float sampleRate);
  void release();
  void rest();
  void (*noteOn)(Note &, int32_t, float, float, float, GlobalParameter &, White<float> &);
  std::array<float, 2> process();
};

namespace NoteMethod {
extern void noteOn_AVX512(
  Note &note,
  int32_t noteId,
  float normalizedKey,
  float frequency,
  float velocity,
  GlobalParameter &param,
  White<float> &rng);
extern void noteOn_AVX2(
  Note &note,
  int32_t noteId,
  float normalizedKey,
  float frequency,
  float velocity,
  GlobalParameter &param,
  White<float> &rng);
extern void noteOn_SSE41(
  Note &note,
  int32_t noteId,
  float normalizedKey,
  float frequency,
  float velocity,
  GlobalParameter &param,
  White<float> &rng);
extern void noteOn_SSE2(
  Note &note,
  int32_t noteId,
  float normalizedKey,
  float frequency,
  float velocity,
  GlobalParameter &param,
  White<float> &rng);
} // namespace NoteMethod
