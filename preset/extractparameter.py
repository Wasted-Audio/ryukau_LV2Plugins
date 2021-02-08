import re
import json
import sys
from pathlib import Path

def convert_string(string: str):
    try:
        value = int(string)
        return value
    except ValueError:
        try:
            value = float(string)
            return value
        except ValueError:
            if string == "true":
                return True
            elif string == "false":
                return False
            return string

def get_enum_range(code):
    # Sort parameter by the order defined in enum ParameterID::ID.
    enum = re.findall(
        r"namespace ParameterID \{\nenum ID.*\{([\s\S]*)\};\n\} // namespace ParameterID",
        code)
    enum = [
        line.strip().replace(",", "")
        for line in enum[0].split("\n")
        if line.strip() != "" and "//" not in line
    ]

    temp = []
    index = -1
    for line in enum:
        if line.find("=") == -1:
            index += 1
            temp.append({"name": line, "index": index})
            continue
        name, end_str = line.replace(" ", "").split("=")
        index = int(end_str)
        temp.append({"name": name, "index": index})

    index = 0
    for idx in range(len(enum) - 1):
        temp[idx]["range"] = temp[idx + 1]["index"] - temp[idx]["index"]
    temp.pop() # Delete ID_ENUM_LENGTH

    enum = {}
    for elem in temp:
        name = elem.pop("name")
        enum[name] = elem
    return enum

def extract_parameter(parameter_hpp_path):
    with open(parameter_hpp_path, "r", encoding="utf-8") as fi:
        code = fi.read()

    plugin_name = re.findall(r"This file is part of (.*)\n", code)[0][:-1]
    enum = get_enum_range(code)

    # Using python struct module format character to represent type.
    # "I" is unsigned int with standard size of 4 bytes.
    # "d" is double with standard size of 8 bytes.
    data = {}
    for matched in re.findall(
            r"value\[.*ID::(.*)\]\s*=\s*std::make_unique<(.*)>\(\s*([^,]*),\s*([^,]*),\s*[^,]*,\s*([^\)]*)\);",
            code,
    ):
        datatype = None
        if matched[1] == "IntValue":
            datatype = "I"
        else:
            datatype = "d"

        if matched[0].find(" + i") != -1:                 # TODO: Change this to regex.
            name, _, _ = matched[0].rpartition(" + i")
            basename = name[0:-1]                         # remove '0'.
            for i in range(enum[name]["range"]):
                code = f"{name} + {i}"
                default = matched[2].replace("idx", str(i))
                data[f"{basename}{i}"] = {
                    "type": datatype,
                    "index": code,
                    "default": convert_string(default),
                    "scale": matched[3],
                    "flags": matched[4],
                }
        else:
            data[matched[0]] = {
                "type": datatype,
                "index": matched[0],
                "default": convert_string(matched[2]),
                "scale": matched[3],
                "flags": matched[4],
            }

    json_dir = Path(__file__).parent / Path("json")
    json_dir.mkdir(parents=True, exist_ok=True)
    with open(json_dir / Path(f"{plugin_name}.type.json"), "w", encoding="utf-8") as fi:
        json.dump(data, fi, indent=2)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Requires parameter.hpp file path as first argument.")
        exit(1)

    if len(sys.argv) > 2:
        print("Only first argument is used. Second or later arguments are discarded.")

    parameter_hpp_path = Path(sys.argv[1])
    if not parameter_hpp_path.exists():
        print("Path doesn't exist.")
        exit(1)

    extract_parameter(parameter_hpp_path)
