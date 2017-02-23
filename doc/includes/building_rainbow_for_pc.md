# Building Rainbow for PC

## Getting the source code

First, make sure you've installed [Git](https://git-scm.com/downloads) as that's
what we'll be using to retrieve the source code. The following command will
download Rainbow and all of its submodules into a folder named `rainbow`:

```shell
git clone --recursive https://bitbucket.org/tido/rainbow.git
```

If you're using a graphical UI for Git, make sure to checkout the submodules as
well.

## Setting up

Next, we'll need to install the dependencies:

* [CMake](https://cmake.org/)
* [FMOD](https://www.fmod.org/) (**optional:** only if you want to use FMOD Studio)
* [Ogg Vorbis](https://www.xiph.org/vorbis/) (**optional:** only if you want to use OpenAL)
* [OpenAL Soft](http://kcat.strangesoft.net/openal.html) (**optional:** only if you want to use OpenAL)
* [SDL](https://libsdl.org/)

If you've decided to use FMOD Studio, download the FMOD Studio API package for
your platform. You will need to extract or install it, and copy the files into
place. Please see the instructions for your platform below.

On Linux or macOS, `tools/fmod-import-tool.sh` will help you copy the files into
place.

### Linux

1. Use your distro's package manager to install the dependencies:

|               |                                                                                     |
|---------------|-------------------------------------------------------------------------------------|
| Arch Linux    | `pacman -S --needed cmake gcc libvorbis mesa openal sdl2` |
| Debian/Ubuntu | `apt-get install build-essential cmake libgl1-mesa-dev libopenal-dev libsdl2-dev libvorbis-dev` |

2. **Optional:** Copy FMOD headers into `lib/FMOD/inc/`, and binaries into `lib/FMOD/lib/linux/`:

```bash
lib/FMOD/inc/fmod.h
lib/FMOD/inc/fmod.hpp
lib/FMOD/inc/fmod_codec.h
lib/FMOD/inc/fmod_common.h
lib/FMOD/inc/fmod_dsp.h
lib/FMOD/inc/fmod_dsp_effects.h
lib/FMOD/inc/fmod_errors.h
lib/FMOD/inc/fmod_output.h
lib/FMOD/inc/fmod_studio.h
lib/FMOD/inc/fmod_studio.hpp
lib/FMOD/inc/fmod_studio_common.h
lib/FMOD/lib/linux/x86/libfmod.so
lib/FMOD/lib/linux/x86/libfmod.so.5
lib/FMOD/lib/linux/x86/libfmod.so.5.14
lib/FMOD/lib/linux/x86/libfmodstudio.so
lib/FMOD/lib/linux/x86/libfmodstudio.so.5
lib/FMOD/lib/linux/x86/libfmodstudio.so.5.14
lib/FMOD/lib/linux/x86_64/libfmod.so
lib/FMOD/lib/linux/x86_64/libfmod.so.5
lib/FMOD/lib/linux/x86_64/libfmod.so.5.14
lib/FMOD/lib/linux/x86_64/libfmodstudio.so
lib/FMOD/lib/linux/x86_64/libfmodstudio.so.5
lib/FMOD/lib/linux/x86_64/libfmodstudio.so.5.14
```

### macOS

1. Download and install [Xcode](https://itunes.apple.com/app/xcode/id497799835?mt=12) from the App Store.
2. Install the Command Line Tools Package by running `xcode-select --install` in Terminal.
3. Install [Homebrew](http://brew.sh/) (or [MacPorts](https://www.macports.org/)).
4. Install the rest of the dependencies: `brew install cmake libogg libvorbis pkg-config sdl2`
5. **Optional:** Copy FMOD headers into `lib/FMOD/inc/`, and binaries into `lib/FMOD/lib/mac/`:

```bash
lib/FMOD/inc/fmod.h
lib/FMOD/inc/fmod.hpp
lib/FMOD/inc/fmod_codec.h
lib/FMOD/inc/fmod_common.h
lib/FMOD/inc/fmod_dsp.h
lib/FMOD/inc/fmod_dsp_effects.h
lib/FMOD/inc/fmod_errors.h
lib/FMOD/inc/fmod_output.h
lib/FMOD/inc/fmod_studio.h
lib/FMOD/inc/fmod_studio.hpp
lib/FMOD/inc/fmod_studio_common.h
lib/FMOD/lib/mac/libfmod.dylib
lib/FMOD/lib/mac/libfmodstudio.dylib
```

### Windows

1. Download and install [Visual Studio Community](https://www.visualstudio.com/vs/community/).
2. Download and install [CMake](https://cmake.org/download/).
3. **Optional:** Copy FMOD headers into `lib/FMOD/inc/`, and binaries into `lib/FMOD/lib/windows/`:

```bash
lib/FMOD/inc/fmod.h
lib/FMOD/inc/fmod.hpp
lib/FMOD/inc/fmod_codec.h
lib/FMOD/inc/fmod_common.h
lib/FMOD/inc/fmod_dsp.h
lib/FMOD/inc/fmod_dsp_effects.h
lib/FMOD/inc/fmod_errors.h
lib/FMOD/inc/fmod_output.h
lib/FMOD/inc/fmod_studio.h
lib/FMOD/inc/fmod_studio.hpp
lib/FMOD/inc/fmod_studio_common.h
lib/FMOD/lib/windows/fmod.dll
lib/FMOD/lib/windows/fmod64.dll
lib/FMOD/lib/windows/fmod64_vc.lib
lib/FMOD/lib/windows/fmod_vc.lib
lib/FMOD/lib/windows/fmodstudio.dll
lib/FMOD/lib/windows/fmodstudio64.dll
lib/FMOD/lib/windows/fmodstudio64_vc.lib
lib/FMOD/lib/windows/fmodstudio_vc.lib
```

<aside class="notice">The build scripts will automatically download and set up OpenAL Soft and SDL for you.</aside>

## Compiling Rainbow

### Configuration

Available features and build configurations:

| Build configuration | Description                                                         |
|:--------------------|:--------------------------------------------------------------------|
| `Debug`             | Compiles Rainbow in debug mode with little or no optimizations.     |
| `Release`           | Compiles Rainbow in release mode with most optimizations turned on. |
| `RelWithDebInfo`    | Similar to `Release` but with debugging symbols.                    |
| `MinSizeRel`        | Similar to `Release` with an extra effort to make the binary small. |

| Feature flag      | Description                                                                   |
|:------------------|:------------------------------------------------------------------------------|
| `SCRIPTING`       | Scripting language: `C++` or `Lua`. This is set to Lua if omitted.            |
| `UNIT_TESTS`      | Compiles unit tests. Only useful for engine developers.                       |
| `USE_FMOD_STUDIO` | Replaces Rainbow's custom audio engine with FMOD Studio.                      |
| `USE_HEIMDALL`    | Compiles in Rainbow's debug overlay and other debugging facilities.           |
| `USE_PHYSICS`     | Compiles in Box2D and its Lua wrappers.                                       |
| `USE_SPINE`       | Enables support for loading Spine rigs.                                       |

Example: Build Rainbow with physics and Spine support for game development.

```shell
/path/to/rainbow/tools/build.sh -DCMAKE_BUILD_TYPE=Debug \
                                -DUSE_PHYSICS=1 \
                                -DUSE_SPINE=1
```

You can pass any number of CMake options. For platform specific options, see the
corresponding sections.

### Linux / macOS

Rainbow must be built outside of the source tree. So you'll need to first make a
directory for building Rainbow, then run the build script:

```shell
mkdir rainbow-build
cd rainbow-build
/path/to/rainbow/tools/build.sh -DCMAKE_BUILD_TYPE=<configuration> [option ...]
```

The build script will generate project files and start building Rainbow.

By default, [GCC](https://gcc.gnu.org/) and Clang will be used to compile both C
and C++ code on Linux and macOS respectively. Unix Makefiles is the default
generator on Linux, and Xcode on macOS. You can change these by prefixing
environment variables. For instance, to use GCC and
[Ninja](https://ninja-build.org/) in place of Clang and Makefiles:

```shell
CC=gcc CXX=g++ GENERATOR=Ninja /path/to/rainbow/tools/build.sh [option ...]
```

If you have problems running `build.sh`, make sure it has execution permission:

```shell
chmod +x /path/to/rainbow/tools/build.sh
```

### Windows

The PowerShell script `tools\make.ps1` will walk you through customising your
Rainbow project. Find it using File Explorer, right-click the file, and select
_Run with PowerShell_. You can dismiss any prompts about execution policy.
