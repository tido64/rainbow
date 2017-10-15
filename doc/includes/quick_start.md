# Quick Start

Before you start, please make sure you've read
[Building Rainbow for PC](#building-rainbow-for-pc) or
[Building Rainbow for Android/iOS](#building-rainbow-for-android-ios). The
following tutorials will assume that you've already built Rainbow.

## Android

1. In your Android project folder, create a directory named `assets`.
2. Copy the contents of `js` into this folder.

Build Rainbow again so assets get bundled with the APK. Connect your device and
install it:

`adb install -r bin/Rainbow-debug.apk`

Start the app on your device.

## iOS

1. Open `build/ios/Rainbow.xcodeproj` in Xcode.
2. Build and then run the current scheme.

## Linux / macOS / Windows

Start the playground:

`rainbow /path/to/rainbow/js`

Or if you're on Windows, drag and drop the folder on `rainbow.exe`.
