import json
import sys
from pathlib import Path

def json_to_cpp(type_json_path, preset_json_path, prefix="preset"):
    """
    Add prefix to preset name because C++ identifier couldn't start with number, symbols etc.
    """
    with open(preset_json_path, "r", encoding="utf-8") as fi:
        preset_list = json.load(fi)
    default_index = next(
        idx for idx, it in enumerate(preset_list) if it["name"] == "Default")
    default_preset = preset_list.pop(default_index)
    preset_list = sorted(preset_list, key=lambda it: it["name"])
    preset_list.insert(0, default_preset)

    with open(type_json_path, "r", encoding="utf-8") as fi:
        parameter_type = json.load(fi)

    indent = "  "

    # loadProgram() method
    code = """#include "parameter.hpp"

// Generated from preset dump.
#ifndef TEST_BUILD

void GlobalParameter::loadProgram(uint32_t index)
{
  using ID = ParameterID::ID;

  switch (index) {
    default:
      resetParameter();
      break;
"""
    for preset in preset_list:
        name = preset["name"]
        code += f"\n{indent * 2}case {prefix + name}: {{\n"
        for name, value in preset["parameter"].items():
            dtype = parameter_type[name]["type"]
            index = parameter_type[name]["index"]
            if name == "bypass":
                value = 0

            if dtype == "I":
                code += f"{indent * 3}value[ID::{index}]->setFromInt({value});\n"
            elif dtype == "d":
                code += f"{indent * 3}value[ID::{index}]->setFromNormalized({value});\n"
            else:
                code += f"{indent * 3}value[ID::{index}]->setFromNormalized({value});\n"

        code += f"{indent * 2}}} break;\n"
    code += f"{indent}}}\n}}\n\n#endif\n"

    out_dir = Path(__file__).parent / Path("cpp")
    out_dir.mkdir(parents=True, exist_ok=True)
    plugin_name = str(preset_json_path.name).split(".")[0]
    with open(out_dir / Path(f"{plugin_name}_preset_code.cpp"), "w") as code_file:
        code_file.write(code)

    # std::array and enum
    code = "enum Preset {\n"
    for preset in preset_list:
        code += f"{indent}{prefix + preset['name']},\n"
    code += f"\n{indent}Preset_ENUM_LENGTH,\n"
    code += "};\n\n"

    code += f"std::array<const char*, {len(preset_list)}> programName{{\n"
    for preset in preset_list:
        name = preset['name']
        code += f'{indent}"{name}",\n'
    code += "};\n"

    with open(out_dir / Path(f"{plugin_name}_preset_enum.cpp"), "w") as code_file:
        code_file.write(code)

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

    json_to_cpp(type_json_path, preset_json_path, "preset")
