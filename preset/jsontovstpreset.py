import json
import struct
import sys

from collections import OrderedDict
from pathlib import Path

def json_to_vstpreset(type_json_path, preset_json_path):
    """
    json_path.name.split(".")[0] must be plugin name.

    company is a term used in VST SDK. It means name of plugin vender.
    """
    with open(preset_json_path, "r") as fi:
        preset_list = json.load(fi)

    with open(type_json_path, "r") as fi:
        type_data = json.load(fi, object_pairs_hook=OrderedDict)

    comp_chunk_size = 0
    for _, value in type_data.items():
        comp_chunk_size += struct.calcsize(value["type"])
    offset_to_chunk_list = 48 + comp_chunk_size  # Added 48 bytes offset for header.

    plugin_name = preset_json_path.name.split(".")[0]
    class_id_path = Path(__file__).parent / Path("class_id.json")
    with open(class_id_path, "r") as fi:
        data = json.load(fi)
        class_id = data[plugin_name].encode("ascii")

    parent_dir = Path(__file__).parent / Path(f"vstpreset/FromJson/{plugin_name}")
    Path(parent_dir).mkdir(parents=True, exist_ok=True)
    for preset in preset_list:
        with open(parent_dir / Path(f"{preset['name']}.vstpreset"), "wb") as fi:
            fi.write(b"VST3")  # header id
            fi.write(struct.pack("i", 1))  # version
            fi.write(class_id)
            fi.write(struct.pack("q", offset_to_chunk_list))
            parameter = preset["parameter"]
            for param_name, elem in type_data.items():
                fi.write(struct.pack(elem["type"], parameter[param_name]))
            fi.write(b"List")
            fi.write(struct.pack("i", 1))  # entry count
            fi.write(b"Comp")
            fi.write(struct.pack("q", 48))  # offset to Comp chunk data.
            fi.write(struct.pack("q", comp_chunk_size))  # size of Comp chunk data

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Requires plugin name as first argument.")
        exit(1)

    if len(sys.argv) > 2:
        print("Only first argument is used. Second or later arguments are discarded.")

    plugin_name = sys.argv[1]
    json_dir = Path(__file__).parent / Path("json")

    type_json_path = json_dir / Path(f"{plugin_name}.type.json")
    if not type_json_path.exists():
        print(f"{str(type_json_path)} doesn't exist.")
        exit(1)

    preset_json_path = json_dir / Path(f"{plugin_name}.preset.json")
    if not preset_json_path.exists():
        print(f"{str(preset_json_path)} doesn't exist.")
        exit(1)

    json_to_vstpreset(type_json_path, preset_json_path)
