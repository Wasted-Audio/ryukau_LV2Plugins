// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of CV_PTRSaw.
//
// CV_PTRSaw is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_PTRSaw is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_PTRSaw.  If not, see <https://www.gnu.org/licenses/>.

#include <utility>

#include "DistrhoPlugin.hpp"
#include "dsp/dspcore.hpp"

START_NAMESPACE_DISTRHO

class CV_PTRSaw : public Plugin {
public:
  // Plugin(nParameters, nPrograms, nStates).
  CV_PTRSaw() : Plugin(ParameterID::ID_ENUM_LENGTH, 0, 0)
  {
    sampleRateChanged(getSampleRate());
    lastNoteId.reserve(dsp.maxVoice + 1);
    alreadyRecievedNote.reserve(dsp.maxVoice);
  }

protected:
  /* Information */
  const char *getLabel() const override { return "CV_PTRSaw"; }
  const char *getDescription() const override
  {
    return "Monophonic order 10 PTR sawtooth oscillator. Internally using double "
           "precision.";
  }
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
  int64_t getUniqueId() const override { return d_cconst('u', '0', '0', '0'); }

  void initAudioPort(bool input, uint32_t index, AudioPort &port)
  {
    if (input) {
      switch (index) {
        case 0:
          port.hints = kAudioPortIsCV;
          port.name = String("Gain");
          port.symbol = String("cv_gain");
          return;
        case 1:
          port.hints = kAudioPortIsCV;
          port.name = String("OscPitch");
          port.symbol = String("cv_osc_pitch");
          return;
        case 2:
          port.hints = kAudioPortIsCV;
          port.name = String("OscMod");
          port.symbol = String("cv_osc_mod");
          return;
        case 3:
          port.hints = kAudioPortIsCV;
          port.name = String("SyncRatio");
          port.symbol = String("cv_sync_ratio");
          return;
        case 4:
          port.hints = kAudioPortIsCV;
          port.name = String("SyncMod");
          port.symbol = String("cv_sync_mod");
          return;
      }
    } else if (index == 0) {
      port.hints = kAudioPortIsCV;
      port.name = String("Output");
      port.symbol = String("cv_out");
      return;
    }
    Plugin::initAudioPort(input, index, port);
  }

  void initParameter(uint32_t index, Parameter &parameter) override
  {
    dsp.param.initParameter(index, parameter);
    parameter.symbol = parameter.name;
  }

  float getParameterValue(uint32_t index) const override
  {
    return dsp.param.getFloat(index);
  }

  void setParameterValue(uint32_t index, float value) override
  {
    dsp.param.setParameterValue(index, value);
  }

  void initProgramName(uint32_t index, String &programName) override
  {
    dsp.param.initProgramName(index, programName);
  }

  void loadProgram(uint32_t index) override { dsp.param.loadProgram(index); }

  void sampleRateChanged(double newSampleRate) { dsp.setup(newSampleRate); }
  void activate() { dsp.startup(); }
  void deactivate() { dsp.reset(); }

  void handleMidi(const MidiEvent ev)
  {
    if (ev.size != 3) return;

    // Copied from nekobi code.
    switch (ev.data[0] & 0xf0) {
      // Note off.
      case 0x80: {
        auto it = std::find_if(
          lastNoteId.begin(), lastNoteId.end(),
          [&](const std::pair<uint8_t, uint32_t> &p) { return p.first == ev.data[1]; });
        if (it == std::end(lastNoteId)) break;
        dsp.pushMidiNote(false, ev.frame, it->second, 0, 0, 0);
        lastNoteId.erase(it);
      } break;

      // Note on. data[1]: note number, data[2] velocity.
      case 0x90:
        if (ev.data[2] > 0) {
          auto it = std::find_if(
            alreadyRecievedNote.begin(), alreadyRecievedNote.end(),
            [&](const uint8_t &noteNo) { return noteNo == ev.data[1]; });
          if (it != std::end(alreadyRecievedNote)) break;
          dsp.pushMidiNote(
            true, ev.frame, noteId, ev.data[1], 0.0f, ev.data[2] / float(INT8_MAX));
          lastNoteId.push_back(std::pair<uint8_t, uint32_t>(ev.data[1], noteId));
          alreadyRecievedNote.push_back(ev.data[1]);
          noteId += 1;
        }
        break;

      // Pitch bend. Center is 8192 (0x2000).
      case 0xe0:
        dsp.param.value[ParameterID::pitchBend]->setFromFloat(
          ((uint16_t(ev.data[2]) << 7) + ev.data[1]) / 16384.0f);
        break;

      default:
        break;
    }
  }

  void run(
    const float **inputs,
    float **outputs,
    uint32_t frames,
    const MidiEvent *midiEvents,
    uint32_t midiEventCount) override
  {
    if (inputs == nullptr || outputs == nullptr) return;

    const auto timePos = getTimePosition();
    if (!wasPlaying && timePos.playing) dsp.startup();
    wasPlaying = timePos.playing;

    for (size_t i = 0; i < midiEventCount; ++i) handleMidi(midiEvents[i]);
    alreadyRecievedNote.resize(0);

    dsp.setParameters();
    dsp.process(
      frames, inputs[0], inputs[1], inputs[2], inputs[3], inputs[4], outputs[0]);
  }

private:
  DSPCore dsp;
  bool wasPlaying = false;
  uint32_t noteId = 0;
  std::vector<std::pair<uint8_t, uint32_t>> lastNoteId;
  std::vector<uint8_t> alreadyRecievedNote;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_PTRSaw)
};

Plugin *createPlugin() { return new CV_PTRSaw(); }

END_NAMESPACE_DISTRHO
