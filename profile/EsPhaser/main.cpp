#include <sndfile.h>
#include <string.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include "../../EsPhaser/dsp/dspcore.hpp"

int32_t
writeWave(const char *filename, std::vector<float> &buffer, const size_t &samplerate)
{
  SF_INFO sfinfo;
  memset(&sfinfo, 0, sizeof(sfinfo));
  sfinfo.samplerate = samplerate;
  sfinfo.frames = buffer.size();
  sfinfo.channels = 1;
  sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_FLOAT);

  SNDFILE *file = sf_open(filename, SFM_WRITE, &sfinfo);
  if (!file) {
    std::cout << "Error: sf_open failed." << std::endl;
    return 1;
  }

  size_t length = sfinfo.channels * buffer.size();
  if (sf_write_float(file, &buffer[0], length) != (sf_count_t)length)
    std::cout << sf_strerror(file) << std::endl;

  sf_close(file);

  return 0;
}

constexpr size_t BUF_LEN = 512;
constexpr size_t N_LOOP = 1024;
constexpr float sampleRate = 96000.0f;

struct Saw {
  float phase = 0;
  float tick = 0;

  Saw(float sampleRate, float frequency) { tick = frequency / sampleRate; }

  float process()
  {
    phase += tick;
    if (phase > 1.0f) phase -= 1.0f;
    return 2.0f * phase - 1.0f;
  }
};

void fillSaw(Saw &saw, float *in0, float *in1)
{
  for (size_t j = 0; j < BUF_LEN; ++j) {
    float sig = 0.5f * saw.process();
    in0[j] = sig;
    in1[j] = sig;
  }
}

int main()
{
  std::vector<float> wav;
  wav.resize(BUF_LEN * N_LOOP, 0.0f);
  size_t wavIndex = 0;

  float in[2][BUF_LEN];
  float out[2][BUF_LEN];

  std::unique_ptr<DSPInterface> dsp;

  auto iset = instrset_detect();
  std::cout << "instrset: " << std::to_string(iset) << std::endl;
  if (iset >= 10) { // AVX512
    dsp = std::make_unique<DSPCore_AVX512>();
  } else if (iset >= 8) { // AVX2
    dsp = std::make_unique<DSPCore_AVX2>();
  } else if (iset >= 5) { // SSE4.1
    dsp = std::make_unique<DSPCore_SSE41>();
  } else if (iset >= 2) { // SSE2
    dsp = std::make_unique<DSPCore_SSE2>();
  } else {
    std::cerr << "\nError: Instruction set SSE2 not supported on this computer";
    exit(EXIT_FAILURE);
  }

  Saw saw(sampleRate, 100.0f);

  dsp->setup(sampleRate);

  dsp->param.value[ParameterID::stage]->setFromNormalized(1.0);

  dsp->setParameters(120.0);

  fillSaw(saw, in[0], in[1]);
  dsp->process(BUF_LEN, in[0], in[1], out[0], out[1]);

  double sumElapsed = 0.0;
  for (size_t i = 0; i < N_LOOP; ++i) {
    auto start = std::chrono::high_resolution_clock::now();
    dsp->setParameters(120.0);
    fillSaw(saw, in[0], in[1]);
    dsp->process(BUF_LEN, in[0], in[1], out[0], out[1]);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = finish - start;
    sumElapsed += elapsed.count();

    for (size_t j = 0; j < BUF_LEN; ++j) {
      wav[wavIndex] = out[0][j];
      wavIndex += 1;
    }
  }

  const char *name = "EsPhaser";
  std::cout << name << "\n"
            << "Total[ms]" << std::to_string(sumElapsed) << "\n"
            << "Average[ms]" << std::to_string(sumElapsed / N_LOOP) << "\n\n";

  writeWave("test.wav", wav, sampleRate);
}
