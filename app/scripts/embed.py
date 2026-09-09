#! /usr/bin/env python3

import sys
import pathlib


def _contents(file_path: str, mode: str) -> str:
    with open(file_path, mode) as file:
        return file.read()


def _contents_text(variable_name: str, full_file_path: str) -> str:
    return f"""
#pragma once

#include <string_view>

inline constexpr std::string_view {variable_name} =
    R"({_contents(full_file_path, "r")})";
"""


def _contents_binary(variable_name: str, full_file_path: str) -> str:
    return f"""
#pragma once

inline constexpr unsigned char {variable_name}[] {{
    {", ".join((f"{byte}" for byte in _contents(full_file_path, "rb")))}
}};
"""


def _generate(variable_name: str, output_file_path: str, full_file_path: str, file_type: str):
    match file_type:
        case "text":
            file_contents = _contents_text(variable_name, full_file_path)
        case "binary":
            file_contents = _contents_binary(variable_name, full_file_path)
        case _:
            raise RuntimeError("Invalid file type")

    file_path = pathlib.Path(f"include/{output_file_path}.hpp")
    file_path.parent.mkdir(parents=True, exist_ok=True)

    with open(f"include/{output_file_path}.hpp", "w") as file:
        file.write(file_contents)


def _main(argv: list[str]) -> int:
    if len(argv) != 5:
        print("Invalid arguments", file=sys.stderr)
        return 1

    variable_name = argv[1]
    output_file_path = argv[2]
    full_file_path = argv[3]
    file_type = argv[4]

    try:
        _generate(variable_name, output_file_path, full_file_path, file_type)
    except Exception as err:
        print(f"An error occurred: {err}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(_main(sys.argv))
