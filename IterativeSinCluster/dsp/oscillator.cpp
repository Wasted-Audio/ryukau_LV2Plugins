#include "oscillator.hpp"

#if INSTRSET >= 10 // AVX512VL
#define SETUPNAME setup_AVX512
#define PROCESSNAME process_AVX512
#elif INSTRSET >= 8 // AVX2
#define SETUPNAME setup_AVX2
#define PROCESSNAME process_AVX2
#elif INSTRSET >= 5 // SSE4.1
#define SETUPNAME setup_SSE41
#define PROCESSNAME process_SSE41
#elif INSTRSET == 2 // SSE2
#define SETUPNAME setup_SSE2
#define PROCESSNAME process_SSE2
#else
#error Unsupported instruction set
#endif

namespace SomeDSP {
namespace BiquadOsc {

void SETUPNAME(Data16x8 &data, float sampleRate)
{
  for (size_t i = 0; i < biquadOscSize; ++i) {
    data.u1[i] = 0;
    auto omega = float(twopi) * data.frequency[i] / sampleRate;
    data.u0[i] = -sincos(&data.k[i], omega);
    data.k[i] *= 2.0f;
  }
}

float PROCESSNAME(Data16x8 &data)
{
  float sum = 0;
  for (size_t i = 0; i < biquadOscSize; ++i) {
    auto out = data.k[i] * data.u1[i] - data.u0[i];
    data.u0[i] = data.u1[i];
    data.u1[i] = out;
    sum += horizontal_add(data.gain[i] * out);
  }
  return sum / float(8 * biquadOscSize);
}

#if INSTRSET == 2
void initMethod(Data16x8 &data)
{
  auto iset = instrset_detect();
  if (iset >= 10) { // AVX512 version
    data.setup = &setup_AVX512;
    data.process = &process_AVX512;
  } else if (iset >= 8) { // AVX2 version
    data.setup = &setup_AVX2;
    data.process = &process_AVX2;
  } else if (iset >= 5) { // SSE4.1 version
    data.setup = &setup_SSE41;
    data.process = &process_SSE41;
  } else if (iset >= 2) { // SSE2 version
    data.setup = &setup_SSE2;
    data.process = &process_SSE2;
  } else { // Error: lowest instruction set not supported
    std::cerr << "\nError: Instruction set SSE2 not supported on this computer";
    exit(EXIT_FAILURE);
  }
}
#endif

} // namespace BiquadOsc
} // namespace SomeDSP
