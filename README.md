# Rainbow [![ReadTheDocs](https://readthedocs.org/projects/rainbow/badge/)](http://rainbow.rtfd.org/) [![Windows build status](https://ci.appveyor.com/api/projects/status/oajtxwu9d9lrayk0?svg=true)](https://ci.appveyor.com/project/Tommy/rainbow)

Rainbow is a fast, scriptable, cross-platform, 2D game engine written in modern
C++.

Copyright &copy; 2010-15 Bifrost Entertainment AS and Tommy Nguyen.

Distributed under the MIT License (see accompanying file `LICENSE` or copy at
http://opensource.org/licenses/MIT). Licenses for third-party software can be
found in file `THIRDPARTY`.

## Features

### Cross-platform

Rainbow runs on most popular operating systems, including:

- Android
- iOS
- Linux
- Mac OS X
- Windows

### Audio

Rainbow integrates [FMOD Studio](http://www.fmod.org/), giving you access to
the same professional tools that AAA studios use. There is also an open source
alternative built on OpenAL and
[OpenSL ES](http://www.khronos.org/opensles/) (Android). Audio format support
depends on the platform. Typically, MP3 and Ogg Vorbis on Android, and
[AAC, ALAC, and MP3](https://developer.apple.com/library/ios/documentation/AudioVideo/Conceptual/MultimediaPG/UsingAudio/UsingAudio.html#//apple_ref/doc/uid/TP40009767-CH2-SW33)
on iOS and OS X.

### Graphics

Rainbow is mostly built on [OpenGL ES 2.0](http://www.khronos.org/opengles/2_X/)
and features:

- Scene graph
- Sprites-based rendering with explicit batching
- Animation with traditional sprite sheets, or…
- Skeletal animation imported directly from
  [Spine](http://esotericsoftware.com/)
- Text rendering (supports TrueType and OpenType via
  [FreeType](http://www.freetype.org/))
- Texture atlas ([PNG](http://www.libpng.org/pub/png/) and PVRTC)

### Input

- Accelerometer (Android and iOS)
- Keyboard and mouse (Linux/Mac OS X/Windows)
- Microphone (Android and iOS)
- Touch (Android and iOS)

### And more…

- [Lua](http://www.lua.org/) scripting language with debugging console and hot
  reloading
- 2d rigid body physics provided by [Box2D](http://box2d.org/)
- Deploy and test with [TestFlight](https://testflightapp.com/) on iOS

## Structure

	build/  # Build related files
	demos/  # Lua demos
	doc/    # Documentation
	lib/    # Third party libraries Rainbow depend on
	lua/    # Lua convenience libraries
	src/    # Rainbow source code
	tools/  # Tools such as the build script

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

The default audio backend for all platforms is currently FMOD Studio. If you
don't want to use FMOD Studio, run `build.sh` with `-DUSE_FMOD_STUDIO=0` or
undefine `USE_FMOD_STUDIO` in the project file.

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
download the source code and extract its content to `rainbow/lib/SDL/`. If you
have installed Mercurial, let the build script download it for you.

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
[MacPorts](http://www.macports.org/). Xcode's Command Line Tools must be
installed. SDL will be downloaded automatically by the build script.

### Windows

- [CMake](http://www.cmake.org/)
- [OpenAL Soft](http://kcat.strangesoft.net/openal.html) (only with
  `-DUSE_FMOD_STUDIO=0`)
- [SDL](http://libsdl.org/)
- [Visual Studio Community](http://www.visualstudio.com/en-us/products/visual-studio-community-vs)
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
- `src/Graphics/Shaders` (copy the folder; not needed for Android/iOS)

Some demos may use resources that (unfortunately) cannot be distributed with
Rainbow.

Once you've copied all the necessary files, run `rainbow` inside the folder:

	$ cd /path/to/demo
	$ rainbow

Or with the path as argument:

	$ rainbow /path/to/demo
