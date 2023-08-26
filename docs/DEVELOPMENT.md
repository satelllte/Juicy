# Development

Development guide.

## Prerequisites

### (1) [JUCE](https://github.com/juce-framework/JUCE/) framework

JUCE source code is included as a Git submodule and located inside `./JUCE` directory.

To populate it after first repository clone, run:

```sh
git submodule update --init --recursive
```

To update JUCE source files to latest version, run:

```sh
pushd ./JUCE
git pull
popd
```

then commit the changes (if there are any):

```sh
git add ./JUCE
git commit -m "Update JUCE to latest"
```

### (2) [CMake](https://cmake.org/download/)

Download and install CMake.

On MacOS it can be done easily via [Homebrew](https://brew.sh/):

```sh
brew install cmake
```

To make sure it's installed run:

```sh
cmake --version
```

#### (3) [LLVM ClangFormat](https://clang.llvm.org/docs/ClangFormat.html)

ClangFormat is used to make sure the project has consistent code formatting.

On MacOS it can be installed easily via:

```sh
brew install clang-format
```

To make sure it's installed run:

```sh
clang-format --version
```

### (4) (Optional) [JUCE AudioPluginHost](https://github.com/juce-framework/JUCE/tree/master/extras/AudioPluginHost)

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

## Build & Run

After all the steps above done, the project can be built.

### (1) Create project for your IDE

This will create `./build` directory files by CMake:

| IDE / OS | CMake command | Notes |
| --- | --- | --- |
| Terminal (MacOS, Linux) | `cmake -DCMAKE_BUILD_TYPE=#BUILD_TYPE -B=./build -G="Unix Makefiles"` | `#BUILD_TYPE` is `Debug` or `Release` |
| Xcode (MacOS) | `cmake -DCMAKE_BUILD_TYPE=#BUILD_TYPE -B=./build -G=Xcode` | `#BUILD_TYPE` is `Debug` or `Release` |
| VSCode (MacOS, Linux) | - *(open project in VSCode)* | If all recommended extensions from `./.vscode` settings folder installed, VSCode will create build files for `Debug` build type automatically and plugin can be built from its UI |

### (2) Build plugin

| IDE / OS | Notes |
| --- | --- |
| Terminal (MacOS, Linux) | Use `cmake --build ./build --config #BUILD_TYPE --target #TARGET -j 10` command, where `#BUILD_TYPE` is `Debug` or `Release`, `#TARGET` is the target name in `ProjectName_Target` format (e.g., `Juicy_VST3`). |
| Xcode (MacOS) | Use Xcode UI for builds. Check out [Apple documentation](https://developer.apple.com/documentation/xcode/building-and-running-an-app) for more details. Also, note: by default, it's going to run the plugin in `AudioPluginHost.app`, but if you want to test the plugin in some other application (e.g., in Ableton), you can go to "Product -> Scheme -> Edit Scheme" and choose another executable. But remember that it'll reset if you re-run the CMake build. |
| VSCode (MacOS, Linux) | Use [VSCode CMake Tools extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) UI for builds. |

## Testing

### (1) Code formatting

To check source code formatting with ClangFormat, run:

```sh
./test_format.sh
```
