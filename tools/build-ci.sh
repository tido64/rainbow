#!/usr/bin/env bash
set -e

TARGET_OS=${TARGET_OS:-$(uname)}

case $1 in
  "install")
    case $TARGET_OS in
      "Android")
        if [[ ! -z "$TRAVIS_BUILD_DIR" ]]; then
          echo y | sdkmanager "ndk-bundle"
          echo y | sdkmanager "cmake;3.6.4111459"
          if [[ ! -f local/gradle-4.10.2 ]]; then
            rm -r local
            curl -LO https://services.gradle.org/distributions/gradle-4.10.2-bin.zip
            unzip gradle-4.10.2-bin.zip
            mv gradle-4.10.2 local
            touch local/gradle-4.10.2
          fi
        fi
        ;;
      "Darwin")
        brew bundle
        ;;
      "Linux")
        if [[ ! -z "$TRAVIS_BUILD_DIR" ]]; then
          if [[ ! -f local/cmake-3.11.4 ]]; then
            rm -r local
            curl -LO https://cmake.org/files/v3.11/cmake-3.11.4-Linux-x86_64.sh
            mkdir -p local
            sh cmake-3.11.4-Linux-x86_64.sh --prefix=local --skip-license
            touch local/cmake-3.11.4
          fi
        else
          sudo add-apt-repository ppa:ubuntu-toolchain-r/test
          sudo apt-get update
          sudo apt-get install --yes gcc-7 g++-7 libpulse-dev libsdl2-dev ninja-build pulseaudio
        fi
        ;;
      *)
        ;;
    esac
    ;;
  "build")
    case $(uname) in
      "Linux")
        export CC=gcc-7 CXX=g++-7 PATH=$(pwd)/local/bin:$PATH
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
        ;;
      "iOS")
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
        if [[ ! -z "$TRAVIS_BUILD_DIR" ]] && [[ "$BUILD_TYPE" == "Debug" ]]; then
          cp -r out/CMakeFiles/rainbow.dir/src .
          curl -s https://codecov.io/bash | bash -s - -x gcov-7
        fi
        ;;
    esac
    ;;
esac
