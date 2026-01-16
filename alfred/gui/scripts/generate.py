#! /usr/bin/env python3

import sys


def _contents(file_name: str) -> str:
    with open(file_name, "r") as file:
        return file.read()


def _generate(variable_name: str, file_name: str):
    contents = _contents(file_name)

    file_contents = (
f"""
#pragma once

#include <string_view>

inline constexpr std::string_view {variable_name} {{
    R"({contents})"
}};
"""
    )

    output_name = file_name.split("/")[-1]

    with open(f"{output_name}.hpp", "w") as file:
        file.write(file_contents)


def main(argv: list[str]) -> int:
    if (len(argv) != 3):
        print("Invalid arguments", file=sys.stderr)
        return 1

    variable_name = argv[1]
    file_name = argv[2]

    try:
        _generate(variable_name, file_name)
    except Exception as err:
        print(f"An error occurred: {err}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv));
