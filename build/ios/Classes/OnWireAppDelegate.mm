//
//  OnWireAppDelegate.m
//  OnWire
//
//  Created by Tommy Nguyen on 6/25/10.
//  Copyright Bifrost Games 2010. All rights reserved.
//

#import "OnWireAppDelegate.h"
#import "OnWireScene.h"

@implementation OnWireAppDelegate

@synthesize window;

-(void)applicationDidFinishLaunching:(UIApplication*)application
{
	// Init the window
	window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

	// Try to use CADisplayLink director, if it fails (SDK < 3.1) use the default director
	if(![CCDirector setDirectorType:kCCDirectorTypeDisplayLink])
		[CCDirector setDirectorType:kCCDirectorTypeDefault];

	CCDirector *director = [CCDirector sharedDirector];
	[director setDeviceOrientation:kCCDeviceOrientationPortrait];  // We don't want landscape orientation
	[director setDisplayFPS:true];                                 // Display FPS only when debugging
	[director setAnimationInterval:1.0/60];                        // 60 FPS is nice

	EAGLView *glView = [EAGLView viewWithFrame:[window bounds]
		pixelFormat:kEAGLColorFormatRGBA8
		depthFormat:GL_DEPTH_COMPONENT24_OES
		//preserveBackbuffer:NO
		//sharegroup:nil
		//multiSampling:YES
		//numberOfSamples:4
	];
	[glView setMultipleTouchEnabled:YES];

	// Attach the OpenGL View to the director
	[director setOpenGLView:glView];
	[window addSubview:glView];
	[window makeKeyAndVisible];

	// Enables High Res mode (Retina Display) on iPhone 4 and maintains low res on all other devices
	[director enableRetinaDisplay:YES];

	// Default texture format for PNG/BMP/TIFF/JPEG/GIF images
	// It can be RGBA8888, RGBA4444, RGB5_A1, RGB565
	// You can change anytime.
	[CCTexture2D setDefaultAlphaPixelFormat:kCCTexture2DPixelFormat_RGBA8888];

	[[CCDirector sharedDirector] runWithScene: [OnWire scene]];
}

-(void)applicationWillResignActive:(UIApplication *)application {
	[[CCDirector sharedDirector] pause];
}

-(void)applicationDidBecomeActive:(UIApplication *)application {
	[[CCDirector sharedDirector] resume];
}

-(void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
	[[CCDirector sharedDirector] purgeCachedData];
}

-(void)applicationDidEnterBackground:(UIApplication*)application {
	[[CCDirector sharedDirector] stopAnimation];
}

-(void)applicationWillEnterForeground:(UIApplication*)application {
	[[CCDirector sharedDirector] startAnimation];
}

-(void)applicationWillTerminate:(UIApplication *)application {
	CCDirector *director = [CCDirector sharedDirector];
	[[director openGLView] removeFromSuperview];
	[window release];
	[director end];
}

-(void)applicationSignificantTimeChange:(UIApplication *)application {
	[[CCDirector sharedDirector] setNextDeltaTimeZero:YES];
}

-(void)dealloc {
	[[CCDirector sharedDirector] release];
	[window release];
	[super dealloc];
}

@end
