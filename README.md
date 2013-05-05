# Rainbow

Rainbow is a scriptable, cross-platform, 2D rendering engine written in C++.

## Features

### Platforms

- Android
- iOS
- Linux
- Mac OS X
- Windows

### Audio

- OpenAL as backend on most platforms.
  [OpenSL ES](http://www.khronos.org/opensles/) on Android.
- [Ogg Vorbis](http://www.vorbis.com/) on most platforms. On iOS (exclusively)
  and Mac OS X (in addition), see the
  [Multimedia Programming Guide](http://developer.apple.com/library/ios/#documentation/AudioVideo/Conceptual/MultimediaPG/UsingAudio/UsingAudio.html).

### Graphics

- [OpenGL](http://www.opengl.org/) and
  [OpenGL ES 2.0](http://www.khronos.org/opengles/2_X/)
- Shaders
- Sprites-based rendering, with implicit batching
- Text rendering (TrueType and OpenType through [FreeType](http://www.freetype.org/))
- Texture atlas ([PNG](http://www.libpng.org/pub/png/) and PVRTC)

### Input

- Accelerometer on Android and iOS.
- Keyboard and mouse on desktop platforms.
- Touch on Android and iOS.

### Others

- Scene graph
- Embedded [Lua](http://www.lua.org/) scripting language
- Physics ([Box2D](http://http://box2d.org/))

## Building Instructions

Easiest way to build Rainbow is to use the provided build scripts:

	$ mkdir rainbow-build
	$ cd rainbow-build
	$ /path/to/rainbow/tools/build.sh [platform] [options]

Run `tools/build.sh help` to get an overview of options and configurations.
`platform` can be omitted if compiling a native build (i.e. not
cross-compiling).

The repository will include some of the libraries required to build Rainbow.
[CMake](http://www.cmake.org/) is required if compiling a desktop build. Each
platform has different requirements as listed below.

### Android

Requires only [Android SDK](http://developer.android.com/sdk/) and
[NDK](http://developer.android.com/tools/sdk/ndk/). The build script will create
an installable APK.

### iOS

Use the provided Xcode project. As of writing, the build script won't compile
iOS builds.


### Linux (and cross-compiling for Windows)

Make sure to have [Mesa](http://www.mesa3d.org/),
[OpenAL Soft](http://kcat.strangesoft.net/openal.html) and
[Vorbis](http://www.vorbis.com/) (particularly Vorbisfile) installed in order to
properly link Rainbow. [Clang](http://clang.llvm.org/) is the default compiler
but you can use [GCC](http://gcc.gnu.org/), or any other compiler for that
matter, by prefixing `CC=gcc CXX=g++`.

Windows builds can be built using [MinGW](http://www.mingw.org/) or
[MinGW-w64](http://mingw-w64.sourceforge.net/) by issuing `windows` as platform.

### Mac OS X

Xcode and the Command Line Tools package must be installed. Additionally,
[CMake](http://www.cmake.org/) and [Vorbis](http://www.vorbis.com/) should be
installed through [MacPorts](http://www.macports.org/).

### Windows

Compiling with Visual Studio is supported using prebuilt `libogg`, `libvorbis`,
`libvorbisfile` and `zlib`. Prebuilt
[OpenAL Soft](http://kcat.strangesoft.net/openal.html) binary is offered at its
official home page. Download it and place it with the others. Use the Windows
build script to generate a Visual Studio project:

	> \path\to\rainbow\tools\build.bat

See Linux for an alternative method.
