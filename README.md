# Juicy

JUCE framework playground

## Development

### Prerequisites

**(1) [JUCE](https://github.com/juce-framework/JUCE/)**

Update JUCE git submodule dependency to latest:

```sh
git submodule update --init --recursive
```

**(2) [CMake](https://cmake.org/download/)**

Download and install CMake.

If you're on MacOS it can be done via [Homebrew](https://brew.sh/):

```sh
brew install cmake
```

To make sure it's installed run:

```sh
cmake --version
```

### Build

On Mac:

```sh
mkdir build
cd build
cmake .. -GXcode
```
