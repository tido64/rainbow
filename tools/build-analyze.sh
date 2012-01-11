#!/bin/bash
PROJECT="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REMOVABLE="CMake* cmake_install.cmake *.a Makefile rainbow"
SCAN_BUILD=/usr/lib/clang-analyzer/scan-build

if [ ! -d $SCAN_BUILD ]; then
	echo "'$SCAN_BUILD' does not exist. Please set SCAN_BUILD appropriately."
	exit 1
fi

# Test cross compiling
rm -fr $REMOVABLE && \
cmake -DMINGW=ON $PROJECT && \
make -j1 && \

# Test compiling with GCC
rm -fr $REMOVABLE && \
CC=gcc CXX=g++ cmake $PROJECT && \
make -j1 && \

# Finally, compile with Clang analyzer and run tests
rm -fr $REMOVABLE && \
#CC=$SCAN_BUILD/ccc-analyzer CXX=$SCAN_BUILD/c++-analyzer cmake -DGTEST=ON $PROJECT && \
CC=$SCAN_BUILD/ccc-analyzer CXX=$SCAN_BUILD/c++-analyzer cmake $PROJECT && \
scan-build -o report --use-cc=clang --use-c++=clang make $@ && \
./rainbow
