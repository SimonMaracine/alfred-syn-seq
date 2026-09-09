#! /usr/bin/env python3

import sys
import pathlib


def _contents(variable_name: str, string_value: str) -> str:
    return f"""
#pragma once

inline constexpr const char* {variable_name} = "{string_value}";
"""


def _generate(variable_name: str, output_file_path: str, string_value: str):
    file_path = pathlib.Path(f"include/{output_file_path}.hpp")
    file_path.parent.mkdir(parents=True, exist_ok=True)

    with open(f"include/{output_file_path}.hpp", "w") as file:
        file.write(_contents(variable_name, string_value))


def _main(argv: list[str]) -> int:
    if len(argv) != 4:
        print("Invalid arguments", file=sys.stderr)
        return 1

    variable_name = argv[1]
    output_file_path = argv[2]
    string_value = argv[3]

    try:
        _generate(variable_name, output_file_path, string_value)
    except Exception as err:
        print(f"An error occurred: {err}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(_main(sys.argv))
