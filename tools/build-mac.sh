#!/bin/bash
PROJECT="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
rm -fr CMakeCache.txt CMakeFiles CMakeScripts Debug Rainbow.* cmake_install.cmake
cmake -DHEIMDALL=ON -G Xcode $PROJECT &&
xcodebuild -project Rainbow.xcodeproj -arch `uname -m` build
