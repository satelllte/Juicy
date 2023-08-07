# Juicy

A playground for audio plugin development with [JUCE framework](https://juce.com/).

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

**(3) [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html)**

ClangFormat is used to make sure the project has consistent code formatting.

To install it on MacOS, you can use:

```sh
brew install clang-format
```

To make sure it's installed run:

```sh
clang-format --version
```

**(4) JUCE [AudioPluginHost](https://github.com/juce-framework/JUCE/tree/master/extras/AudioPluginHost)**

AudioPluginHost is a JUCE application that can be used to test plugins.
It's located in `JUCE/extras/AudioPluginHost` directory, but you have to build it.

To do this on MacOS, open it in Xcode:

```sh
open JUCE/extras/AudioPluginHost/Builds/MacOSX/AudioPluginHost.xcodeproj
```

Then build it in Xcode.

To make sure it's built (MacOS), you can run:

```sh
test -d JUCE/extras/AudioPluginHost/Builds/MacOSX/build/Debug/AudioPluginHost.app && echo "+"
```

### Build & Run

After you've done all the steps above, you can build and run the project.

**(1) Create project for your IDE**

On Mac with Xcode:

```sh
mkdir build
cd build
cmake .. -G Xcode
```

**(2) Open project**

On Mac with Xcode:

```sh
open build/ProjectName.xcodeproj
```

Now you can select the target and build it. 

By default, it's going to run in `AudioPluginHost.app`, but if you want to test the plugin in some other application, e.g. in Ableton, you can go to "Product -> Scheme -> Edit Scheme" and choose another executable. But remember that it'll reset if you re-run the CMake build.

### Testing

**(1) Code formatting**

To check code formatting, run:

```sh
./test_format.sh
```
