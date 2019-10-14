# LV2Plugins

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
