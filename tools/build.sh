#!/bin/bash
GENERATOR=${GENERATOR:-Unix Makefiles}
RB=$(cd -P "$(dirname $0)/.." && pwd)

function compile {
	case $1 in
		"Ninja")
			ninja
			;;
		"Unix Makefiles")
			make
			;;
		"Xcode")
			xcodebuild -project Rainbow.xcodeproj -arch $(uname -m) build
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
		echo "  -DCANVAS=1               Enable canvas module"
		echo "  -DGTEST=1                Enable Google C++ Testing Framework"
		echo "  -DHEIMDALL=1             Enable Heimdall debugging facilities"
		echo "  -DPHYSICS=1              Enable physics module (Box2D)"
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
		$SHELL "$RB/tools/build-android.sh" $ARGS
		;;
	"clean")
		rm -fr CMakeFiles CMakeScripts Debug Rainbow.* Release lib
		rm -f .ninja_log CMakeCache.txt Makefile {build,rules}.ninja cmake_install.cmake lib*.a rainbow rainbow.exe
		;;
	"help")
		$SHELL $0 --help
		;;
	"linux")
		CC=${CC:-clang} CXX=${CXX:-clang} cmake $ARGS -G "$GENERATOR" "$RB" &&
		compile "$GENERATOR"
		;;
	"mac")
		cmake $ARGS -G Xcode "$RB" &&
		compile Xcode
		;;
	"windows")
		cmake -DCMAKE_TOOLCHAIN_FILE="$RB/build/cmake/MinGW.cmake" $ARGS -G "$GENERATOR" "$RB" &&
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
