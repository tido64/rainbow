#!/bin/bash
MAKE="make -j1"
OUTPUT=rainbow
PROJECT="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REMOVABLE="CMake* cmake_install.cmake *.a Makefile $OUTPUT"
SCAN_BUILD=/usr/lib/clang-analyzer/scan-build

if [ ! -d $SCAN_BUILD ]; then
	echo "'$SCAN_BUILD' does not exist. Please set SCAN_BUILD appropriately."
	exit 1
fi

function compile {
	if ! which $2 &> /dev/null; then
		echo $1 pass: Skipped
		return
	fi
	rm -fr $REMOVABLE && \
	CC=$CC CXX=$CXX cmake $3 $PROJECT && \
	$MAKE
	if [ ! -x $OUTPUT ]; then
		echo $1 pass: Failed
		exit 1
	fi
	echo $1 pass: OK
}

# Cross compile pass
CC=
CXX=
compile "Cross compile" i486-mingw32-c++ -DMINGW=ON

# GCC compile pass
CC=gcc
CXX=g++
compile "GCC compile" $CC

# Clang compile pass
# Also performs static analysis and generates a report
CC=$SCAN_BUILD/ccc-analyzer
CXX=$SCAN_BUILD/c++-analyzer
MAKE="scan-build -o report --use-cc=clang --use-c++=clang make $@"
compile "Clang compile" scan-build #-DGTEST=ON
