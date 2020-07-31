// Original by:
// DISTRHO Plugin Framework (DPF)
// Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
//
// Modified by:
// (c) 2020 Takamitsu Endo
//
// This file is part of CV_TimeInfo.
//
// CV_TimeInfo is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CV_TimeInfo is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CV_TimeInfo.  If not, see <https://www.gnu.org/licenses/>.

#include "DistrhoPlugin.hpp"

#include <iostream> // debug

START_NAMESPACE_DISTRHO

enum PortIndex : uint8_t {
  Playing = 0,
  BPM,
  TimeSignatureUpper,
  TimeSignatureLower,
  BarTrigger,
  BeatTrigger,

  PORT_INDEX_LENGTH,
};

class CV_TimeInfo : public Plugin {
public:
  // Plugin(nParameters, nPrograms, nStates).
  CV_TimeInfo() : Plugin(0, 0, 0) { sampleRateChanged(getSampleRate()); }

protected:
  /* Information */
  const char *getLabel() const override { return "UhhyouCV_TimeInfo"; }
  const char *getDescription() const override
  {
    return "Provides time information as CV signal.";
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
    if (!input && index == Playing) {
      port.hints = kAudioPortIsCV;
      port.name = String("Playing");
      port.symbol = String("Playing");
    } else if (!input && index == BPM) {
      port.hints = kAudioPortIsCV;
      port.name = String("BPM");
      port.symbol = String("BPM");
    } else if (!input && index == TimeSignatureUpper) {
      port.hints = kAudioPortIsCV;
      port.name = String("TimeSignatureUpper");
      port.symbol = String("TimeSignatureUpper");
    } else if (!input && index == TimeSignatureLower) {
      port.hints = kAudioPortIsCV;
      port.name = String("TimeSignatureLower");
      port.symbol = String("TimeSignatureLower");
    } else if (!input && index == BarTrigger) {
      port.hints = kAudioPortIsCV;
      port.name = String("BarTrigger");
      port.symbol = String("BarTrigger");
    } else if (!input && index == BeatTrigger) {
      port.hints = kAudioPortIsCV;
      port.name = String("BeatTrigger");
      port.symbol = String("BeatTrigger");
    } else {
      Plugin::initAudioPort(input, index, port);
    }
  }

  void initParameter(uint32_t /* index */, Parameter & /*parameter*/) override {}
  float getParameterValue(uint32_t /* index */) const override { return 0; }
  void setParameterValue(uint32_t /* index */, float /* value */) override {}
  void sampleRateChanged(double newSampleRate) { sampleRate = newSampleRate; }
  void activate() {}
  void deactivate() {}

  void run(const float ** /* inputs */, float **outputs, uint32_t frames) override
  {
    if (outputs == nullptr) return;

    const auto timePos = getTimePosition();

    float playing = timePos.playing ? 1.0f : 0.0f;
    for (uint32_t i = 0; i < frames; ++i) outputs[Playing][i] = playing;

    if (timePos.bbt.valid) {
      auto &bbt = timePos.bbt;

      for (uint32_t i = 0; i < frames; ++i) {
        outputs[BPM][i] = float(bbt.beatsPerMinute);
        outputs[TimeSignatureUpper][i] = bbt.beatsPerBar;
        outputs[TimeSignatureLower][i] = bbt.beatType;
      }

      if (timePos.playing) {
        double secondsPerBeat = 60.0 / bbt.beatsPerMinute;
        double framesPerBeat = sampleRate * secondsPerBeat;
        double beatFraction = bbt.tick / bbt.ticksPerBeat;

        beatCounter
          = beatFraction == 0.0 ? 0 : int32_t(framesPerBeat * (1.0 - beatFraction));

        int32_t beatsPerBar = int32_t(bbt.beatsPerBar);
        beatInBar = !wasPlaying ? beatsPerBar : bbt.beat;

        for (uint32_t i = 0; i < frames; ++i) {
          --beatCounter;
          bool beatTrigger = beatCounter <= 0;

          outputs[BarTrigger][i] = beatInBar == beatsPerBar && beatTrigger ? 1 : 0;
          outputs[BeatTrigger][i] = beatTrigger ? 1 : 0;

          if (beatTrigger) {
            beatCounter = int32_t(framesPerBeat);

            ++beatInBar;
            if (beatInBar > beatsPerBar) beatInBar = 1;
          }
        }
      } else {
        for (uint32_t i = 0; i < frames; ++i) {
          outputs[BarTrigger][i] = 0.0;
          outputs[BeatTrigger][i] = 0.0;
        }
      }
    } else {
      for (uint32_t i = 0; i < frames; ++i) {
        outputs[BPM][i] = 0.0;
        outputs[TimeSignatureUpper][i] = 0.0;
        outputs[TimeSignatureLower][i] = 0.0;
        outputs[BarTrigger][i] = 0.0;
        outputs[BeatTrigger][i] = 0.0;
      }
    }

    wasPlaying = timePos.playing;
  }

private:
  double sampleRate = 44100;
  int32_t beatInBar = 0;
  int32_t beatCounter = 0;
  bool wasPlaying = false;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CV_TimeInfo)
};

Plugin *createPlugin() { return new CV_TimeInfo(); }

END_NAMESPACE_DISTRHO
