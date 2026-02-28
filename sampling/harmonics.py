import sys
import pprint


def _db_to_amplitude(power: int) -> float:
    return 10.0 ** (power / 20.0)


def _process_inputs(args: list[str]) -> tuple[float, float, str]:
    if len(args) == 1:
        time = float(input("Time: "))
        frequency_fundamental = float(input("Frequency fundamental: "))
        harmonics_file = input("Harmonics file: ")
    else:
        time = float(args[1])
        frequency_fundamental = float(args[2])
        harmonics_file = args[3]

    return time, frequency_fundamental, harmonics_file


def _process_harmonics(frequency_fundamental: float, harmonics_file: str) -> list[tuple[float, float]]:
    with open(harmonics_file, "r") as file:
        lines = file.readlines()

    harmonics = []

    for line in lines:
        line = line.lstrip()

        if not line:
            continue

        if line[0] == "#":
            continue

        frequency, power = line.split(",")

        harmonic = (
            float(frequency) / frequency_fundamental,
            _db_to_amplitude(0 - int(power))
        )

        harmonics.append(harmonic)

    return harmonics


def _main(args: list[str]) -> int:
    try:
        time, frequency_fundamental, harmonics_file = _process_inputs(args)
        harmonics = _process_harmonics(frequency_fundamental, harmonics_file)

        with open(f"harmonic_{time}.txt", "w") as file:
            print("Time:", time, file=file)
            print("Frequency fundamental:", frequency_fundamental, file=file)
            print(file=file)
            for harmonic in harmonics:
                print(round(harmonic[0], 4), round(harmonic[1], 4), file=file)

        pprint.pprint(harmonics)
    except KeyboardInterrupt:
        return 1
    except Exception as e:
        print(e, file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(_main(sys.argv))
