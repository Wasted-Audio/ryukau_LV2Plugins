# LV2Plugins
Audio plugins for Linux.

VST3 version is available at https://github.com/ryukau/VSTPlugins .

# Building
On Ubuntu 18.04, open terminal and run following command.

```bash
sudo apt install git pkg-config libjack-jackd2-dev libgl-dev liblo-dev
git clone --recursive <this repository url>
cd LV2Plugins
make -j
make install # Copy *.lv2 to ~/.lv2
```

Binaries are built into `LV2Plugins/bin`.

# Plugins
Note that some parameter configuration leads to massive DC offset. To stay safe, it's better to insert high-pass filter after these plugins. Monitoring output with oscilloscope is recommended.

Some controls turns red when pointing. They can be the cause of potential blow up. It is recommended to always change those controls slowly with <kbd>Shift</kbd> + <kbd>Mouse Left Drag</kbd>.

## TrapezoidSynth
<figure>
<img src="docs/img/lv2_trapezoidsynth.png" alt="Image of TrapezoidSynth GUI."/>
</figure>

TrapezoidSynth a monophonic synthesizer equipped with trapezoid oscillator which is made from 5th order PTR ramp function. In other words, it's just a fancy version of tri-pulse oscillator. 2 pitch shifters are added to make some chord.

AM pitch shifter algorithm described by Scott Wardle.

- [WAR19 - Audio_Hilbert_WAR19.pdf](https://www.mikrocontroller.net/attachment/33905/Audio_Hilbert_WAR19.pdf)

Hilbert filter by Olli Niemitalo. This filter is used in AM pitch shifter.

- [Hilbert transform – iki.fi/o](http://yehar.com/blog/?p=368)

## FDNCymbal
<figure>
<img src="docs/img/lv2_fdncymbal.png" alt="Image of FDNCymbal GUI."/>
</figure>

FDNCymbal is another attempt to make cymbal sounds. This one at least sounds like hitting a metal plate. Unlike the name, most of metalic texture comes from stage 1 of [Schroeder allpass section](https://ccrma.stanford.edu/~jos/pasp/Schroeder_Allpass_Sections.html) rather than [FDN](https://ccrma.stanford.edu/~jos/cfdn/Feedback_Delay_Networks.html) (feedback delay network). FDN section makes nice impact sound when `FDN->Time` is short. Tremolo is added to simulate wobbling of cymbal.

FDNCymbal can be used as both synth/effect. To turn off oscillator, click `Stick` button.

Caution:
- When HP Cutoff is moving fast, it may output massive DC. To avoid this, use <kbd>Shift</kbd> + <kbd>Mouse Left Drag</kbd> or turn up `Smooth`.
- When FDN->Feedback is non zero, it may possibly blow up. If that happens, turn `FDN->Feedback` to leftmost.

JavaScript version is available. Requires browser that supports Web Worker and Web Audio. This is not exactly the same as plugin version, but using similar structure.

- [FDNCymbal](https://ryukau.github.io/FDNCymbal/)

## WaveCymbal
<figure>
<img src="docs/img/lv2_wavecymbal.png" alt="Image of WaveCymbal GUI."/>
</figure>

WaveCymbal is a banded wave-guide synthesizer. This is an attempt to make cymbal sounds, but result is more like dragging bunch of empty cans on asphalt. Be cautious tweaking controls that turns red when pointing. They can drastically change output gain.

WaveCymbal can be used as a effect. To turn off oscillator, change `Oscillator->Sustain` to `Off`.

Block diagram of WaveCymbal. BP is short for Band-pass.

<figure>
<img src="docs/img/wavecymbal_block_diagram.svg" alt="Image of block diagram of WaveCymbal."/>
</figure>

JavaScript version is available. Requires browser that supports Web Worker and Web Audio.

- [WaveCymbal Demo](https://ryukau.github.io/WaveCymbal/)

The idea of banded wave-guide is based on Figure 4 in this paper.

- Essl, Georg, et al. "[Theory of banded waveguides.](https://cpb-us-w2.wpmucdn.com/sites.uwm.edu/dist/0/236/files/2016/09/CMJ04-theory-2ib3uzx.pdf)" Computer Music Journal 28.1 (2004): 37-50.

## SyncSawSynth
<figure>
<img src="docs/img/lv2_syncsawsynth.png" alt="Image of SyncSawSynth GUI."/>
</figure>

SyncSawSynth is a synthesizer with up to 10th order PTR sawtooth oscillator. PTR (Polynomial Transition Regions) is an aliasing suppressing technique.

PTR paper by Kleimola and Valimaki.

- Jari Kleimola, and Vesa Valimaki. “[Reducing aliasing from synthetic audio signals using polynomial transition regions.](https://aaltodoc.aalto.fi/bitstream/handle/123456789/7747/publication6.pdf?sequence=9&isAllowed=y)” IEEE Signal Processing Letters 19.2 (2011): 67-70.

Derivation of higher order PTR equations (Japanese).

- [PTR オシレータ](https://ryukau.github.io/filter_notes/ptr_oscillator/ptr_oscillator.html)

## SevenDelay
<figure>
<img src="docs/img/lv2_sevendelay.png" alt="Image of SevenDelay GUI."/>
</figure>

SevenDelay is using 7th order lagrange fractional delay. Also 7 times oversampled. Suitable for making sound with actively changing parameters by hand or DAW automation. Parameters are tuned towards short delay.

Lagrange FD explained by Julius O. Smith III. PDFs at the bottom of the page is better formatted.

- [MUS420 Lecture 4A Interpolated Delay Lines, Ideal Bandlimited Interpolation, and Fractional Delay Filter Design](https://ccrma.stanford.edu/~jos/Interpolation/)

Implementation of delay (Japanese).

- [ディレイの実装](https://ryukau.github.io/filter_notes/delay/delay.html)

## Other Notice
Plugins using filter from:
- [RBJ Audio-EQ-Cookbook — Musicdsp.org documentation](https://www.musicdsp.org/en/latest/Filters/197-rbj-audio-eq-cookbook.html)
- [Book: The Art of VA Filter Design 2.1.0 - KVR Audio](https://www.kvraudio.com/forum/viewtopic.php?t=350246)

# License
GPLv3. See License directory for the complete license.

## Library
- [DPF - DISTRHO Plugin Framework](https://github.com/DISTRHO/DPF)
- [juce_FastMathApproximations.h](https://github.com/WeAreROLI/JUCE/blob/master/modules/juce_dsp/maths/juce_FastMathApproximations.h) from [JUCE](https://juce.com/)

## Font
- [Tinos](https://fonts.google.com/specimen/Tinos)
