# LV2Plugins
Porting of VST3 Plugins from https://github.com/ryukau/VSTPlugins .

# Build
On Linux, open terminal and run following command.

```bash
git clone <this repository url>
cd LV2Plugins
git submodule update --init --recursive
make -j
make install # Copy *.lv2 to ~/.lv2
```

Binaries are built into `LV2Plugins/bin`.

# License
GPLv3. See License directory for the complete license.

## Library
- [DPF - DISTRHO Plugin Framework](https://github.com/DISTRHO/DPF)

## Font
- [Tinos](https://fonts.google.com/specimen/Tinos)
