# LV2 CVPort Plugins
Example patch.

<figure>
<img src="img/lv2_cvport_example1.png" alt="Image of LV2 CVPort example patch on Carla patchbay." style="padding-bottom: 12px;"/>
</figure>

## Quick Setup
First, install `qjackctl`, `jack-keyboard`, `a2jmidid` and `Carla`.

If your environment has multiple sound device, it may requires to configure `qjackctl`. In this case, click `Setup` buttion. Then open `Settings` -> `Advanced` tab. `Output Device` and `Input Device` may not be set to primary device, so fix it.

When configuration is done, click `Start` button on main windows of `qjackctl` to start `jackd`.

If you have MIDI keyboard, use `a2jmidid` to receive notes. To start `a2jmidid`, type following command after started `jackd` via `qjackctl`.

```
a2jmidid -e
```

If you don't have MIDI keyboard, use `jack-keyboard`.

`Carla` also requires some configuration to use LV2 CV port. Click `Configure Carla`, then go to `Engine` tab. Change `Audio driver` to JACK and `Process mode` to Patchbay. After configuration, quit Carla, start `jackd` via `qjackctl`, then restart `Carla`. `Carla` now loads LV2 CV port plugins.

<figure>
<img src="img/lv2_cvport_carla_setting.png" alt="Image of Carla configuration for LV2 CV port." style="padding-bottom: 12px;"/>
</figure>

JACK application have to be manually connected. Click `Connect` button on main window of `qjackctl`, then use drag and drop to connect applications to appropriate port.

<figure>
<img src="img/lv2_cvport_audio_connection.png" alt="Image of qjackctl audio connection example." style="padding-bottom: 12px;"/>
</figure>

<figure>
<img src="img/lv2_cvport_midi_connection.png" alt="Image of qjackctl MIDI connection example." style="padding-bottom: 12px;"/>
</figure>

`qjackctl` can save current connection as a session. Click `Session` buttion on main window of `qjackctl`, then click `Save`.

For more detailed information, take a look at ArchWiki.

- [JACK Audio Connection Kit - ArchWiki](https://wiki.archlinux.org/index.php/JACK_Audio_Connection_Kit)
- [Professional audio - ArchWiki](https://wiki.archlinux.org/index.php/Professional_audio)

## CV_AudioToCv
Convert audio port signal to CV port signal.

- 1 audio input
- 1 CV output.

## CV_CvToAudio
Convert CV port signal to audio port signal.

- 1 CV input
- 1 audio output.

## ExpADSREnvelope
Exponential ADSR Envelope.

- 1 event input
- 1 CV output.

## ExpPolyADEnvelope
Exponential polynomial envelope. Note that this envelope resets to 0 for each note-on.

- 1 event input
- 1 CV output.

Beware that increasing `curve` parameter may lead to very loud output.

## CV_LinearADSREnvelope
Linear ADSR envelope. 1 event input and 1 CV output.

## CV_Multiply
Multiply CV signal. 2 CV inputs and 1 CV output.

## CV_ParabolicADEnvelope
Parabolic envelope. Note that this envelope resets to 0 for each note-on.

- 1 event input
- 1 CV output.

## CV_PTRSaw
Monophonic order 10 PTR sawtooth oscillator. Double precision is used internally.

- 1 event input
- 5 CV inputs
- 1 CV output.

### CV Mapping
Equation to calculate gain value:

```
gain = paramGain * paramBoost + cvGain
```

OscPitch and SyncPitch equations:

```
oscFreq  = noteFreq * pow(2.0f, cvOscPitch * 32.0f / 12.0f)
syncFreq = oscFreq * cvSyncPitch
```

OscMod and SyncMod are added to phase.

```
phase += frequency / sampleRate + cvMod
phase -= floor(phase)
```

## CV_PTRTrapezoid
Monophonic PTR trapezoid oscillator. Double precision is used internally. PTRTrapezoid changes PTR order according to current frequency.

- 1 event input
- 5 CV input
- 1 CV output.

### CV Mapping
Equation to calculate gain value:

```
gain = paramGain * paramBoost + cvGain
```

Equation to convert from OscPitch CV to frequency:

```
oscFreq = noteFreq * pow(2.0f, cvPitch * 32.0f / 12.0f)
```

PhaseMod is added to oscillator phase.

```
phase += frequency / sampleRate + cvPhaseMod
phase -= floor(phase)
```

Input to PulseWidth should be in range of `[-1, 1]`. Excessed range will be hard clipped.

OscSlope will be multiplied by the value of `slope` parameter.

## CV_Sin
Monophonic sine oscillator.

- 1 event input
- 3 CV input
- 1 CV output.

### CV Mapping
Equation to calculate gain value:

```
gain = paramGain * paramBoost + cvGain
```

Equation to convert from Pitch CV to frequency:

```
oscFreq = noteFreq * pow(2.0f, cvPitch * 32.0f / 12.0f)`
```

Phase CV is added to sine phase for each sample.

```
phase = fmodf(phase + twopi * freq / sampleRate + cvPhase, twopi);
return sinf(phase);
```