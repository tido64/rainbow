#!/usr/bin/env bash
set -e

TARGET_OS=${TARGET_OS:-$(uname)}

case $1 in
  "install")
    case $TARGET_OS in
      "Darwin")
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
        cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DUNIT_TESTS=1 -DUSE_PHYSICS=1 -DUSE_SPINE=1 -G Ninja ..
        ninja
        ;;
    esac
    ;;
  "test")
    case $TARGET_OS in
      "Android")
        # disabled
        ;;
      "iOS")
        # disabled
        ;;
      *)
        cd out
        ./rainbow --gtest_filter=-AudioTest*:ChronoTest.TimeDifferenceBetweenUpdates --gtest_output="xml:test-report.xml"
        ;;
    esac
    ;;
  "after_success")
    case $TARGET_OS in
      "Linux")
        cd out
        bash <(curl -s https://codecov.io/bash) -R .. -x gcov-7 -K
        ;;
    esac
    ;;
esac
