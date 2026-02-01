#! /usr/bin/env python3

import sys
import os
from typing import Iterable

def _contents(file_name: str, mode: str) -> str:
    with open(file_name, mode) as file:
        return file.read()


def _contents_text(variable_name: str, file_name: str) -> str | Iterable[int]:
    return f"""
#pragma once

#include <string_view>

inline constexpr std::string_view {variable_name} {{
    R"({_contents(file_name, "r")})"
}};
"""

def _contents_binary(variable_name: str, file_name: str) -> str:
    return f"""
#pragma once

inline constexpr unsigned char {variable_name}[] {{
    {", ".join((f"{byte}" for byte in _contents(file_name, "rb")))}
}};
"""


def _generate(variable_name: str, file_name: str, file_type: str):
    match file_type:
        case "text":
            file_contents = _contents_text(variable_name, file_name)
        case "binary":
            file_contents = _contents_binary(variable_name, file_name)
        case _:
            raise RuntimeError("Invalid file type")

    output_name = file_name.split("/")[-1]

    try:
        os.mkdir("include")
    except Exception as err:
        print(err, file=sys.stderr)

    with open(f"include/{output_name}.hpp", "w") as file:
        file.write(file_contents)


def main(argv: list[str]) -> int:
    if len(argv) != 4:
        print("Invalid arguments", file=sys.stderr)
        return 1

    variable_name = argv[1]
    file_name = argv[2]
    file_type = argv[3]

    try:
        _generate(variable_name, file_name, file_type)
    except Exception as err:
        print(f"An error occurred: {err}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
