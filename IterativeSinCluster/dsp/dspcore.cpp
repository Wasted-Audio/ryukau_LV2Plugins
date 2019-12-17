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

#include "dspcore.hpp"

inline float clamp(float value, float min, float max)
{
  return (value < min) ? min : (value > max) ? max : value;
}

inline float midiNoteToFrequency(float pitch, float tuning)
{
  return 440.0f * powf(2.0f, ((pitch - 69.0f) * 100.0f + tuning) / 1200.0f);
}

void DSPCore::setup(double sampleRate)
{
  this->sampleRate = sampleRate;

  LinearSmoother<float>::setSampleRate(sampleRate);
  LinearSmoother<float>::setTime(0.04f);

  for (auto &note : notes) note.setup(sampleRate);

  for (auto &chrs : chorus)
    chrs.setup(
      sampleRate, 0,
      Scales::chorusDelayTimeRange.getMax() + Scales::chorusMinDelayTime.getMax());

  // 2 msec + 1 sample transition time.
  transitionBuffer.resize(1 + size_t(sampleRate * 0.005), {0, 0});

  startup();
}

void DSPCore::free() {}

void DSPCore::reset()
{
  for (auto &note : notes) note.rest();
  lastNoteFreq = 1.0f;

  for (auto &chrs : chorus) chrs.reset();

  startup();
}

void DSPCore::startup() { rng.seed = param.value[ParameterID::seed]->getInt(); }

void DSPCore::setParameters()
{
  using ID = ParameterID::ID;

  LinearSmoother<float>::setTime(param.value[ID::smoothness]->getFloat());

  interpTremoloMix.push(param.value[ID::chorusMix]->getFloat());
  interpMasterGain.push(
    param.value[ID::gain]->getFloat() * param.value[ID::gainBoost]->getFloat());

  nVoice = 1 << param.value[ID::nVoice]->getInt();
  if (nVoice > notes.size()) nVoice = notes.size();

  for (auto &note : notes) {
    if (note.state == NoteState::rest) continue;
    note.gainEnvelope.set(
      param.value[ID::gainA]->getFloat(), param.value[ID::gainD]->getFloat(),
      param.value[ID::gainS]->getFloat(), param.value[ID::gainR]->getFloat(),
      note.frequency);
  }

  for (size_t i = 0; i < chorus.size(); ++i) {
    chorus[i].setParam(
      param.value[ID::chorusFrequency]->getFloat(),
      param.value[ID::chorusPhase]->getFloat()
        + i * param.value[ID::chorusOffset]->getFloat(),
      param.value[ID::chorusFeedback]->getFloat(),
      param.value[ID::chorusDepth]->getFloat(),
      param.value[ID::chorusDelayTimeRange0 + i]->getFloat(),
      param.value[ID::chorusKeyFollow]->getInt()
        ? 200.0f * param.value[ID::chorusMinDelayTime0 + i]->getFloat() / lastNoteFreq
        : param.value[ID::chorusMinDelayTime0 + i]->getFloat());
  }
}

void DSPCore::process(const size_t length, float *out0, float *out1)
{
  LinearSmoother<float>::setBufferSize(length);

  std::array<float, 2> frame{};
  std::array<float, 2> chorusOut{};
  for (size_t i = 0; i < length; ++i) {
    processMidiNote(i);

    frame.fill(0.0f);

    for (auto &note : notes) {
      if (note.state == NoteState::rest) continue;
      auto noteSig = note.process();
      frame[0] += noteSig[0];
      frame[1] += noteSig[1];
    }

    if (isTransitioning) {
      frame[0] += transitionBuffer[mptIndex][0];
      frame[1] += transitionBuffer[mptIndex][1];
      transitionBuffer[mptIndex].fill(0.0f);
      mptIndex = (mptIndex + 1) % transitionBuffer.size();
      if (mptIndex == mptStop) isTransitioning = false;
    }

    const auto chorusIn = frame[0] + frame[1];
    chorusOut.fill(0.0f);
    for (auto &chrs : chorus) {
      const auto out = chrs.process(chorusIn);
      chorusOut[0] += out[0];
      chorusOut[1] += out[1];
    }
    chorusOut[0] /= chorus.size();
    chorusOut[1] /= chorus.size();

    const auto chorusMix = interpTremoloMix.process();
    const auto masterGain = interpMasterGain.process();
    out0[i] = masterGain * (frame[0] + chorusMix * (chorusOut[0] - frame[0]));
    out1[i] = masterGain * (frame[1] + chorusMix * (chorusOut[1] - frame[1]));
  }
}

void DSPCore::noteOn(int32_t noteId, int16_t pitch, float tuning, float velocity)
{
  size_t noteIdx = 0;
  size_t mostSilent = 0;
  float gain = 1.0f;
  for (; noteIdx < nVoice; ++noteIdx) {
    if (notes[noteIdx].id == noteId) break;
    if (notes[noteIdx].state == NoteState::rest) break;
    if (!notes[noteIdx].gainEnvelope.isAttacking() && notes[noteIdx].gain < gain) {
      gain = notes[noteIdx].gain;
      mostSilent = noteIdx;
    }
  }
  if (noteIdx >= nVoice) {
    isTransitioning = true;

    noteIdx = mostSilent;

    // Beware the negative overflow. mptStop is size_t.
    mptStop = mptIndex - 1;
    if (mptStop >= transitionBuffer.size()) mptStop += transitionBuffer.size();

    for (size_t bufIdx = 0; bufIdx < transitionBuffer.size(); ++bufIdx) {
      if (notes[noteIdx].state == NoteState::rest) {
        mptStop = mptIndex + bufIdx;
        if (mptStop >= transitionBuffer.size()) mptStop -= transitionBuffer.size();
        break;
      }

      auto frame = notes[noteIdx].process();
      auto idx = (mptIndex + bufIdx) % transitionBuffer.size();
      auto interp = 1.0f - float(bufIdx) / transitionBuffer.size();

      transitionBuffer[idx][0] += frame[0] * interp;
      transitionBuffer[idx][1] += frame[1] * interp;
    }
  }

  if (param.value[ParameterID::randomRetrigger]->getInt())
    rng.seed = param.value[ParameterID::seed]->getInt();

  auto normalizedKey = float(pitch) / 127.0f;
  lastNoteFreq = midiNoteToFrequency(pitch, tuning);
  notes[noteIdx].noteOn(
    notes[noteIdx], noteId, normalizedKey, lastNoteFreq, velocity, param, rng);
}

void DSPCore::noteOff(int32_t noteId)
{
  size_t i = 0;
  for (; i < notes.size(); ++i) {
    if (notes[i].id == noteId) break;
  }
  if (i >= notes.size()) return;

  notes[i].release();
}
