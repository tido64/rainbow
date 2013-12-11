//
//  AppDelegate.m
//  Rainbow
//
//  Created by Tommy Nguyen on 4/4/12.
//  Copyright (c) 2012-13 Bifrost Entertainment AS. All rights reserved.
//

#import "AppDelegate.h"
#import "RainbowViewController.h"

#ifdef USE_TESTFLIGHT_SDK
#	import <AdSupport/AdSupport.h>
#	import <TestFlight/TestFlight.h>
#	define kRainbowAppToken @"3f3029e5-fa3f-46af-b5fb-bd36d2779e57"
#endif

#pragma clang diagnostic ignored "-Wunused-parameter"

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
#ifdef USE_TESTFLIGHT_SDK
	NSString *deviceIdentifier = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
	[TestFlight setDeviceIdentifier:deviceIdentifier];
	[TestFlight takeOff:kRainbowAppToken];
#endif

	[[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationLandscapeRight];
	self.viewController = [[RainbowViewController alloc] init];
	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	[self.window setRootViewController:self.viewController];

	self.window.backgroundColor = [UIColor whiteColor];
	[self.window makeKeyAndVisible];
	return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
