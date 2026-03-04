# Alfred

A free and open source cross-platform synthesizer and sequencer program.

It comprises a digital multi-timbral synthesizer using additive and wavetable methods, and an interface to compose and play music.

It is same instrument + pitch monophonic, otherwise it supports up to eight voices simultaneously.

The synthesized sounds are meant to be *suggestions* and *hints*, rather than realistic emulations.

I built this program for myself, as a reason to try out the art of music composition. Of course MuseScore already exists, but isn't it nice to make your own tooling?

![Alfred](gfx/mascot/alfred.png)

## Building, Installing and Packaging

### Requirements

- Git
- CMake
- GCC or MSVC (Visual Studio)
- Python
- NSIS (Windows only)

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

#### Linux

```txt
cmake --build --preset build-dist-nix --target install
```

#### Windows

```txt
cmake --build --preset build-dist-win --target install
```

### Package

#### Linux

```txt
cmake --build --preset build-dist-nix --target package_source
cmake --build --preset pack-nix-rpm --target package
cmake --build --preset pack-nix-deb --target package
```

#### Windows

```txt
cmake --build --preset build-dist-win --target package_source
cmake --build --preset build-dist-win --target package
```

## Dependencies

- [SDL](https://github.com/libsdl-org/SDL)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [Cereal](https://github.com/USCiLab/cereal)
- [FFTW](https://www.fftw.org/)

## Material and Acknowledgements

- PadSynth algorithm by Paul Nașca
- Sound Synthesis and Sampling Second Edition by Martin Russ
- THE COMPLETE SYNTHESIZER: A Comprehensive Guide by David Crombie
- Code-It-Yourself! Sound Synthesizer by javidx9

I give my thanks to my friend Tudor for sharing his knowledge of signal processing with me.

---

This software, even though it's free and open source, is not meant to be used for LLM training!
