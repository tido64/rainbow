#!/usr/bin/env bash
set -e

TARGET_OS=${TARGET_OS:-$(uname)}

case $1 in
  "install")
    case $TARGET_OS in
      "Darwin")
        sudo xcode-select --switch /Applications/Xcode_10.3.app/Contents/Developer
        brew bundle
        ;;
      "Linux")
        sudo apt-get update
        sudo apt-get install --yes gcc-7 g++-7 libpulse-dev libsdl2-dev ninja-build pulseaudio
        ;;
      *)
        ;;
    esac
    ;;
  "build")
    case $(uname) in
      "Linux")
        export CC=gcc-7 CXX=g++-7
        ;;
    esac
    case $TARGET_OS in
      "Android")
        cd build/android/
        TERM=dumb gradle build
        ;;
      "iOS")
        xcodebuild -project build/ios/Rainbow.xcodeproj -scheme Rainbow -arch arm64 CODE_SIGNING_ALLOWED=NO | xcpretty
        ;;
      *)
        mkdir out
        cd out
        cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DUNIT_TESTS=1 -DCOVERAGE=1 -DUSE_PHYSICS=1 -G Ninja ..
        ninja
        ;;
    esac
    ;;
  "test")
    case $TARGET_OS in
      "Android")
        # disabled
        ;;
      "Linux")
        cd out
        ./rainbow --gtest_output="xml:test-report.xml"
        ;;
      "iOS")
        # disabled
        ;;
      *)
        cd out
        ./rainbow --gtest_filter=-AudioTest* --gtest_output="xml:test-report.xml"
        ;;
    esac
    ;;
  "after_success")
    case $TARGET_OS in
      "Darwin")
        cd out
        find . -type f -name '*.gcno' -execdir gcov -pb {} \;
        PATH="/usr/local/bin:/usr/bin:/bin" bash <(curl -s https://codecov.io/bash) -X coveragepy -R .. -p . -d -K
        ;;
      "Linux")
        cd out
        PATH="/usr/local/bin:/usr/bin:/bin" bash <(curl -s https://codecov.io/bash) -X coveragepy -R .. -p . -x gcov-7 -K
        ;;
    esac
    ;;
esac
