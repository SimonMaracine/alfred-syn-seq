# Alfred

A free and open source cross-platform synthesizer and sequencer program.

It comprises a digital multi-timbral synthesizer using additive and wavetable methods, and an interface to compose and play music.

It is same instrument + pitch monophonic, otherwise it supports up to twelve voices simultaneously.

The synthesized sounds are meant to be *suggestions* and *hints*, rather than realistic emulations.

I built this program for myself, as a reason to try out the art of music composition. Of course MuseScore already exists, but isn't it nice to make your own tooling?

![Alfred](gfx/mascot/alfred.png)

Alfred is split into two:

- A synthesizer library worrying about synthesis-related things almost exclusively
- A sequencer GUI application which enables playing notes and music, and which loosely models some sheet music concepts

Currently, Alfred is in its infancy, so its current instruments are not well-made, and it is still missing some important features, which will probably be addressed with time:

- Plugins which dynamically add compiled instruments/presets
- A graphical way to create, store and load runtime instruments/presets
- Support for MIDI keyboard controllers
- Presenting errors nicely instead of just silently logging them

Alfred works with 16-bit samples, one channel, and frequency 44100 Hz.

Read the manual [here](MANUAL.md).

![Screenshot](gfx/screenshots/1.png)

## Building, Installing and Packaging

Alfred was at least built and tested on:

- Fedora Linux 43, GNOME 49, Wayland, PipeWire, GCC 15.2, CMake 3.31
- Ubuntu 25.04, GNOME 48, X11 (Xwayland), PipeWire, GCC 14.2, CMake 3.31
- Windows 11 25H2, WASAPI, MSVC 19.50, CMake 4.3

Additionally, it was at least tested on:

- ...

Alfred is solely 64-bit software.

Build in distribution/release mode for the best audio experience! Compiled without optimizations, due to low performance, the audio output can sometimes sound glitchy and bad!

### Requirements

- Git
- CMake
- GCC or MSVC with C++23 support
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
cmake --preset conf-dist-nix
cmake --build --preset build-dist-nix --target package_source

cmake --preset conf-dist-nix-rpm
cmake --build --preset build-dist-nix --target package

cmake --preset conf-dist-nix-deb
cmake --build --preset build-dist-nix --target package
```

#### Windows

```txt
cmake --preset conf-dist-win
cmake --build --preset build-dist-win --target package_source

cmake --preset conf-dist-win
cmake --build --preset build-dist-win --target package
```

## Dependencies

- [SDL](https://github.com/libsdl-org/SDL)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [Cereal](https://github.com/USCiLab/cereal)
- [FFTW](https://www.fftw.org/)

The licenses are found in their respective directories.

## Material and Acknowledgements

- PadSynth algorithm by Paul Nașca
- Sound Synthesis and Sampling Second Edition by Martin Russ
- THE COMPLETE SYNTHESIZER: A Comprehensive Guide by David Crombie
- Code-It-Yourself! Sound Synthesizer by javidx9

I give my thanks to my friend Tudor for sharing his knowledge of signal processing with me.

## Credits

One of the test compositions is the first section only from [I Secretly Love You](https://www.youtube.com/watch?v=7gYG95NG1YA) by Yuang Chen (Seycara Orchestral). 

---

This software, even though it's free and open source, is not meant to be consumed for LLM training!
