#! /usr/bin/env python3

import sys
import os


def _contents(version_string: str) -> str:
    return f"""
#pragma once

inline constexpr const char* ALFRED_VERSION = "{version_string}";
"""


def _generate(version_string: str):
    try:
        os.mkdir("include")
    except Exception as err:
        print(err, file=sys.stderr)

    with open(f"include/version.hpp", "w") as file:
        file.write(_contents(version_string))


def main(argv: list[str]) -> int:
    if len(argv) != 2:
        print("Invalid arguments", file=sys.stderr)
        return 1

    version_string = argv[1]

    try:
        _generate(version_string)
    except Exception as err:
        print(f"An error occurred: {err}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
