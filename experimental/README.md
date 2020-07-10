# Experimental Plugins
Experimental Plugins is not well tuned. They are also unstable in term of development. Expect breaking changes in future.

## Building
Change `build` rule in the top level `Makefile` in this repository as following.

```make
# build: common \
# 	lv2cvport \
# 	CubicPadSynth \
#   ...

build: experimental
```

## KSCymbal
Porting of [web version](https://ryukau.github.io/KSCymbal/).

TODO list:
- Add envelope.
- Add frequency randomize strategy.
- Add termination mechanism.
- Experiment with other excitation.
- Experiment with other filters.

## KuramotoModel
Based on [Kuramoto model](https://en.wikipedia.org/wiki/Kuramoto_model) of synchronization.

Original Kuramoto model is a bit too naive for musical application. Adding decay to Kuramoto model improved the sound.

The sound is similar to FM.

TODO list:
- Change oscillator to use wavetable.
- Implement full pitch bend.
- Improve UI. Especially for oscillator pitch control.
- Tune parameter.

Some results:
- Adding overtone is inefficient.
- Oversampling didn't help much to reduce noise.
- Strong coupling tends to converge too fast.
- VCL helps to speed up.

## BubbleSynth
It sounds like bubble when turning up `Xi` parameter.

I thought this might be good for hand clap synth.

## NoiseTester
Outputs gaussian/uniform noise signal.

## RingDelaySynth
**Caution**: Output may blow up.

16 delays connected as a ring. The output of the ring will be fed into 8 cascaded Karplus-Strong delay. The ring also has a feedback.

This was an attempt to make a bass drum synthesizer, but the result was nowhere near close.
