# LV2Plugins
Porting of VST3 Plugins from https://github.com/ryukau/VSTPlugins .

# Building
On Linux, open terminal and run following command.

```bash
git clone <this repository url>
cd LV2Plugins
git submodule update --init --recursive
make -j
make install # Copy *.lv2 to ~/.lv2
```

Binaries are built into `LV2Plugins/bin`.

# Plugins
Note that some parameter configuration leads to massive DC offset. To be stay safe, it's better to insert high-pass filter after these plugins. Monitoring output with oscilloscope is recommended.

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

## Font
- [Tinos](https://fonts.google.com/specimen/Tinos)
