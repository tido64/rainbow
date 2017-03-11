# Rainbow [![codecov](https://codecov.io/gh/tn0502/rainbow/branch/master/graph/badge.svg)](https://codecov.io/gh/tn0502/rainbow)

Rainbow is a fast, scriptable, cross-platform, 2D game engine written in modern
C++.

Copyright &copy; 2010-present Bifrost Entertainment AS and Tommy Nguyen.

Distributed under the MIT License (see accompanying file `LICENSE` or copy at
https://opensource.org/licenses/MIT). Licenses for third-party software can be
found in file `THIRDPARTY`.

- [Documentation](https://tido.bitbucket.io/rainbow/)
- [Roadmap](https://trello.com/b/r2TqudY6/rainbow)
- [Discussion](https://groups.google.com/forum/#!forum/rainbow-tech)

## Features

### Cross-platform

Rainbow runs on most popular operating systems:

| Android | iOS | Linux/macOS | Windows |
|:-------:|:---:|:-----------:|:-------:|
| ✓ | ✓ | [![Linux build Status](https://travis-ci.org/tn0502/rainbow.svg?branch=master)](https://travis-ci.org/tn0502/rainbow)<br />[![*nix build status](https://doozer.io/badge/tn0502/rainbow/buildstatus/master)](https://doozer.io/tn0502/rainbow) | [![Windows build status](https://ci.appveyor.com/api/projects/status/oajtxwu9d9lrayk0?svg=true)](https://ci.appveyor.com/project/Tommy/rainbow) |

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

	git clone --recursive https://bitbucket.org/tido/rainbow.git

The repository only includes some of the libraries required to build Rainbow.
The remaining requisites are listed in each platform's section below. They must
be installed or copied into the repository before you can start building
Rainbow.

More details can be found in the
[documentation](https://tido.bitbucket.io/rainbow/#building-rainbow-for-pc).

### On Linux/macOS

Easiest way to build Rainbow is to use the provided build scripts:

	mkdir rainbow-build
	cd rainbow-build
	/path/to/rainbow/tools/build.sh [platform] [options]

Run `build.sh help` to get an overview of options and configurations. `platform`
can be omitted if compiling a native build (i.e. not cross-compiling).

If you have problems running `build.sh`, please make sure it has execution
permission:

	chmod +x /path/to/rainbow/tools/build.sh

### On Windows

Run the PowerShell script `tools\make.ps1` (right-click on it and select
_Run with PowerShell_). It will take you through the steps.

## Learning

Check out the [documentation here](https://tido.bitbucket.io/rainbow/) or under
`doc`, and the demos under `demos`. Note: In order to run a demo, you'll need to
copy some files from `lua` into the folder of the demo you want to run. It's
easier to copy all even if you won't need everything.

Some demos may use resources that (unfortunately) cannot be distributed with
Rainbow.

Once you've copied all the necessary files, run `rainbow` inside the folder:

	cd /path/to/demo
	rainbow

Or with the path as argument:

	rainbow /path/to/demo
