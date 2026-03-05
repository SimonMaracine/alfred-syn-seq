#! /usr/bin/env python3

import sys
import dataclasses


@dataclasses.dataclass(slots=True, frozen=True)
class Analysis:
    time: float
    partials: list[tuple[float, float]]


def _db_to_amplitude(power: int) -> float:
    return 10.0 ** (power / 20.0)


def _read_analysis(lines: list[str], line_index: int) -> Analysis | None:
    if line_index == len(lines):
        return None

    time = float(lines[line_index])
    line_index += 1

    if line_index == len(lines):
        return None

    frequency_fundamental = float(lines[line_index])
    line_index += 1

    if line_index == len(lines):
        return None

    amplitude_fundamental = int(lines[line_index])
    line_index += 1

    partials = [(1.0, _db_to_amplitude(-int(amplitude_fundamental)))]

    while True:
        if line_index == len(lines):
            return None

        line = lines[line_index].strip()

        if not line:
            line_index += 1
            continue

        if line == "ANALYSIS":
            break

        # Increment only after check
        line_index += 1

        frequency, power = line.split()

        partial = (float(frequency) / frequency_fundamental, _db_to_amplitude(-int(power)))
        partials.append(partial)

    return Analysis(time, partials)


def _read_partials_file(lines: list[str]) -> list[Analysis]:
    line_index = 0
    analysis_list = []

    while True:
        line = lines[line_index].strip()

        if line == "ANALYSIS":
            line_index += 1
            analysis = _read_analysis(lines, line_index)

            if analysis is None:
                break

            analysis_list.append(analysis)

        line_index += 1

    return analysis_list


def _main(args: list[str]) -> int:
    try:
        note = args[1]
        partials_file = args[2]

        with open(partials_file, "r") as file:
            lines = file.readlines()

        analysis_list = _read_partials_file(lines)

        with open(f"partials_{note}.txt", "w") as file:
            for analysis in analysis_list:
                print(analysis, file=file)
                print(file=file)

        for analysis in analysis_list:
            # Don't forget the fundamental
            partials = [(1.0, 1.0)] + analysis.partials

            print(
f"""syn::util::SoundAtTime {{
    {analysis.time},
    [](double time, syn::NoteId note) {{
        return syn::util::sound(
            time,
            note,
            std::array {{ {", ".join(map(str, (round(partial[0], 1) for partial in partials)))} }},
            std::array {{ {", ".join(map(str, (round(partial[1], 4) for partial in partials)))} }}
        );
    }}
}},"""
            )
    except KeyboardInterrupt:
        return 1
    except Exception as e:
        print(e, file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(_main(sys.argv))
