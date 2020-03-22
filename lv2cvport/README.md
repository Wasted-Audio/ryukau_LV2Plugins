# LV2 CVPort Plugins
WIP

## CV_AudioToCv
Convert audio port signal to CV port signal.

1 audio input and 1 CV output.

## CV_CvToAudio
Convert CV port signal to audio port signal.

1 CV input and 1 audio output.

## ExpADSREnvelope
Exponential ADSR Envelope.

1 event input and 1 CV output.

## ExpPolyADEnvelope
Exponential polynomial envelope.

1 event input and 1 CV output.

Note that this envelope resets to 0 for each note-on.

## CV_LinearADSREnvelope
Linear ADSR envelope.

1 event input and 1 CV output.

## CV_Multiply
Multiply CV signal.

2 CV inputs and 1 CV output.

## CV_ParabolicADEnvelope
Parabolic envelope.

1 event input and 1 CV output.

Note that this envelope resets to 0 for each note-on.

## CV_Sin
Monophonic sine oscillator.

3 CV input and 1 CV output.

Gain value expression: `paramGain * paramBoost + cvGain`.

Pitch CV to frequency expression: `powf(2.0f, cvPitch * 32.0f / 12.0f)`.

Phase CV is added to sine phase for each sample:

```
phase = fmodf(phase + twopi * freq / sampleRate + cvPhase, twopi);
return sinf(phase);
```
