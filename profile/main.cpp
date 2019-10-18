#include <sndfile.h>
#include <string.h>

#include <iostream>
#include <vector>

#include "../SyncSawSynth/dsp/dspcore.hpp"

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
  if (sf_write_float(file, &buffer[0], length) != length)
    std::cout << sf_strerror(file) << std::endl;

  sf_close(file);

  return 0;
}

constexpr size_t BUF_LEN = 512;
constexpr size_t N_LOOP = 127;
constexpr float sampleRate = 44100.0f;

int main()
{
  std::vector<float> wav;
  wav.resize(BUF_LEN * N_LOOP, 0.0f);
  size_t wavIndex = 0;

  float sig[2][BUF_LEN];

  DSPCore dsp;
  dsp.setup(sampleRate);

  dsp.param.value[ParameterID::gainR]->setFromNormalized(1.0);
  dsp.param.value[ParameterID::filterR]->setFromNormalized(1.0);

  dsp.setParameters();

  for (int32_t n = 0; n < dsp.maxPoly(); ++n) dsp.noteOn(n, 48 + n, 0, 0.5);
  dsp.setParameters();
  dsp.process(BUF_LEN, sig[0], sig[1]);

  for (size_t i = 0; i < N_LOOP; ++i) {
    if (i == 20) // 20 * BUF_LEN / sampleRate [seconds].
      for (int32_t n = 0; n < dsp.maxPoly(); ++n) dsp.noteOff(n);
    dsp.setParameters();
    dsp.process(BUF_LEN, sig[0], sig[1]);
    for (size_t j = 0; j < BUF_LEN; ++j) {
      wav[wavIndex] = sig[0][j];
      wavIndex += 1;
    }
  }

  std::cout << std::to_string(sig[0][0]) << "\n";

  writeWave("test.wav", wav, sampleRate);
}
