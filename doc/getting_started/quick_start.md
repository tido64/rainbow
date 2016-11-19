# Quick Start Guide

Before you start, please make sure you've read `README.md`. The following
tutorials will assume that you've already built Rainbow.

Pick a demo under `demos`. For the purpose of this tutorial, we'll try to get
`transitions` running.

## Android

1. Create a folder to contain Android files.
2. Copy `transitions` into this folder.
3. Follow steps 2-4 for Linux/macOS/Windows.
4. Rename `transitions` `assets`.

From the folder in step 1, run the build script:

	$ /path/to/rainbow/tools/build.sh android

Connect your device and install it:

	$ adb install -r bin/Rainbow-debug.apk

Start the app on your device.

## iOS

1. Open `build/xcode4/Rainbow.xcodeproj` in Xcode.
2. Expand `Resources` to see all the convenience libraries.
3. Right-click `Resources` and choose "Add Files to Rainbow...".
4. Navigate to `demos` and select `transitions`. Leave "Destination" unchecked
   and "Create groups for any added folders" selected. Click "Add".
5. You should now see `transitions` under `Resources`. Right-click it and choose
   "Add Files to Rainbow...".
6. This time, navigate to `demos` and add `canvas.png`.

You should now be able to run this demo in the simulator or on your device.

## Linux / macOS / Windows

1. Copy this folder somewhere. In the following steps, we'll be copying more
   stuff into this folder. This will be your game folder.
2. The `transitions` demo uses some convenience libraries, so navigate to the
   `lua` folder and copy all the Lua files to your game folder. (The demo
   doesn't actually use all of them but we'll just copy all for simplicity's
   sake.)
3. `transitions` also needs a texture. This texture is located in
   `demos/`. Copy `canvas.png` to your game folder.

Now your demo folder should contain at least the following files:

	Kim.lua
	Math.lua
	Transition.lua
	TransitionFunctions.lua
	canvas.png
	main.lua

Start the demo:

	$ rainbow /path/to/demo

Or if you're on Windows, drag and drop the folder on `rainbow.exe`.
