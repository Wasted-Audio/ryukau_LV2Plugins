# Experimental Plugins
Experimental Plugins is not well tuned. They are also unstable in term of development. Expect breaking changes in future.

## BubbleSynth
It sounds like bubble when turning up `Xi` parameter.

I thought this might be good for hand clap synth.

## NoiseTester
Outputs gaussian/uniform noise signal.

## RingDelaySynth
**Caution**: Output may blow up.

16 delays connected as a ring. The output of the ring will be fed into 8 cascaded Karplus-Strong delay. The ring also has a feedback.

This was an attempt to make a bass drum synthesizer, but the result was nowhere near close.
