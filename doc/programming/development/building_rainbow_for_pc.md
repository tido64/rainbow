# Building Rainbow for PC

## Getting the source code

First, make sure you've installed [Git](http://git-scm.com/downloads) as that's what we'll be using to retrieve the source code. The following command will download Rainbow and all of its submodules into a folder named `rainbow`:

```bash
git clone --recursive https://bitbucket.org/tido/rainbow.git
```

If you're using a graphical UI for Git, make sure to checkout the submodules as well.

## Setting up

Next, we'll need to install the dependencies:

* [CMake](http://www.cmake.org/)
* [FMOD](http://www.fmod.org/) (only if you want to use FMOD Studio)
* [Mercurial](http://mercurial.selenic.com/) (Linux and Mac OS X only)
* [Ogg Vorbis](http://www.vorbis.com/) (only if you want to use OpenAL)
* [OpenAL Soft](http://kcat.strangesoft.net/openal.html) (only if you want to use OpenAL)
* [SDL](https://www.libsdl.org/download-2.0.php) (Windows only)

If you've decided to use FMOD Studio, download the FMOD Studio API package for
your platform. Extract or install it, and copy the files into place. Please see
the instructions in `README.md` for more details.

### Linux

1. Use your distro's package manager to install the dependencies:

|               |                                                                                     |
|---------------|-------------------------------------------------------------------------------------|
| Arch Linux    | `pacman -S --needed cmake gcc libvorbis mercurial mesa openal`                    |
| Debian/Ubuntu | `apt-get install build-essential cmake libopenal-dev libvorbis-dev mercurial mesa-common-dev` |

> **Note**: The correct version of SDL will be automatically downloaded by the build script.

### Mac OS X

1. Download and install [Xcode](https://itunes.apple.com/app/xcode/id497799835?mt=12) from the App Store.
2. Install the Command Line Tools Package by running `xcode-select --install` in Terminal.
3. Install [Homebrew](http://brew.sh/) (or [MacPorts](http://www.macports.org/)).
4. Install the rest of the dependencies: `brew install cmake libogg libvorbis mercurial`

> **Note**: The correct version of SDL will be automatically downloaded by the build script.

### Windows

1. Download and install [Visual Studio Community](http://www.visualstudio.com/en-us/products/visual-studio-community-vs).
2. Download and install [CMake](http://www.cmake.org/download/).
3. Download [SDL development libraries](https://www.libsdl.org/download-2.0.php) for Visual C++ and extract its content to `lib\SDL` in the repository.
4. **Optional**: Download [OpenAL Soft](http://kcat.strangesoft.net/openal.html) and extract it. Copy includes to `build\windows\include` in the repository, and libraries to `build\windows\lib`.

> **Note**: If you're on Visual Studio 2015 or newer, the PowerShell script will automatically download OpenAL Soft and SDL for you.

## Compiling Rainbow

### Configuration

Available features and build configurations:

| Build configuration | Description |
|:--------------------|:------------|
| `Debug`             | Compiles Rainbow in debug mode with little or no optimizations. |
| `Release`           | Compiles Rainbow in release mode with most optimizations turned on. |
| `RelWithDebInfo`    | Similar to `Release` but with debugging symbols. |
| `MinSizeRel`        | Similar to `Release` with an extra effort to make the binary small. |

| Feature flag      | Description |
|:------------------|:------------|
| `UNIT_TESTS`      | Compiles unit tests. Only useful for engine developers. |
| `USE_FMOD_STUDIO` | Replaces Rainbow's custom audio engine with FMOD Studio. |
| `USE_HEIMDALL`    | Compiles in Rainbow's debug overlay and other debugging facilities. |
| `USE_PHYSICS`     | Compiles in Box2D and its Lua wrappers. |
| `USE_SPINE`       | Enables support for loading Spine rigs. |
| `USE_VECTOR`      | Compiles in NanoVG for vector drawing capabilities. |

Example: Build Rainbow with physics and Spine support for game development.

```bash
/path/to/rainbow/tools/build.sh -DCMAKE_BUILD_TYPE=Debug \
                                -DUSE_PHYSICS=1 \
                                -DUSE_SPINE=1
```

You can pass any number of CMake options. For platform specific options, see the corresponding sections.

### Linux / Mac OS X

Rainbow must be built outside of the source tree. So you'll need to first make a directory for building Rainbow, then run the build script:

```bash
mkdir rainbow-build
cd rainbow-build
/path/to/rainbow/tools/build.sh -DCMAKE_BUILD_TYPE=<configuration> [option ...]
```

The build script will generate project files and start building Rainbow.

By default, [GCC](https://gcc.gnu.org/) and Clang will be used to compile both C and C++ code on Linux and OS X respectively. Unix Makefiles is the default generator on Linux, and Xcode on OS X. You can change these by prefixing environment variables. For instance, to use GCC and [Ninja](https://martine.github.io/ninja/) in place of Clang and Makefiles:

```bash
CC=gcc CXX=g++ GENERATOR=Ninja /path/to/rainbow/tools/build.sh [option ...]
```

If you have problems running `build.sh`, make sure it has execution permission:

```bash
chmod +x /path/to/rainbow/tools/build.sh
```

### Windows

The PowerShell script `tools\make.ps1` will walk you through customising your Rainbow project. Find it using File Explorer, right-click the script and select _Run with PowerShell_. It will also automatically download OpenAL Soft and SDL for you.

If you're still on Visual Studio 2013, use the batch script to generate project files.

```bat
\path\to\rainbow\tools\build.bat -DCMAKE_BUILD_TYPE=<configuration> [option ...]
```
