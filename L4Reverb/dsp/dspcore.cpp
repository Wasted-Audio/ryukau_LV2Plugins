// (c) 2019-2020 Takamitsu Endo
//
// This file is part of L4Reverb.
//
// L4Reverb is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// L4Reverb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with L4Reverb.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

#include <iostream>

#if INSTRSET >= 10
  #define DSPCORE_NAME DSPCore_AVX512
#elif INSTRSET >= 8
  #define DSPCORE_NAME DSPCore_AVX2
#elif INSTRSET >= 5
  #define DSPCORE_NAME DSPCore_SSE41
#elif INSTRSET == 2
  #define DSPCORE_NAME DSPCore_SSE2
#else
  #error Unsupported instruction set
#endif

void DSPCORE_NAME::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.2f);

  for (auto &dly : delay) dly.setup(sampleRate, Scales::time.getMax());

  reset();
}

inline std::array<float, 2> calcOffset(float offset, float mul)
{
  if (offset >= 0) return {mul, (1.0f + offset) * mul};
  return {(1.0f + offset) * mul, mul};
}

#define ASSIGN_ALLPASS_PARAMETER(METHOD)                                                 \
  auto timeMul = param.value[ID::timeMultiply]->getFloat();                              \
  auto innerMul = param.value[ID::innerFeedMultiply]->getFloat();                        \
  auto d1FeedMul = param.value[ID::d1FeedMultiply]->getFloat();                          \
  auto d2FeedMul = param.value[ID::d2FeedMultiply]->getFloat();                          \
  auto d3FeedMul = param.value[ID::d3FeedMultiply]->getFloat();                          \
  auto d4FeedMul = param.value[ID::d4FeedMultiply]->getFloat();                          \
  uint16_t i1 = 0;                                                                       \
  uint16_t i2 = 0;                                                                       \
  uint16_t i3 = 0;                                                                       \
  uint16_t i4 = 0;                                                                       \
  auto &ap4L = delay[0];                                                                 \
  auto &ap4R = delay[1];                                                                 \
  for (uint8_t d4 = 0; d4 < nDepth; ++d4) {                                              \
    auto &ap3L = ap4L.allpass[d4];                                                       \
    auto &ap3R = ap4R.allpass[d4];                                                       \
    for (uint8_t d3 = 0; d3 < nDepth; ++d3) {                                            \
      auto &ap2L = ap3L.allpass[d3];                                                     \
      auto &ap2R = ap3R.allpass[d3];                                                     \
      for (uint8_t d2 = 0; d2 < nDepth; ++d2) {                                          \
        auto &ap1L = ap2L.allpass[d2];                                                   \
        auto &ap1R = ap2R.allpass[d2];                                                   \
        for (uint8_t d1 = 0; d1 < nDepth; ++d1) {                                        \
          auto offsetD1Time = calcOffset(offsetDist(rng), timeMul);                      \
          /* auto offsetInnerFeed = calcOffset(offsetDist(rng), innerMul); */            \
          /* auto offsetD1Feed = calcOffset(offsetDist(rng), d1FeedMul); */              \
                                                                                         \
          ap1L.seconds[d1].METHOD(                                                       \
            param.value[ID::time0 + i1]->getFloat() * offsetD1Time[0]);                  \
          ap1L.innerFeed[d1].METHOD(param.value[ID::innerFeed0 + i1]->getFloat());       \
          ap1L.outerFeed[d1].METHOD(param.value[ID::d1Feed0 + i1]->getFloat());          \
                                                                                         \
          ap1R.seconds[d1].METHOD(                                                       \
            param.value[ID::time0 + i1]->getFloat() * offsetD1Time[1]);                  \
          ap1R.innerFeed[d1].METHOD(param.value[ID::innerFeed0 + i1]->getFloat());       \
          ap1R.outerFeed[d1].METHOD(param.value[ID::d1Feed0 + i1]->getFloat());          \
                                                                                         \
          ++i1;                                                                          \
        }                                                                                \
                                                                                         \
        /* auto offsetD2Feed = calcOffset(offsetDist(rng), d2FeedMul); */                \
                                                                                         \
        ap2L.feed[d2].METHOD(param.value[ID::d2Feed0 + i2]->getFloat());                 \
        ap2R.feed[d2].METHOD(param.value[ID::d2Feed0 + i2]->getFloat());                 \
        ++i2;                                                                            \
      }                                                                                  \
                                                                                         \
      /* auto offsetD3Feed = calcOffset(offsetDist(rng), d3FeedMul); */                  \
                                                                                         \
      ap3L.feed[d3].METHOD(param.value[ID::d3Feed0 + i3]->getFloat());                   \
      ap3R.feed[d3].METHOD(param.value[ID::d3Feed0 + i3]->getFloat());                   \
      ++i3;                                                                              \
    }                                                                                    \
                                                                                         \
    /* auto offsetD4Feed = calcOffset(offsetDist(rng), d4FeedMul); */                    \
                                                                                         \
    ap4L.feed[d4].METHOD(param.value[ID::d4Feed0 + i4]->getFloat());                     \
    ap4R.feed[d4].METHOD(param.value[ID::d4Feed0 + i4]->getFloat());                     \
    ++i4;                                                                                \
  }

void DSPCORE_NAME::reset()
{
  using ID = ParameterID::ID;

  for (auto &dly : delay) dly.reset();

  std::minstd_rand rng{0};
  std::uniform_real_distribution<float> offsetDist(-0.05f, 0.05f);

  ASSIGN_ALLPASS_PARAMETER(reset);

  interpStereoSpread.reset(param.value[ID::stereoSpread]->getFloat());
  interpDry.reset(param.value[ID::dry]->getFloat());
  interpWet.reset(param.value[ID::wet]->getFloat());
}

void DSPCORE_NAME::startup() { rng.seed(0); }

void DSPCORE_NAME::setParameters(float tempo)
{
  using ID = ParameterID::ID;

  SmootherCommon<float>::setTime(param.value[ID::smoothness]->getFloat());

  std::minstd_rand rng{0};
  std::uniform_real_distribution<float> offsetDist(-0.05f, 0.05f);

  ASSIGN_ALLPASS_PARAMETER(push);

  interpStereoSpread.push(param.value[ID::stereoSpread]->getFloat());
  interpDry.push(param.value[ID::dry]->getFloat());
  interpWet.push(param.value[ID::wet]->getFloat());
}

void DSPCORE_NAME::process(
  const size_t length, const float *in0, const float *in1, float *out0, float *out1)
{
  SmootherCommon<float>::setBufferSize(length);

  for (size_t i = 0; i < length; ++i) {
    float delayOutL = delay[0].process(in0[i], sampleRate);
    float delayOutR = delay[1].process(in1[i], sampleRate);
    const auto mid = delayOutL + delayOutR;
    const auto side = delayOutL - delayOutR;

    const auto spread = interpStereoSpread.process();
    delayOutL = mid - spread * (mid - side);
    delayOutR = mid - spread * (mid + side);

    const auto dry = interpDry.process();
    const auto wet = interpWet.process();
    out0[i] = dry * in0[i] + wet * delayOutL;
    out1[i] = dry * in1[i] + wet * delayOutR;
  }
}
