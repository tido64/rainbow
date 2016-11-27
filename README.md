# Rainbow [![codecov](https://codecov.io/gh/tn0502/rainbow/branch/master/graph/badge.svg)](https://codecov.io/gh/tn0502/rainbow)

Rainbow is a fast, scriptable, cross-platform, 2D game engine written in modern
C++.

Copyright &copy; 2010-16 Bifrost Entertainment AS and Tommy Nguyen.

Distributed under the MIT License (see accompanying file `LICENSE` or copy at
https://opensource.org/licenses/MIT). Licenses for third-party software can be
found in file `THIRDPARTY`.

- [Documentation](https://tido.bitbucket.io/rainbow/)
- [Roadmap](https://trello.com/b/r2TqudY6/rainbow)
- [Discussion](https://groups.google.com/forum/#!forum/rainbow-tech)

## Features

### Cross-platform

Rainbow runs on most popular operating systems:

| Android | iOS | Linux | macOS | Windows |
|:-------:|:---:|:-----:|:-----:|:-------:|
| ✓ | ✓ | [![Linux build Status](https://travis-ci.org/tn0502/rainbow.svg?branch=master)](https://travis-ci.org/tn0502/rainbow) | ✓ | [![Windows build status](https://ci.appveyor.com/api/projects/status/oajtxwu9d9lrayk0?svg=true)](https://ci.appveyor.com/project/Tommy/rainbow) |

### Audio

Rainbow integrates [FMOD Studio](https://www.fmod.org/), giving you access to
the same professional tools that AAA studios use. There is also an open source
alternative built on OpenAL. Audio format support depends on the platform.
Typically, MP3 and Ogg Vorbis on Android, and
[AAC, ALAC, and MP3](https://developer.apple.com/library/ios/documentation/AudioVideo/Conceptual/MultimediaPG/UsingAudio/UsingAudio.html#//apple_ref/doc/uid/TP40009767-CH2-SW33)
on iOS and macOS.

### Graphics

Rainbow is mostly built on
[OpenGL ES 2.0](https://www.khronos.org/opengles/2_X/) and features:

- Sprites-based rendering with explicit batching
- Animation with traditional sprite sheets, or…
- Skeletal animation imported directly from
  [Spine](http://esotericsoftware.com/)
- Text rendering (supports TrueType and OpenType via
  [FreeType](https://freetype.org/))
- Texture atlas
  ([DDS](https://msdn.microsoft.com/en-us/library/windows/desktop/bb943991(v=vs.85).aspx),
   [PNG](http://libpng.org/pub/png/libpng.html),
   [PVRTC](https://community.imgtec.com/developers/powervr/tools/pvrtextool/),
   and [SVG](https://www.w3.org/Graphics/SVG/))

### Input

- Accelerometer (Android and iOS)
- Keyboard and mouse (Linux/macOS/Windows)
- Microphone (Android and iOS)
- Touch (Android and iOS)

### And more…

- [Lua](https://www.lua.org/) scripting language with debugging console and hot
  reloading
- 2d rigid body physics provided by [Box2D](http://box2d.org/)

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
The remaining requisites are listed in each platform's section below. They must
be installed or copied into the repository before you can start building
Rainbow.

Easiest way to build Rainbow is to use the provided build scripts:

	$ mkdir rainbow-build
	$ cd rainbow-build
	$ /path/to/rainbow/tools/build.sh [platform] [options]

Run `build.sh help` to get an overview of options and configurations. `platform`
can be omitted if compiling a native build (i.e. not cross-compiling).

If you have problems running `build.sh`, please make sure it has execution
permission:

	$ chmod +x /path/to/rainbow/tools/build.sh

For Windows developers, run the PowerShell script `tools\make.ps1` (right-click
on it and select _Run with PowerShell_). It will take you through the steps.

For iOS builds, see the corresponding section below. More detailed instructions
can be found in the
[documentation](https://tido.bitbucket.io/rainbow/#building-rainbow-for-pc).

### Building with FMOD Studio

The default audio backend for all platforms is currently OpenAL. If you want to
use FMOD Studio, run `build.sh` with `-DUSE_FMOD_STUDIO=1` or replace
`RAINBOW_AUDIO_AL=1` with `RAINBOW_AUDIO_FMOD=1` in the project file.

FMOD's licence prevents redistribution of its files so you'll have to download
them yourself [here](https://www.fmod.org/download/). Extract or install it and
move the files into the repository under `lib/FMOD/`. It should look similar to
the listing below if you've done it correctly:

```bash
# Headers
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

# Android
lib/FMOD/lib/android/armeabi/libfmod.so
lib/FMOD/lib/android/armeabi/libfmodstudio.so
lib/FMOD/lib/android/armeabi-v7a/libfmod.so
lib/FMOD/lib/android/armeabi-v7a/libfmodstudio.so
lib/FMOD/lib/android/fmod.jar
lib/FMOD/lib/android/x86/libfmod.so
lib/FMOD/lib/android/x86/libfmodstudio.so

# iOS
lib/FMOD/lib/ios/libfmod_iphoneos.a
lib/FMOD/lib/ios/libfmod_iphonesimulator.a
lib/FMOD/lib/ios/libfmodstudio_iphoneos.a
lib/FMOD/lib/ios/libfmodstudio_iphonesimulator.a

# Linux
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

# macOS
lib/FMOD/lib/mac/libfmod.dylib
lib/FMOD/lib/mac/libfmodstudio.dylib

# Windows
lib/FMOD/lib/windows/fmod.dll
lib/FMOD/lib/windows/fmod64.dll
lib/FMOD/lib/windows/fmod64_vc.lib
lib/FMOD/lib/windows/fmod_vc.lib
lib/FMOD/lib/windows/fmodstudio.dll
lib/FMOD/lib/windows/fmodstudio64.dll
lib/FMOD/lib/windows/fmodstudio64_vc.lib
lib/FMOD/lib/windows/fmodstudio_vc.lib
```

Note that you don't need to download libraries for all platforms. They're only
listed for sake of completeness. If you're on Linux or macOS, you can use
`tools/fmod-import-tool.sh` to copy the files into place.

### Android

- [Android NDK](https://developer.android.com/tools/sdk/ndk/)
- [Android SDK](https://developer.android.com/sdk/)

Once you've installed both, open `rainbow/tools/build-android.sh` and change
`NDK_HOME` to point at the NDK. The build script will create a debuggable and
installable APK but is currently only available on Linux/macOS. You can still
compile for Android on Windows, it just requires some manual labour.

Follow the steps outlined earlier and specify `android` for `platform`:

	$ /path/to/rainbow/tools/build.sh android [options]

### iOS

- [Xcode](https://itunes.apple.com/no/app/xcode/id497799835?mt=12)

Use the provided Xcode project under `rainbow/build/xcode4/`.

### Linux (and cross-compiling for Windows)

- [CMake](https://cmake.org/)
- [Mesa](http://www.mesa3d.org/)
- [Ogg Vorbis](https://www.xiph.org/vorbis/) (optional with
  `-DUSE_FMOD_STUDIO=1`)
- [OpenAL Soft](http://kcat.strangesoft.net/openal.html) (optional with
  `-DUSE_FMOD_STUDIO=1`)
- [SDL](https://libsdl.org/)
- [MinGW-w64](https://mingw-w64.org/) (for cross-compiling only)

You can install most of these using your favourite package manager.

Windows builds can be built using MinGW-w64 by issuing `windows` as platform.
However, this method hasn't been maintained in quite some time and may no longer
work.

### macOS

- [CMake](https://cmake.org/)
- [Ogg Vorbis](https://www.xiph.org/vorbis/) (optional with
  `-DUSE_FMOD_STUDIO=1`)
- [SDL](https://libsdl.org/)
- [Xcode](https://itunes.apple.com/no/app/xcode/id497799835?mt=12)

CMake, Ogg Vorbis, and SDL can be installed through [Homebrew](http://brew.sh/)
or [MacPorts](https://www.macports.org/). Xcode's Command Line Tools must be
installed.

### Windows

- [CMake](https://cmake.org/)
- [OpenAL Soft](http://kcat.strangesoft.net/openal.html) (optional with
  `-DUSE_FMOD_STUDIO=1`)
- [SDL](https://libsdl.org/)
- [Visual Studio Community](https://www.visualstudio.com/en-us/products/visual-studio-community-vs.aspx)
  (2015 with Update 3 or later)

The process of downloading and setting up OpenAL Soft and SDL is fully automated
by the build scripts. Simply run the PowerShell script, `tools\make.ps1`, and
follow the wizard. If you get a prompt about execution policy, you can simply
dismiss it as the script doesn't require any special rights.

Alternatively, see "Linux" for cross-compilation.

## Learning

Check out the [documentation here](https://tido.bitbucket.io/rainbow/) or under
`doc`, and the demos under `demos`. Note: In order to run a demo, you'll need to
copy some files from `lua` into the folder of the demo you want to run. It's
easier to copy all even if you won't need everything.

Some demos may use resources that (unfortunately) cannot be distributed with
Rainbow.

Once you've copied all the necessary files, run `rainbow` inside the folder:

	$ cd /path/to/demo
	$ rainbow

Or with the path as argument:

	$ rainbow /path/to/demo
