# VST 3 Preset Converter
This directory contains python scripts to convert VST 3 Preset to json and C++ source code.

Usage:

Copy VST 3 preset directory of a plugin to `LV2Plugins/preset/vstpreset/Uhhyou`. Then run `run.py`.

```
cd LV2Plugins/preset
mkdir vstpreset/Uhhyou
cp -r PresetDir vstpreset/Uhhyou/PresetDir

python path/to/run.py PluginName
```

## extractparameter.py
Extract parameter properties from `parameter.hpp` to `*.type.json`.

Most important property is type of parameter. It is required to get parameter values from `*.vstpreset` file.

## vstpresettojson.py
Convert `*.vstpreset` to `*.preset.json`.

## jsontocpp.py
Convert `*.preset.json` to C++ source code which are only used to this repository.

## jsontovstpreset.py
Convert `*.preset.json` to `*.vstpreset` files.

It is reported that `*.vstpreset` created by VST3PluginTestHost can't be loaded by Live 10 on macOS. However, `*.vstpreset` created by this script can be loaded.

## class_id.json
GUID of VST 3 plugins. VST 3 preset requires processor ID.
