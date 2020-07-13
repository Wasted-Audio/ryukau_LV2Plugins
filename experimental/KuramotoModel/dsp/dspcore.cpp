// (c) 2020 Takamitsu Endo
//
// This file is part of KuramotoModel.
//
// KuramotoModel is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KuramotoModel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KuramotoModel.  If not, see <https://www.gnu.org/licenses/>.

#include "dspcore.hpp"

#if INSTRSET >= 10
  #define NOTE_NAME Note_AVX512
  #define DSPCORE_NAME DSPCore_AVX512
#elif INSTRSET >= 8
  #define NOTE_NAME Note_AVX2
  #define DSPCORE_NAME DSPCore_AVX2
#elif INSTRSET >= 5
  #define NOTE_NAME Note_SSE41
  #define DSPCORE_NAME DSPCore_SSE41
#elif INSTRSET >= 2
  #define NOTE_NAME Note_SSE2
  #define DSPCORE_NAME DSPCore_SSE2
#else
  #error Unsupported instruction set
#endif

inline float calcMasterPitch(
  int32_t octave, int32_t semi, int32_t milli, float bend, float equalTemperament)
{
  return equalTemperament * octave + semi + milli / 1000.0f + (bend - 0.5f) * 4.0f;
}

inline float
notePitchToFrequency(float notePitch, float equalTemperament = 12.0f, float a4Hz = 440.0f)
{
  return a4Hz * powf(2.0f, (notePitch - 69.0f) / equalTemperament);
}

void DSPCORE_NAME::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  SmootherCommon<float>::setSampleRate(sampleRate);
  SmootherCommon<float>::setTime(0.01f);

  noteStack.reserve(128);
  noteStack.resize(0);

  // 10 msec + 1 sample transition time.
  transitionBuffer.resize(1 + size_t(sampleRate * 0.01), 0.0f);

  reset();
  prepareRefresh = true;
}

void DSPCORE_NAME::reset()
{
  noteStack.resize(0);

  using ID = ParameterID::ID;
  auto &pv = param.value;

  smoothMasterGain.reset(pv[ID::boost]->getFloat());
}

void DSPCORE_NAME::startup() {}

void DSPCORE_NAME::setParameters(float tempo)
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  smoothMasterGain.push(pv[ID::boost]->getFloat());

  if (prepareRefresh || (!isTableRefeshed && param.value[ID::refreshTable]->getInt()))
    refreshTable();
  isTableRefeshed = param.value[ID::refreshTable]->getInt();

  prepareRefresh = false;
}

void DSPCORE_NAME::process(const size_t length, float *out0)
{
  SmootherCommon<float>::setBufferSize(length);

  float frame;
  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);

    frame = 0;

    if (isTransitioning) {
      frame += transitionBuffer[trIndex];
      transitionBuffer[trIndex] = 0.0f;
      trIndex = (trIndex + 1) % transitionBuffer.size();
      if (trIndex == trStop) isTransitioning = false;
    }

    frame += velocity * smoothMasterGain.process() * gate.process()
      * oscillator.process(sampleRate, wavetable.table);

    out0[i] = frame;
  }
}

void DSPCORE_NAME::noteOn(int32_t noteId, int16_t pitch, float tuning, float velocity)
{
  using ID = ParameterID::ID;
  auto &pv = param.value;

  NoteInfo info;
  info.id = noteId;
  noteStack.push_back(info);

  fillTransitionBuffer();

  this->velocity = velocityMap.map(velocity);

  gate.reset(sampleRate, pv[ID::attack]->getFloat());

  float decayMul = pv[ID::decayMultiply]->getFloat();

  float eqTemp = pv[ID::equalTemperament]->getFloat() + 1;
  float a4Hz = pv[ID::pitchA4Hz]->getFloat() + 100;
  float notePitch = calcMasterPitch(
    int32_t(pv[ID::octave]->getInt()) - 12, pv[ID::semitone]->getInt() - 120,
    pv[ID::milli]->getInt() - 1000, pv[ID::pitchBend]->getFloat(), eqTemp);
  notePitch += pitch + tuning;

  for (int idx = 0; idx < 16; ++idx) {
    float oscPitch = pv[ID::pitch0 + idx]->getFloat();
    float oscFreq = notePitchToFrequency(fabsf(oscPitch) + notePitch, eqTemp, a4Hz);
    if (oscFreq > 20000) oscFreq = 0;
    oscillator.frequency[idx] = copysignf(1.0f, oscPitch) * oscFreq / sampleRate;
    oscillator.decay[idx]
      = powf(1e-5, 1.0f / (sampleRate * decayMul * pv[ID::decay0 + idx]->getFloat()));
    oscillator.coupling[idx] = pv[ID::coupling0 + idx]->getFloat();
    oscillator.couplingDecay[idx] = pv[ID::couplingDecay0 + idx]->getFloat();
    oscillator.gain[idx] = pv[ID::gain0 + idx]->getFloat();
  }
  oscillator.trigger();
}

void DSPCORE_NAME::noteOff(int32_t noteId)
{
  auto it = std::find_if(noteStack.begin(), noteStack.end(), [&](const NoteInfo &info) {
    return info.id == noteId;
  });
  if (it == noteStack.end()) return;
  noteStack.erase(it);
}

void DSPCORE_NAME::fillTransitionBuffer()
{
  isTransitioning = true;

  // Beware the negative overflow. trStop is size_t.
  trStop = trIndex - 1;
  if (trStop >= transitionBuffer.size()) trStop += transitionBuffer.size();

  float gain = velocity * smoothMasterGain.getValue();
  for (size_t bufIdx = 0; bufIdx < transitionBuffer.size(); ++bufIdx) {
    auto oscOut = gain * gate.process() * oscillator.process(sampleRate, wavetable.table);
    auto idx = (trIndex + bufIdx) % transitionBuffer.size();
    auto interp = 1.0f - float(bufIdx) / transitionBuffer.size();
    transitionBuffer[idx] += oscOut * interp;
  }
}

void DSPCORE_NAME::refreshTable()
{
  using ID = ParameterID::ID;

  reset();

  std::vector<float> table(nWaveform);
  for (size_t idx = 0; idx < nWaveform; ++idx)
    table[idx] = param.value[ID::waveform0 + idx]->getFloat();

  wavetable.refreshTable(table, 1); // TODO: Add interpolation type parameter.
}
