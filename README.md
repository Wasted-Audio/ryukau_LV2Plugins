**Important**: Development is freezed.

In recent years, more hosts started supporting VST 3, or they support VST 3 better than before. So there's less reason to port my plugins to LV2. This decision is mostly to reduce maintainance on my side. See [this link](https://github.com/ryukau/LV2Plugins/blob/master/docs/dev_note/code_walkthrough.md#difference-to-vst-3-repository) for the problems I faced.

# Uhhyou Plugins LV2
Audio plugins for Linux.

VST3 version is available at https://github.com/ryukau/VSTPlugins .

- [Plugin Manuals](https://ryukau.github.io/VSTPlugins/)

# Package
For Arch user, package is available on AUR. Thanks to mxmilkb for packaging.

- [AUR (en) - uhhyou.lv2-git](https://aur.archlinux.org/packages/uhhyou.lv2-git)

For openSUSE user, package is available on openSUSE Build Service. Thanks to Konstantin Voinov for packaging.

- [Show home:kill_it / Uhhyou-Plugins - openSUSE Build Service](https://build.opensuse.org/package/show/home:kill_it/Uhhyou-Plugins)
- [Show home:kill_it / vst3-Uhhyou-Plugins - openSUSE Build Service](https://build.opensuse.org/package/show/home:kill_it/vst3-Uhhyou-Plugins)

# Building
On Ubuntu 18.04, open terminal and run following command.

```bash
sudo apt install \
  git \
  pkg-config \
  libjack-jackd2-dev \
  libgl-dev \
  liblo-dev

git clone --recursive <this repository url>
cd LV2Plugins

make -j8 # If build process goes out of memory, try removing the -j option.

make installHome     # Copy *.lv2 to ~/.lv2
make installHomeVST2 # Copy *-vst.so to ~/.lxvst
```

Binaries are built into `LV2Plugins/bin`.

To rebuild, use `make clean` or `make cleanall`.

```bash
cd LV2Plugins
make clean    # Remove LV2Plugins/bin directory.
make cleanall # Remove LV2Plugins/bin and files built for DPF.
```

To disable particular plugin type, set the variables `LV2`, `JACK`, `VST2` to `false`.

```bash
cd LV2Plugins
make -j LV2=false             # Disable LV2 build.
make -j JACK=false VST2=false # Disable jack and vst2 build.
```

## System Wide Installation
Run `make install` to install plugins on system wide.

Following variables can be set to specify install path:

| Variable      | Default                         | Usage                                                              |
| ------------- | ------------------------------- | ------------------------------------------------------------------ |
| `PREFIX`      | `/usr/local`                    | Prefix of install path.                                            |
| `lv2dir`      | `$(DESTDIR)$(PREFIX)/lib/lv2`   | LV2 plugin directory path.                                         |
| `vst2dir`     | `$(DESTDIR)$(PREFIX)/lib/lxvst` | VST 2 plugin directory path.                                       |
| `bindir`      | `$(DESTDIR)$(PREFIX)/bin`       | Binary directory path. Used to install JACK binaries.              |
| `sysconfdir`  | `$(DESTDIR)$(PREFIX)/etc`       | Plugin config directory path.                                      |
| `datarootdir` | `$(DESTDIR)$(PREFIX)/share`     | Root directory for data and documentation.                         |
| `datadir`     | `$(datarootdir)`                | Plugin resource directory path. Used for `style/theme`.            |
| `docdir`      | `$(datarootdir)/doc`            | Plugin documentation directory path.                               |
| `DESTDIR`     |                                 | Optional. Used for staging installations to temporary directories. |

Currently, plugins only reads config in following directories. The number shows precedence:

1. `$XDG_CONFIG_HOME`
2. `/usr/local/etc`
3. `/etc`

If you are planning to use other directory for `sysconfdir`, feel free to open issue.

Examples:

```bash
make install PREFIX="/usr" # install everything but config to `/usr`

# Test directory structure.
mkdir test
make install DESTDIR="test"
```

- [Directory Variables (GNU Coding Standards)](https://www.gnu.org/prep/standards/html_node/Directory-Variables.html)
- [Practical Makefiles, by example](http://nuclear.mutantstargoat.com/articles/make/#going-the-extra-mile-for-release)

## Patch
Temporary patches to libraries are placed under `patch` directory. Patches are automatically applied with `make`.

See following issues for details.

- https://github.com/DISTRHO/DPF/issues/216
- https://github.com/DISTRHO/DPF/pull/218

# Plugins
Note that some parameter configuration leads to massive DC offset. To stay safe, it's better to insert high-pass filter after these plugins. Monitoring output with oscilloscope is recommended.

## Color Configuration
Color configuration is placed at `$XDG_CONFIG_HOME/UhhyouPlugins/style/style.json`. If `$XDG_CONFIG_HOME` is empty, it defaults to `$HOME/.config`.

- [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html)

If you are building from source, `make install` also installs color configuration files.

For more details, please refer to [style/ColorConfig.md](https://github.com/ryukau/LV2Plugins/tree/l4reverb/style/ColorConfig.md). When you made a nice color theme, feel free to send a patch.

## Controls
Knobs, sliders etc. has common functionalities listed on below.

- <kbd>Shift</kbd> + <kbd>Mouse Left Drag</kbd>: Fine adjustment.
- <kbd>Ctrl</kbd> + <kbd>Left Click</kbd>: Reset to default.

Some controls turn red when pointing. They can be the cause of potential blow up or stopping all sounds for a while. It is recommended to always change those controls slowly with <kbd>Shift</kbd> + <kbd>Mouse Left Drag</kbd>.

TextKnob which is shown in the image below has an additional control <kbd>Mouse Right Click</kbd> to cycle min/mid/max.

<figure>
<img src="docs/img/textknob.png" alt="Image of TextKnob." style="padding-bottom: 12px;"/>
</figure>

BarBox controls which is shown in the image below have keyboard shortcuts. Shortcuts are only enabled when cursor is on overtone control. Cheat sheet may be available on plugin information pop-up or information tab.

<figure>
<img src="docs/img/barbox.png" alt="Image of BarBox." style="padding-bottom: 12px;"/>
</figure>

| Input                                                      | Control                            |
| ---------------------------------------------------------- | ---------------------------------- |
| <kbd>Left Drag</kbd>                                       | Change Value                       |
| <kbd>Shift</kbd> + <kbd>Left Drag</kbd>                    | Change Value (Snapped)             |
| <kbd>Ctrl</kbd> + <kbd>Left Drag</kbd>                     | Reset to Default                   |
| <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>Left Drag</kbd>  | Change Value (Skip Between Frames) |
| <kbd>Right Drag</kbd>                                      | Draw Line                          |
| <kbd>Shift</kbd> + <kbd>Right Drag</kbd>                   | Edit One Bar                       |
| <kbd>Ctrl</kbd> + <kbd>Right Drag</kbd>                    | Reset to Default                   |
| <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>Right Drag</kbd> | Toggle Lock                        |
| <kbd>a</kbd>                                               | Alternate Sign                     |
| <kbd>d</kbd>                                               | Reset Everything to Default        |
| <kbd>D</kbd>                                               | Toggle Min/Mid/Max                 |
| <kbd>e</kbd>                                               | Emphasize Low                      |
| <kbd>E</kbd>                                               | Emphasize High                     |
| <kbd>f</kbd>                                               | Low-pass Filter                    |
| <kbd>F</kbd>                                               | High-pass Filter                   |
| <kbd>i</kbd>                                               | Invert Value (Preserve minimum)    |
| <kbd>I</kbd>                                               | Invert Value (Minimum to 0)        |
| <kbd>l</kbd>                                               | Toggle Lock under Mouse Cursor     |
| <kbd>L</kbd>                                               | Toggle Lock for All                |
| <kbd>n</kbd>                                               | Normalize (Preserve minimum)       |
| <kbd>N</kbd>                                               | Normalize (Minimum to 0)           |
| <kbd>p</kbd>                                               | Permute                            |
| <kbd>r</kbd>                                               | Randomize                          |
| <kbd>R</kbd>                                               | Sparse Randomize                   |
| <kbd>s</kbd>                                               | Sort Descending Order              |
| <kbd>S</kbd>                                               | Sort Ascending Order               |
| <kbd>t</kbd>                                               | Subtle Randomize (Random walk)     |
| <kbd>T</kbd>                                               | Subtle Randomize (Converge to 0)   |
| <kbd>z</kbd>                                               | Undo                               |
| <kbd>Z</kbd>                                               | Redo                               |
| <kbd>,</kbd> (Comma)                                       | Rotate Back                        |
| <kbd>.</kbd> (Period)                                      | Rotate Forward                     |
| <kbd>1</kbd>                                               | Decrease                           |
| <kbd>2</kbd>-<kbd>9</kbd>                                  | Decrease 2n-9n                     |

Snapping is not available for all BarBox. If you'd like to have snapping for certain BarBox, feel free to open issue.

Edit One Bar with <kbd>Shift</kbd> + <kbd>Right Drag</kbd> holds a bar under the cursor when mouse right button is pressed. Then only changes that one bar while holding down mouse right button.

Toggle Lock with <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>Right Drag</kbd> behaves as line edit. When right mouse button (RMB) is pressed, it holds the opposite state of the bar below mouse cursor, then use the state for the rest of bars. For example, if RMB is pressed on a locked bar, dragging unlocks bars while holding down RMB.

Some dense BarBox has scroll bar to zoom in. Scroll bar has handles on left and right end. To control zoom, use <kbd>Left Drag</kbd> on one of the handle, or roll <kbd>Mouse Wheel</kbd> on scroll bar region. <kbd>Right Click</kbd> can be used to reset the zoom. When plugin window is closed, zoom will be reset. Following image shows a scroll bar under a BarBox.

<figure>
<img src="docs/img/barbox_dense.png" alt="Image of scroll bar under BarBox." style="padding-bottom: 12px;"/>
</figure>

## CollidingCombSynth
<figure>
<img src="docs/img/lv2_collidingcombsynth.png" alt="Image of CollidingCombSynth GUI."/>
</figure>

CollidingCombSynth (CCS) is an experimental synthesizer which sounds like bowed string. Tuning of pitch is difficult, so mostly suitable for sound effects.

CCS is based on [KSCymbal](https://ryukau.github.io/KSCymbal/). The idea is to collide Karplus-Strong string model to each other. "Collision" in CCS works like a kind of oscillator sync.

Up to 192kHz sample rate is supported.

**Caution**:
- Output may be loud. Recommend to always insert limiter after CCS, even when `Compressor` is turned on.
- DSP load spikes at note-on.

## L4Reverb/L3Reverb
<figure>
<img src="docs/img/lv2_l4reverb.png" alt="Image of L4Reverb GUI."/>
</figure>

L4Reverb is an extended version of LatticeReverb. This time, lattice structure has 4 * 4 * 4 * 4 = 256 sections per channel.

Note that L4Reverb is heavy weight. DSP load is around 4 times higher than [Dragonfly Room Reverb](https://michaelwillis.github.io/dragonfly-reverb/) on my environment.

If the DSP load is a problem, L3Reverb can be used. L3Reverb is a lightweight version of L4Reverb. Lattice structure has 3 * 3 * 3 * 5 = 135 sections, so the DSP load is roughly half of L4Reverb.

**Caution**:

- Output may change with different sample rate or buffer size.
- Output may blow up when both `Cross` and `Spread` are not 0.
- Output may become loud when offset of `*Feed` is not 0 and modulation is enabled.
- Output may become loud when following steps are performed.
  1. Set some of the OuterFeed or InnerFeed to close to minimum or maximum.
  2. Input signals.
  3. Change the value of OuterFeed or InnerFeed which was set at step 1.

## LatticeReverb
<figure>
<img src="docs/img/lv2_latticereverb.png" alt="Image of LatticeReverb GUI."/>
</figure>

LatticeReverb is a stereo reverb using lattice structure. Equipped with 16 delays per channel. All internal parameters are exposed.

**Caution**:

- Output may change with different sample rate or buffer size.
- Output may become loud when following steps are performed.
  1. Set some of the OuterFeed or InnerFeed to close to minimum or maximum.
  2. Input signals.
  3. Change the value of OuterFeed or InnerFeed which was set at step 1.

Lattice structure reference.

- [Nested Allpass Filters](https://ccrma.stanford.edu/~jos/pasp/Nested_Allpass_Filters.html)

## LightPadSynth
<figure>
<img src="docs/img/lv2_lightpadsynth.png" alt="Image of LightPadSynth GUI."/>
</figure>

LightPadSynth is a lighter version of CubicPadSynth. For efficiency, following changes are made:

- Removed pitch modulation. Feedback delay is added instead.
- Changed LFO to modulate delay time.
- Changed interpolation from cubic to linear.
- Changed filter from wavetable low-pass to naive IIR low-pass.

**Caution**: Filter is a bit peaky. Be careful when changing `resonance`.

Wavetable and LFO will not refresh automatically. To refresh, press `Refresh Wavetable` or `Refresh LFO` button.

Cheat sheet of shortcuts is available on Information tab.

## WaveShaper Pack
<figure>
<img src="docs/img/lv2_foldshaper.png" alt="Image of FoldShaper GUI." style="padding-bottom: 12px;"/>
</figure>

<figure>
<img src="docs/img/lv2_moduloshaper.png" alt="Image of ModuloShaper GUI." style="padding-bottom: 12px;"/>
</figure>

<figure>
<img src="docs/img/lv2_oddpowshaper.png" alt="Image of OddPowShaper GUI." style="padding-bottom: 12px;"/>
</figure>

<figure>
<img src="docs/img/lv2_softclipper.png" alt="Image of SoftClipper GUI." style="padding-bottom: 12px;"/>
</figure>

WaveShaper Pack contains 4 stereo waveshapers. Waveshaping algorithms are naive, but all provides option for 16x oversampling.

**Caution**: Do not use `More*` parameters without inserting a limiter after waveshaper. Output amplitude may become very loud.

Oversampling may not be necessary for SoftClipper, unless input signal contains high frequency (4000 Hz or higher).

`Hardclip` option guarantees safe output which amplitude is in between -1.0 to 1.0. However, `Hardclip` kills the characteristic of waveshaper.

## CubicPadSynth
<figure>
<img src="docs/img/lv2_cubicpadsynth.png" alt="Image of CubicPadSynth GUI."/>
</figure>

CubicPadSynth is a wavetable synthesizer which uses PADsynth algorithm to generate oscillator tables. Cubic interpolation is used to get smooth sound even at inaudible low frequency range. LFO waveform can be directly drawn.

- [PADsynth algorithm](https://zynaddsubfx.sourceforge.io/doc/PADsynth/PADsynth.htm)

Wavetable and LFO will not refresh automatically. To refresh, press `Refresh Wavetable` or `Refresh LFO` button.

Some parameters have wide range of value. <kbd>Shift</kbd> + <kbd>Left Drag</kbd> can be used to fine adjustment.

Cheat sheet of shortcuts is available on Information tab.

## EsPhaser
<figure>
<img src="docs/img/lv2_esphaser.png" alt="Image of EsPhaser GUI."/>
</figure>

EsPhaser is a phaser with up to 4096 stages of order 2 Thiran all-pass filters. This is the same phaser used in EnvelopedSine.

**Caution**:
- When `stage` is set to 4096, it will be CPU intensive.
- Output varies in different sample rate.
- Output may be loud when changing `Cas. Offset`. Use <kbd>Shift</kbd> + <kbd>Mouse Left Drag</kbd> to slowly change the value, or insert limiter to prevent hard clipping.

## EnvelopedSine
<figure>
<img src="docs/img/lv2_envelopedsine.png" alt="Image of EnvelopedSine GUI."/>
</figure>

EnvelopedSine is another additive synthesizer which computes 64 sine waves per note. There are individual controls of attack, decay and saturation for each sine oscillator.

EnvelopedSine uses stable quadrature oscillator (QuadOsc) algorithm described by Martin Vicanek. QuadOsc is used because I couldn't figure out how to set initial phase for faster algorithm.

- [QuadOsc.pdf](https://www.vicanek.de/articles/QuadOsc.pdf)

Order 2 Thiran all-pass filters are used for phaser.

- [Thiran Allpass Interpolators](https://ccrma.stanford.edu/~jos/pasp/Thiran_Allpass_Interpolators.html)

## IterativeSinCluster
<figure>
<img src="docs/img/lv2_iterativesincluster.png" alt="Image of IterativeSinCluster GUI."/>
</figure>

IterativeSinCluster is an additive synthesizer. This synth computes 512 sine waves for each note to make tone cluster. I somehow thought "iterative sin" was a thing, but the correct term is "recursive sine".

Overview of calculation of pitch for each sine wave. Parameter `ET` stands for equal temperament. `Milli` means 1 milli semitone = 1 / 10 cent.

```
function toneToPitch(semi, milli):
  return 2 ^ (1000 * semi + milli) / (ET * 1000)

for each Chord:
  chordPitch = toneToPitch(Chord.semi, Chord.milli)
  for each Note:
    notePitch = toneToPitch(Note.semi, Note.milli)
    for each Overtone:
      frequency = midiNoteFrequency
        * (1 + mod(Multiply * Overtone * notePitch * chordPitch, Modulo))
```

Various recursive sin algorithm described by Martin Vicanek. IterativeSinCluster uses biquad oscillator.

- [QuadOsc.pdf](https://www.vicanek.de/articles/QuadOsc.pdf)

## TrapezoidSynth
<figure>
<img src="docs/img/lv2_trapezoidsynth.png" alt="Image of TrapezoidSynth GUI."/>
</figure>

TrapezoidSynth is a monophonic synthesizer equipped with trapezoid oscillator which is made from 5th order PTR ramp function. In other words, it's just a fancy version of tri-pulse oscillator. 2 pitch shifters are added to make some chord.

PTR trapezoid oscillator algorithm.

- [PTR 台形オシレータ](https://ryukau.github.io/filter_notes/ptr_trapezoid/ptr_trapezoid.html)

AM pitch shifter algorithm described by Scott Wardle.

- [WAR19 - Audio_Hilbert_WAR19.pdf](https://www.mikrocontroller.net/attachment/33905/Audio_Hilbert_WAR19.pdf)

IIR Hilbert filter by Olli Niemitalo. This filter is used in AM pitch shifter to approximate Hilbert transform.

- [Hilbert transform – iki.fi/o](http://yehar.com/blog/?p=368)

## FDNCymbal
<figure>
<img src="docs/img/lv2_fdncymbal.png" alt="Image of FDNCymbal GUI."/>
</figure>

FDNCymbal is another attempt to make cymbal sounds. This one at least sounds like hitting a metal plate. Unlike the name, most of metalic texture comes from stage 1 of [Schroeder allpass section](https://ccrma.stanford.edu/~jos/pasp/Schroeder_Allpass_Sections.html) rather than [FDN](https://ccrma.stanford.edu/~jos/cfdn/Feedback_Delay_Networks.html) (feedback delay network). FDN section makes nice impact sound when `FDN->Time` is short. Tremolo is added to simulate wobbling of cymbal.

FDNCymbal can be used as both synth/effect. To turn off oscillator, click `Stick` button.

**Caution**:
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
- [VCL - Vector class library version2](https://github.com/vectorclass/version2)
- [FFTW](http://fftw.org/)
- [PocketFFT for C++](https://gitlab.mpcdf.mpg.de/mtr/pocketfft/-/tree/cpp)
- [JSON for Modern C++](https://github.com/nlohmann/json)
- [Filesystem](https://github.com/gulrak/filesystem)

## Font
- [Tinos](https://fonts.google.com/specimen/Tinos)
