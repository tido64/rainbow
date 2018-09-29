#!/usr/bin/env bash
case $1 in
  "before_install")
    case $TRAVIS_OS_NAME in
      "linux")
        if [[ "$BUILD_TYPE" == "Android" ]]; then
          echo y | sdkmanager "ndk-bundle"
          echo y | sdkmanager "cmake;3.6.4111459"
          if [[ ! -f local/gradle-4.10.2 ]]; then
            rm -r local
            curl -LO https://services.gradle.org/distributions/gradle-4.10.2-bin.zip
            unzip gradle-4.10.2-bin.zip
            mv gradle-4.10.2 local
            touch local/gradle-4.10.2
          fi
        else
          if [[ ! -f local/cmake-3.11.4 ]]; then
            rm -r local
            curl -LO https://cmake.org/files/v3.11/cmake-3.11.4-Linux-x86_64.sh
            mkdir -p local
            sh cmake-3.11.4-Linux-x86_64.sh --prefix=local --skip-license
            touch local/cmake-3.11.4
          fi
        fi
        ;;
      "osx")
        brew bundle
        ;;
    esac
    ;;
  "script")
    case $TRAVIS_OS_NAME in
      "linux")
        export CC=gcc-7 CXX=g++-7 PATH=$(pwd)/local/bin:$PATH
        export TEST_ARGS="--test"
        ;;
      "osx")
        export TEST_ARGS="--gtest_filter=-AudioTest*:ChronoTest.TimeDifferenceBetweenUpdates"
        ;;
    esac
    if [[ "$BUILD_TYPE" == "Android" ]]; then
      cd build/android/
      TERM=dumb gradle build
    elif [[ "$BUILD_TYPE" == "iOS" ]]; then
      xcodebuild -project build/ios/Rainbow.xcodeproj -scheme Rainbow -arch arm64 CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED=NO | xcpretty
    else
      mkdir $TRAVIS_BUILD_DIR-build
      cd $TRAVIS_BUILD_DIR-build
      GENERATOR=Ninja $TRAVIS_BUILD_DIR/tools/build.sh -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DUNIT_TESTS=1 -DUSE_PHYSICS=1 -DUSE_SPINE=1
      ./rainbow $TEST_ARGS
    fi
    ;;
  "after_success")
    case $TRAVIS_OS_NAME in
      "linux")
        if [[ "$BUILD_TYPE" == "Debug" ]]; then
          cd $TRAVIS_BUILD_DIR
          cp -r $TRAVIS_BUILD_DIR-build/CMakeFiles/rainbow.dir/src .
          curl -s https://codecov.io/bash | bash -s - -x gcov-7
        fi
        ;;
    esac
    ;;
esac
