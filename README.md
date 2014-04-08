# Rainbow

Rainbow is a scriptable, cross-platform, 2D game engine written in C++.

Copyright &copy; 2010-14 Bifrost Entertainment AS and Tommy Nguyen.

Distributed under the MIT License (see accompanying file `LICENSE` or copy at
http://opensource.org/licenses/MIT). Licenses for third-party software can be
found in file `THIRDPARTY`.

## Features

### Platforms

- Android
- iOS
- Linux
- Mac OS X
- Windows

### Audio

Rainbow integrates [FMOD Studio](http://www.fmod.org/). Its API is available at
the scripting layer.

A custom solution based on OpenAL and
[OpenSL ES](http://www.khronos.org/opensles/) (Android-only) is also available,
with support for the following formats:

- Android: Any format supported by Android's
  [OpenSL ES](http://www.khronos.org/opensles/) implementation
- Linux/Mac OS X/Windows: [Ogg Vorbis](http://www.vorbis.com/)
- iOS/Mac OS X (additionally): Any format listed in the
  [Multimedia Programming Guide](https://developer.apple.com/library/ios/documentation/AudioVideo/Conceptual/MultimediaPG/UsingAudio/UsingAudio.html#//apple_ref/doc/uid/TP40009767-CH2-SW33)

### Graphics

Uses mostly [OpenGL ES 2.0](http://www.khronos.org/opengles/2_X/) compatible
features:

- Shaders
- Sprites-based rendering with implicit batching
- Text rendering (supports TrueType and OpenType through
  [FreeType](http://www.freetype.org/))
- Texture atlas ([PNG](http://www.libpng.org/pub/png/) and PVRTC)

### Input

- Accelerometer (Android and iOS)
- Keyboard and mouse (Linux/Mac OS X/Windows)
- Microphone (Android and iOS)
- Touch (Android and iOS)

### Others

- [Lua](http://www.lua.org/) scripting language with debugging console and hot
  reloading
- Physics ([Box2D](http://box2d.org/))
- Scene graph
- [TestFlight](https://testflightapp.com/) integration

## Structure

- `build` - Contains compilation-related files.
- `demos` - Demos.
- `doc` - Documentation.
- `lib` - Libraries necessary to compile Rainbow.
- `lua` - Convenience libraries for implementing apps or games.
- `src` - Rainbow source files.
- `tests` - Rainbow unit tests.
- `tools` - Tools such as the build script.

## Building

First of all, clone the repository:

	$ git clone --recursive https://bitbucket.org/tido/rainbow.git

The repository only includes some of the libraries required to build Rainbow.
The remaining requisites is listed on each platform's section. They must be
installed or copied into the repository before you can start building Rainbow.

Easiest way to build Rainbow is to use the provided build scripts:

	$ mkdir rainbow-build
	$ cd rainbow-build
	$ /path/to/rainbow/tools/build.sh [platform] [options]

Run `build.sh help` to get an overview of options and configurations. `platform`
can be omitted if compiling a native build (i.e. not cross-compiling).

If you have problems running `build.sh`, please make sure it has execution
permission:

	$ chmod +x /path/to/rainbow/tools/build.sh

Windows-users have to use `build.bat` instead. For iOS builds, see the
corresponding section.

### Building with FMOD Studio

The default audio backend for desktop builds is currently FMOD Studio.
[Download](http://www.fmod.org/download/) and extract/install it, then copy the
header files to `rainbow/lib/FMOD/inc/` and the libraries to
`rainbow/lib/FMOD/lib/`.

If you don't want to use FMOD Studio, run `build.sh` with `-DUSE_FMOD_STUDIO=0`.

### Android

- [Android NDK](http://developer.android.com/tools/sdk/ndk/)
- [Android SDK](http://developer.android.com/sdk/)

Once you've installed both, open `rainbow/tools/build-android.sh` and change
`NDK_HOME` to point at the NDK. The build script will create a debuggable and
installable APK but is currently only available on Linux/Mac OS X. You can still
compile for Android on Windows, it just requires some manual labour.

Follow the steps outlined earlier and specify `android` for `platform`:

	$ /path/to/rainbow/tools/build.sh android [options]

### iOS

- [Xcode](https://itunes.apple.com/no/app/xcode/id497799835?mt=12)

Use the provided Xcode project under `rainbow/build/xcode4/`.

### Linux (and cross-compiling for Windows)

- [Clang](http://clang.llvm.org/)
- [CMake](http://www.cmake.org/)
- [Mesa](http://www.mesa3d.org/)
- [Ogg Vorbis](http://www.vorbis.com/) (only with `-DUSE_FMOD_STUDIO=0`)
- [OpenAL Soft](http://kcat.strangesoft.net/openal.html) (only with
  `-DUSE_FMOD_STUDIO=0`)
- [SDL](http://libsdl.org/)
- [MinGW](http://www.mingw.org/) or
  [MinGW-w64](http://mingw-w64.sourceforge.net/) (optional)

You can install most of these using your favourite package manager. For SDL,
download the source code and extract its content to `rainbow/lib/SDL/`.

Clang is the default compiler but you can use [GCC](http://gcc.gnu.org/), or any
other compiler for that matter, by prefixing `CC=gcc CXX=g++`.

Windows builds can be built using MinGW or MinGW-w64 by issuing `windows` as
platform. However, this method hasn't been maintained in quite some time and may
no longer work.

### Mac OS X

- [CMake](http://www.cmake.org/)
- [Ogg Vorbis](http://www.vorbis.com/) (only with `-DUSE_FMOD_STUDIO=0`)
- [SDL](http://libsdl.org/)
- [Xcode](https://itunes.apple.com/no/app/xcode/id497799835?mt=12)

CMake and Ogg Vorbis can be installed through [Homebrew](http://brew.sh/) or
[MacPorts](http://www.macports.org/). Download SDL source code and extract its
content to `rainbow/lib/SDL/`. Xcode's Command Line Tools must be installed.

### Windows

- [CMake](http://www.cmake.org/)
- [OpenAL Soft](http://kcat.strangesoft.net/openal.html) (only with
  `-DUSE_FMOD_STUDIO=0`)
- [SDL](http://libsdl.org/)
- [Visual Studio Express for Windows Desktop](http://microsoft.com/express/)
  (2013 or later)

Download OpenAL Soft and place the files under `rainbow\build\windows\include\`
and `rainbow\build\windows\lib\`. Finally, download SDL development libraries
and move its content to `rainbow\lib\SDL\`.

Alternatively, see "Linux" for cross-compilation.

## Learning

See documentation under `doc`, and the demos under `demos`. Note: In order to
run a demo, you'll need to copy some files into the folder of the demo you want
to run:

- `lua/*` (you may not need all but it's easier to just copy everything)
- `src/Graphics/Shaders` (copy the folder, not needed for Android/iOS)

Some demos may use resources that (unfortunately) cannot be distributed with
Rainbow.

Once you've copied all the necessary files, run `rainbow` inside the folder:

	$ cd /path/to/demo
	$ rainbow

Or with the path as argument:

	$ rainbow /path/to/demo
