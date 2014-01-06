#!/bin/bash
GENERATOR=${GENERATOR:-Unix Makefiles}
IS_INSIDE=$(git rev-parse --show-toplevel 2> /dev/null)
RAINBOW=$(cd -P "$(dirname $0)/.." && pwd)
if [[ "$IS_INSIDE" == "$RAINBOW" ]]; then
	echo "$0: Cannot run while still inside the repository"
	exit 0
fi

function compile {
	case "$1" in
		"Emscripten")
			make && [[ -f rainbow ]] || exit 1
			mv rainbow rainbow.bc
			em++ -v rainbow.bc -o rainbow.html
			mv rainbow.bc rainbow
			;;
		"Ninja")
			ninja
			;;
		"Unix Makefiles")
			make
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
		echo "  -DGTEST=1                Enable Google C++ Testing Framework"
		echo "  -DUSE_HEIMDALL=1         Enable Heimdall debugging facilities"
		echo "  -DUSE_PHYSICS=1          Enable physics module (Box2D)"
		echo
		echo "  CMake options are passed directly to CMake so you can set variables like"
		echo "  -DCMAKE_BUILD_TYPE=<type> among others."
		echo
		echo "Environment variables:"
		echo "  CC                       C compiler command"
		echo "  CXX                      C++ compiler command"
		echo "  GENERATOR                CMake: Specify a makefile generator"
		echo "  NDK_DEBUG                Android: Compile debuggable binary"
		echo "  NDK_TOOLCHAIN_VERSION    Android: Compiler used by ndk-build"
		;;
	"analyze")
		CC=ccc-analyzer CXX=c++-analyzer $SHELL $0 $ARGS
		;;
	"android")
		$SHELL "$RAINBOW/tools/build-android.sh" $ARGS
		;;
	"clean")
		rm -fr CMakeFiles CMakeScripts Debug Rainbow.* Release lib
		rm -f .ninja_log CMakeCache.txt Makefile {build,rules}.ninja cmake_install.cmake lib*.a rainbow*
		;;
	"emscripten")
		EMSCRIPTEN=$(em-config EMSCRIPTEN_ROOT)
		if [[ ! -d "$EMSCRIPTEN" ]]; then
			echo "$0: Could not find Emscripten"
			exit 1
		fi
		cmake -DCMAKE_TOOLCHAIN_FILE="$EMSCRIPTEN/cmake/Platform/Emscripten.cmake" \
		      -DCMAKE_MODULE_PATH="$EMSCRIPTEN/cmake" \
		      -DEMSCRIPTEN=1 \
		      $ARGS "$RAINBOW" &&
		compile Emscripten
		;;
	"help")
		$SHELL $0 --help
		;;
	"linux")
		CC=${CC:-clang} CXX=${CXX:-clang++} cmake $ARGS -G "$GENERATOR" "$RAINBOW" &&
		compile "$GENERATOR"
		;;
	"mac")
		cmake $ARGS -G Xcode "$RAINBOW" &&
		compile Xcode
		;;
	"windows")
		cmake -DCMAKE_TOOLCHAIN_FILE="$RAINBOW/build/cmake/MinGW.cmake" $ARGS -G "$GENERATOR" "$RAINBOW" &&
		compile $GENERATOR
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
