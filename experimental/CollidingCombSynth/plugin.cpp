// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of CollidingCombSynth.
//
// CollidingCombSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CollidingCombSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CollidingCombSynth.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>
#include <memory>
#include <utility>

#include "DistrhoPlugin.hpp"
#include "dsp/dspcore.hpp"

START_NAMESPACE_DISTRHO

class CollidingCombSynth : public Plugin {
public:
  CollidingCombSynth() : Plugin(ParameterID::ID_ENUM_LENGTH, 0, 0)
  {
    auto iset = instrset_detect();
    if (iset >= 10) {
      dsp = std::make_unique<DSPCore_AVX512>();
    } else if (iset >= 8) {
      dsp = std::make_unique<DSPCore_AVX2>();
    } else if (iset >= 5) {
      dsp = std::make_unique<DSPCore_SSE41>();
    } else if (iset >= 2) {
      dsp = std::make_unique<DSPCore_SSE2>();
    } else {
      std::cerr << "\nError: Instruction set SSE2 not supported on this computer";
      exit(EXIT_FAILURE);
    }
    dsp->param.validate();

    sampleRateChanged(getSampleRate());
    lastNoteId.reserve(dsp->maxVoice + 1);
    alreadyRecievedNote.reserve(dsp->maxVoice);
  }

protected:
  const char *getLabel() const override { return "CollidingCombSynth"; }
  const char *getDescription() const override { return "Bubble synthesizer."; }
  const char *getMaker() const override { return "Uhhyou"; }
  const char *getHomePage() const override
  {
    return "https://github.com/ryukau/LV2Plugins";
  }
  const char *getLicense() const override { return "GPLv3"; }
  uint32_t getVersion() const override
  {
    return d_version(MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
  }
  int64_t getUniqueId() const override { return d_cconst('u', 'b', 'u', 'b'); }

  void initParameter(uint32_t index, Parameter &parameter) override
  {
    dsp->param.initParameter(index, parameter);
    parameter.symbol = parameter.name;
  }

  float getParameterValue(uint32_t index) const override
  {
    return dsp->param.getFloat(index);
  }

  void setParameterValue(uint32_t index, float value) override
  {
    dsp->param.setParameterValue(index, value);
  }

  void initProgramName(uint32_t index, String &programName) override
  {
    dsp->param.initProgramName(index, programName);
  }

  void loadProgram(uint32_t index) override { dsp->param.loadProgram(index); }

  void sampleRateChanged(double newSampleRate) { dsp->setup(newSampleRate); }
  void activate() {}
  void deactivate() { dsp->reset(); }

  void handleMidi(const MidiEvent ev)
  {
    if (ev.size != 3) return;

    switch (ev.data[0] & 0xf0) {
      // Note off.
      case 0x80: {
        auto it = std::find_if(
          lastNoteId.begin(), lastNoteId.end(),
          [&](const std::pair<uint8_t, uint32_t> &p) { return p.first == ev.data[1]; });
        if (it == std::end(lastNoteId)) break;
        dsp->pushMidiNote(false, ev.frame, it->second, 0, 0, 0);
        lastNoteId.erase(it);
      } break;

      // Note on. data[1]: note number, data[2] velocity.
      case 0x90:
        if (ev.data[2] > 0) {
          auto it = std::find_if(
            alreadyRecievedNote.begin(), alreadyRecievedNote.end(),
            [&](const uint8_t &noteNo) { return noteNo == ev.data[1]; });
          if (it != std::end(alreadyRecievedNote)) break;
          dsp->pushMidiNote(
            true, ev.frame, noteId, ev.data[1], 0.0f, ev.data[2] / float(INT8_MAX));
          lastNoteId.push_back(std::pair<uint8_t, uint32_t>(ev.data[1], noteId));
          alreadyRecievedNote.push_back(ev.data[1]);
          noteId += 1;
        }
        break;

      // Pitch bend. Center is 8192 (0x2000).
      case 0xe0:
        dsp->param.value[ParameterID::pitchBend]->setFromFloat(
          ((uint16_t(ev.data[2]) << 7) + ev.data[1]) / 16384.0f);
        break;

      default:
        break;
    }
  }

  void run(
    const float **,
    float **outputs,
    uint32_t frames,
    const MidiEvent *midiEvents,
    uint32_t midiEventCount) override
  {
    if (outputs == nullptr) return;
    if (dsp->param.value[ParameterID::bypass]->getInt()) return;

    const auto timePos = getTimePosition();
    if (!wasPlaying && timePos.playing) dsp->startup();
    wasPlaying = timePos.playing;

    for (size_t i = 0; i < midiEventCount; ++i) handleMidi(midiEvents[i]);
    alreadyRecievedNote.resize(0);

    dsp->setParameters(timePos.bbt.beatsPerMinute);
    dsp->process(frames, outputs[0], outputs[1]);
  }

private:
  std::unique_ptr<DSPInterface> dsp;
  bool wasPlaying = false;
  uint32_t noteId = 0;
  std::vector<std::pair<uint8_t, uint32_t>> lastNoteId;
  std::vector<uint8_t> alreadyRecievedNote;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CollidingCombSynth)
};

Plugin *createPlugin() { return new CollidingCombSynth(); }

END_NAMESPACE_DISTRHO
