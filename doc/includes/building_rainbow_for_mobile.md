# Building Rainbow for Android/iOS

## Getting the source code

First, make sure you've installed [Git](https://git-scm.com/downloads) as that's
what we'll be using to retrieve the source code. The following command will
download Rainbow and all of its submodules into a folder named `rainbow`:

```shell
git clone --recursive https://bitbucket.org/tido/rainbow.git
```

If you're using a graphical UI for Git, make sure to checkout the submodules as
well.

## Building with FMOD Studio

FMOD's licence prevents redistribution of its files so you'll have to download
them yourself [here](https://www.fmod.org/download/). Extract or install it and
move the files into the repository under `lib/FMOD/`. If you have access to a
Bash-compatible shell, `tools/fmod-import-tool.sh` will help you copy the files
into place.

## Build for Android

Install the dependencies:

* [Android Studio](https://developer.android.com/studio/)
* [Android NDK](https://developer.android.com/ndk/downloads/)
* [CMake](https://cmake.org/)
* [FMOD](https://www.fmod.org/)

Once installed:

	mkdir out
	cd out
	/path/to/rainbow/tools/build.sh android [options]

The build script will create a debuggable and installable APK. For a list of
options, type `build.sh help` or see [Compiling Rainbow](#compiling-rainbow).

### Build for iOS

Install [Xcode](https://itunes.apple.com/no/app/xcode/id497799835?mt=12) from
the App Store, then open the provided Xcode project
`build/ios/Rainbow.xcodeproj`.

The default audio backend is currently OpenAL. If you want to use FMOD Studio,
replace `RAINBOW_AUDIO_AL=1` with `RAINBOW_AUDIO_FMOD=1` in the project's
settings.
