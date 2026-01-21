# Alfred

A free and open source synthesizer and sequencer program.

## Building

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

### Build for Distribution

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
