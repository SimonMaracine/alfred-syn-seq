# Alfred

A free and open source cross-platform synthesizer and sequencer program.

It comprises a digital multi-timbral synthesizer using additive and wavetable methods, and an interface to compose and play music.

It is same instrument + pitch monophonic, otherwise it supports up to eight voices simultaneously.

The synthesized sounds are meant to be *suggestions* and *hints*, rather than realistic emulations.

![Alfred](gfx/mascot/alfred.png)

## Building and Installing

### Requirements

- Git
- CMake
- GCC or MSVC (Visual Studio)
- Python

### Download

```txt
git clone https://github.com/SimonMaracine/alfred
git submodule update --init
```

### Build (Distribution)

#### Linux

```txt
cmake --preset conf-dist-nix
cmake --build --preset build-dist-nix
```

#### Windows

```txt
cmake --preset conf-dist-win
cmake --build --preset build-dist-win
```

### Install

```txt
cmake --install build/dist/
```

## Dependencies

- [SDL](https://github.com/libsdl-org/SDL)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [Cereal](https://github.com/USCiLab/cereal)
- [FFTW](https://www.fftw.org/)

## Material

- PadSynth algorithm by Paul Nașca
- Sound Synthesis and Sampling Second Edition by Martin Russ
- THE COMPLETE SYNTHESIZER: A Comprehensive Guide by David Crombie
- Code-It-Yourself! Sound Synthesizer by javidx9
