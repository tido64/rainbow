#!/bin/bash
# Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

RAINBOW=$(cd -P "$(dirname $0)/.." && pwd)
if [[ "$(git rev-parse --show-toplevel 2> /dev/null)" == "$RAINBOW" ]]; then
  echo "$0: Cannot run while still inside the repository"
  exit 0
fi

function compile {
  case "$1" in
    "Emscripten")
      make
      ;;
    "Ninja")
      $ANALYZER ninja
      ;;
    "Unix Makefiles")
      $ANALYZER make
      ;;
    "Xcode")
      local configuration="Debug"
      if [[ "$ARGS" =~ CMAKE_BUILD_TYPE=([A-za-z]+) ]]; then
        configuration=${BASH_REMATCH[1]}
      fi
      xcrun xcodebuild -project Rainbow.xcodeproj -configuration $configuration -arch $(uname -m) build
      ;;
    *)
      ;;
  esac
}

# Try to download SDL
if [ ! -z $(which hg) ] && [ ! -f "$RAINBOW/lib/SDL/include/SDL.h" ]; then
  echo -n 'Downloading SDL…'
  hg clone -q -u release-2.0.3 http://hg.libsdl.org/SDL "$RAINBOW/lib/SDL"
  echo ' done'
fi

# Prune arguments
if [ ! -z "$2" ]; then
  ARGS=$@
  ARGS=${ARGS#* }
fi

case $1 in
  "--help")
    echo "Syntax: $(basename $0) [analyze|android|clean|linux|mac|windows] [option ...]"
    echo
    echo "Options:"
    echo "  -DUNIT_TESTS=1           Enable unit tests"
    echo "  -DUSE_FMOD_STUDIO=1      Enable FMOD Studio audio engine"
    echo "  -DUSE_HEIMDALL=1         Enable Heimdall debugging facilities"
    echo "  -DUSE_PHYSICS=1          Enable physics module (Box2D)"
    echo "  -DUSE_SPINE=1            Enable Spine runtime"
    echo
    echo "CMake options are passed directly to CMake so you can set variables like"
    echo "-DCMAKE_BUILD_TYPE=<type> among others."
    echo
    echo "Environment variables:"
    echo "  CC                       C compiler command"
    echo "  CXX                      C++ compiler command"
    echo "  GENERATOR                CMake: Specify a build system generator"
    echo "  NDK_DEBUG                Android: Compile debuggable binary"
    echo "  NDK_TOOLCHAIN_VERSION    Android: Compiler used by ndk-build"
    ;;
  "analyze")
    ANALYZER=scan-build CC=ccc-analyzer CXX=c++-analyzer $SHELL $0 $ARGS
    ;;
  "android")
    $SHELL "$RAINBOW/tools/build-android.sh" $ARGS
    ;;
  "clean")
    rm -fr CMakeFiles CMakeScripts Debug MinSizeRel Rainbow.* Release RelWithDebInfo lib
    rm -f .ninja_* CMakeCache.txt Makefile {build,rules}.ninja cmake_install.cmake lib*.a rainbow*
    ;;
  "emscripten")
    EMSCRIPTEN=$(em-config EMSCRIPTEN_ROOT)
    if [[ ! -d "$EMSCRIPTEN" ]]; then
      echo "$0: Could not find Emscripten"
      exit 1
    fi
    cmake -DCMAKE_TOOLCHAIN_FILE="$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake" \
          $ARGS "$RAINBOW" &&
    compile Emscripten
    ;;
  "help")
    $SHELL $0 --help
    ;;
  "linux")
    GENERATOR=${GENERATOR:-Unix Makefiles}
    CC=${CC:-clang} CXX=${CXX:-clang++} cmake $ARGS -G "$GENERATOR" "$RAINBOW" &&
    compile "$GENERATOR"
    ;;
  "mac")
    GENERATOR=${GENERATOR:-Xcode}
    cmake $ARGS -G "$GENERATOR" "$RAINBOW" &&
    compile "$GENERATOR"
    ;;
  "windows")
    GENERATOR=${GENERATOR:-Unix Makefiles}
    cmake -DCMAKE_TOOLCHAIN_FILE="$RAINBOW/build/cmake/MinGW.cmake" \
          $ARGS -G "$GENERATOR" "$RAINBOW" &&
    compile "$GENERATOR"
    ;;
  *)  # Attempt to detect platform
    case $(uname) in
      "Darwin")
        $SHELL $0 mac $@
        ;;
      "Linux")
        $SHELL $0 linux $@
        ;;
      *)
        $SHELL $0 --help
        ;;
    esac
    ;;
esac
