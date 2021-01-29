"""
Generate metadata for PawPaw. See the link below.
https://github.com/DISTRHO/PawPaw/tree/master/plugins
"""

import json
import subprocess
from pathlib import Path

def getGitHash():
    return subprocess.check_output(
        ["git", "rev-parse", "HEAD"],
        encoding="utf-8",
    ).strip()

def getPluginNames():
    names = []
    for path in Path("../..").glob("**/DistrhoPluginInfo.h"):
        if path.parts[2] == "experimental":
            continue
        names.append(path.parts[-2])
    return sorted(names)

meta = {
    "name": "uhhyou-lv2",
    "lv2bundles": getPluginNames(),
    "version": getGitHash(),
    "description": "Uhhyou Plugins bundle.",
    "dlbaseurl": "https://github.com/ryukau/LV2Plugins.git",
    "dlmethod": "git",
    "dlname": "uhhyou-lv2",
    "buildtype": "make",
    "buildargs": "JACK=false VST2=false"
}

with open("uhhyou.json", "w", encoding="utf-8") as fi:
    json.dump(meta, fi, indent=4)
