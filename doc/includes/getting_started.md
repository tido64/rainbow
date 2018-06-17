# Getting Started

## Download the Source Code

First, make sure you've installed [Git](https://git-scm.com/downloads) as that's
what we'll be using to retrieve the source code. The following command will
download Rainbow and all of its submodules into a folder named `rainbow`:

`git clone --recursive https://bitbucket.org/tido/rainbow.git`

If you're using a graphical UI for Git, make sure to checkout the submodules as
well.

## Building for Windows

1. Install [Visual Studio Community](https://www.visualstudio.com/vs/community/).

2. Install [CMake](https://cmake.org/download/).

3. **Optional:** Download FMOD Studio API, and extract it. Copy the headers into
   `lib/FMOD/inc/`, and binaries into `lib/FMOD/lib/windows/`. You should end up
   with the following:

    ```shell
    rainbow/lib/FMOD
    ├── inc
    │   ├── fmod.h
    │   ├── fmod.hpp
    │   ├── fmod_codec.h
    │   ├── fmod_common.h
    │   ├── fmod_dsp.h
    │   ├── fmod_dsp_effects.h
    │   ├── fmod_errors.h
    │   ├── fmod_output.h
    │   ├── fmod_studio.h
    │   ├── fmod_studio.hpp
    │   └── fmod_studio_common.h
    └── lib
        └── windows
            ├── fmod.dll
            ├── fmod_vc.lib
            ├── fmod64.dll
            ├── fmod64_vc.lib
            ├── fmodstudio.dll
            ├── fmodstudio_vc.lib
            ├── fmodstudio64.dll
            └── fmodstudio64_vc.lib
    ```

4. The PowerShell script `tools\make.ps1` will walk you through customising your
   Rainbow project. Find it using File Explorer, right-click the file, and
   select _Run with PowerShell_. You can dismiss any prompts about execution
   policy.

5. When Visual Studio opens the project, simply hit `F5` to build and run.

## Building for macOS

1. Install [Xcode](https://itunes.apple.com/app/xcode/id497799835?mt=12) from App Store.

2. Once Xcode is installed, launch it once to install Command Line Tools.

3. Install [Homebrew](https://brew.sh/).

4. Install build dependencies from the root of the repo: `brew bundle`

5. **Optional:** Download FMOD Studio API, extract it, and point
   `tools/fmod-import-tool.sh` at the directory you extracted it to. This should
   copy all the necessary files into the `lib` folder.

6. Make a directory for building Rainbow, then run the build script:

    ```shell
    mkdir rainbow-build
    cd rainbow-build
    /path/to/rainbow/tools/build.sh -DCMAKE_BUILD_TYPE=<configuration> [option ...]
    ```

7. Start the playground:

    ```shell
    ./rainbow /path/to/rainbow/js
    ```

You'll find an overview of configuration and feature flags further below.

## Building for Linux

1. Use your distro's package manager to install the dependencies:

    |               |                                                          |
    |---------------|----------------------------------------------------------|
    | Arch Linux    | `pacman -S --needed cmake gcc libvorbis mesa pulseaudio sdl2` |
    | Debian/Ubuntu | `apt install build-essential cmake libgl1-mesa-dev libpulse-dev libsdl2-dev libvorbis-dev pulseaudio` |

2. **Optional:** Download FMOD Studio API, extract it, and point
   `tools/fmod-import-tool.sh` at the directory you extracted it to. This should
   copy all the necessary files into the `lib` folder.

3. Make a directory for building Rainbow, then run the build script:

    ```shell
    mkdir rainbow-build
    cd rainbow-build
    /path/to/rainbow/tools/build.sh -DCMAKE_BUILD_TYPE=<configuration> [option ...]
    ```

4. Start the playground:

    ```shell
    ./rainbow /path/to/rainbow/js
    ```

You'll find an overview of configuration and feature flags further below.

## Building for Android

1. Install [Android Studio](https://developer.android.com/studio/).

2. Install [CMake](https://cmake.org/).

3. Download FMOD Studio API, and extract it. Copy the headers into
   `lib/FMOD/inc/`, and binaries into `lib/FMOD/lib/android/`. You should end up
   with the following:

    ```shell
    rainbow/lib/FMOD
    ├── inc
    │   ├── fmod.h
    │   ├── fmod.hpp
    │   ├── fmod_codec.h
    │   ├── fmod_common.h
    │   ├── fmod_dsp.h
    │   ├── fmod_dsp_effects.h
    │   ├── fmod_errors.h
    │   ├── fmod_output.h
    │   ├── fmod_studio.h
    │   ├── fmod_studio.hpp
    │   └── fmod_studio_common.h
    └── lib
        └── android
            ├── arm64-v8a
            │   ├── libfmod.so
            │   └── libfmodstudio.so
            ├── armeabi
            │   ├── libfmod.so
            │   └── libfmodstudio.so
            ├── armeabi-v7a
            │   ├── libfmod.so
            │   └── libfmodstudio.so
            ├── fmod.jar
            └── x86
                ├── libfmod.so
                └── libfmodstudio.so
    ```


4. Make a directory for the Gradle project, then run the build script:

    ```shell
    mkdir rainbow-build
    cd rainbow-build
    /path/to/rainbow/tools/build.sh android [option ...]
    ```

5. Import the Gradle project in Android Studio.

6. Run the app on your device or emulator.

## Building for iOS

1. Install [Xcode](https://itunes.apple.com/no/app/xcode/id497799835?mt=12) from the App Store.
2. Open `build/ios/Rainbow.xcodeproj` in Xcode.
3. Build and run the current scheme.

## Build Configuration

Available features and build configurations:

| Build configuration | Description                                                         |
|:--------------------|:--------------------------------------------------------------------|
| `Debug`             | Compiles Rainbow in debug mode with little or no optimizations.     |
| `Release`           | Compiles Rainbow in release mode with most optimizations turned on. |
| `RelWithDebInfo`    | Similar to `Release` but with debugging symbols.                    |
| `MinSizeRel`        | Similar to `Release` with emphasis on small binary size.            |

| Feature flag      | Description                                                         |
|:------------------|:--------------------------------------------------------------------|
| `UNIT_TESTS`      | Compiles unit tests. Only useful for engine developers.             |
| `USE_FMOD_STUDIO` | Replaces Rainbow's custom audio engine with FMOD Studio.            |
| `USE_HEIMDALL`    | Compiles in Rainbow's debug overlay and other debugging facilities. |
| `USE_PHYSICS`     | Compiles in Box2D.                                                  |
| `USE_SPINE`       | Enables support for loading Spine rigs.                             |

Example: Build Rainbow with physics and Spine support for game development.

```shell
/path/to/rainbow/tools/build.sh -DCMAKE_BUILD_TYPE=Debug \
                                -DUSE_HEIMDALL=1 \
                                -DUSE_PHYSICS=1 \
                                -DUSE_SPINE=1
```

You can pass any number of CMake options.

### Specifying Compiler

By default, [GCC](https://gcc.gnu.org/) and [Clang](https://clang.llvm.org/)
will be used to compile both C and C++ code on Linux and macOS respectively.
Unix Makefiles is the default generator on Linux, and Xcode on macOS. You can
change these by prefixing environment variables. For instance, to use GCC and
[Ninja](https://ninja-build.org/) in place of Clang and Makefiles:

`CC=gcc CXX=g++ GENERATOR=Ninja /path/to/rainbow/tools/build.sh [option ...]`

If you have problems running `build.sh`, make sure it has execution permission:

`chmod +x /path/to/rainbow/tools/build.sh`
