#!/bin/bash
# Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

FMOD=$(cd -P "$(dirname $0)/.." && pwd)/lib/FMOD
SRC="$1/api"

if [ ! -d "$SRC" ]; then
  echo "$(basename $0): $SRC: No such directory"
  exit 1
fi

mkdir -p "$FMOD/inc"
pushd "$FMOD" 1> /dev/null
if [ ! -d "$SRC" ]; then
  popd 1> /dev/null
  SRC=$(cd -P "$SRC" && pwd)
  pushd "$FMOD" 1> /dev/null
fi

cp -v "$SRC/lowlevel/inc/fmod.h" inc/
cp -v "$SRC/lowlevel/inc/fmod.hpp" inc/
cp -v "$SRC/lowlevel/inc/fmod_codec.h" inc/
cp -v "$SRC/lowlevel/inc/fmod_common.h" inc/
cp -v "$SRC/lowlevel/inc/fmod_dsp.h" inc/
cp -v "$SRC/lowlevel/inc/fmod_dsp_effects.h" inc/
cp -v "$SRC/lowlevel/inc/fmod_errors.h" inc/
cp -v "$SRC/lowlevel/inc/fmod_output.h" inc/
cp -v "$SRC/studio/inc/fmod_studio.h" inc/
cp -v "$SRC/studio/inc/fmod_studio.hpp" inc/
cp -v "$SRC/studio/inc/fmod_studio_common.h" inc/

# Android
if [ -d "$SRC/lowlevel/lib/armeabi-v7a" ]; then
  if [ -d "$SRC/lowlevel/lib/armeabi" ]; then
    mkdir -p lib/android/armeabi
    cp -v "$SRC/lowlevel/lib/armeabi/libfmod.so" lib/android/armeabi/
    cp -v "$SRC/studio/lib/armeabi/libfmodstudio.so" lib/android/armeabi/
  fi
  mkdir -p lib/android/armeabi-v7a
  cp -v "$SRC/lowlevel/lib/armeabi-v7a/libfmod.so" lib/android/armeabi-v7a/
  cp -v "$SRC/studio/lib/armeabi-v7a/libfmodstudio.so" lib/android/armeabi-v7a/
  if [ -f "$SRC/lowlevel/lib/fmod.jar" ]; then
    mkdir -p lib/android
    cp -v "$SRC/lowlevel/lib/fmod.jar" lib/android/
  fi
  if [ -d "$SRC/lowlevel/lib/x86" ]; then
    mkdir -p lib/android/x86
    cp -v "$SRC/lowlevel/lib/x86/libfmod.so" lib/android/x86/
    cp -v "$SRC/studio/lib/x86/libfmodstudio.so" lib/android/x86/
  fi
fi

# iOS
if [ -f "$SRC/lowlevel/lib/libfmod_iphoneos.a" ]; then
  mkdir -p lib/ios
  cp -v "$SRC/lowlevel/lib/libfmod_iphoneos.a" lib/ios/
  cp -v "$SRC/lowlevel/lib/libfmod_iphonesimulator.a" lib/ios/
  cp -v "$SRC/studio/lib/libfmodstudio_iphoneos.a" lib/ios/
  cp -v "$SRC/studio/lib/libfmodstudio_iphonesimulator.a" lib/ios/
fi

# Linux
if [ -f "$SRC"/lowlevel/lib/x86_64/libfmod.so.*.* ]; then
  mkdir -p lib/linux/x86_64
  cp -av "$SRC"/lowlevel/lib/x86_64/libfmod.so* lib/linux/x86_64/
  cp -av "$SRC"/studio/lib/x86_64/libfmodstudio.so* lib/linux/x86_64/
  if [ -d "$SRC/lowlevel/lib/x86" ]; then
    mkdir -p lib/linux/x86
    cp -av "$SRC"/lowlevel/lib/x86/libfmod.so* lib/linux/x86/
    cp -av "$SRC"/studio/lib/x86/libfmodstudio.so* lib/linux/x86/
  fi
fi

# macOS
if [ -f "$SRC/lowlevel/lib/libfmod.dylib" ]; then
  mkdir -p lib/mac
  cp -v "$SRC/lowlevel/lib/libfmod.dylib" lib/mac/
  cp -v "$SRC/studio/lib/libfmodstudio.dylib" lib/mac/
fi

cp -v "$SRC/../doc/LICENSE.TXT" .
cp -v "$SRC/../doc/revision.txt" .

popd 1> /dev/null
