import extractparameter
import jsontocpp
import jsontovstpreset
import vstpresettojson

import argparse
import sys
from pathlib import Path

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Requires *.preset.json path as first argument.")
        exit(1)

    if len(sys.argv) > 2:
        print("Only first argument is used. Second or later arguments are discarded.")

    plugin_name = sys.argv[1]
    script_dir = Path(__file__).parent

    parameter_hpp_path = script_dir / Path(f"../{plugin_name}/parameter.hpp")
    if not parameter_hpp_path.exists():
        print(f"{str(parameter_hpp_path)} doesn't exist.")
        exit(1)
    extractparameter.extract_parameter(parameter_hpp_path)

    vstpreset_dir_path = script_dir / Path(f"vstpreset/Uhhyou/{plugin_name}")
    if not vstpreset_dir_path.exists():
        print(f"{str(vstpreset_dir_path)} doesn't exist.")
        exit(1)
    vstpresettojson.vstpreset_to_json(vstpreset_dir_path)

    type_json_path = script_dir / Path(f"json/{plugin_name}.type.json")
    if not type_json_path.exists():
        print(f"{str(type_json_path)} doesn't exist.")
        exit(1)
    preset_json_path = script_dir / Path(f"json/{plugin_name}.preset.json")
    if not preset_json_path.exists():
        print(f"{str(preset_json_path)} doesn't exist.")
        exit(1)
    jsontocpp.json_to_cpp(type_json_path, preset_json_path)
    jsontovstpreset.json_to_vstpreset(type_json_path, preset_json_path)
