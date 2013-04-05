// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#include "Platform/Definitions.h"
#ifdef RAINBOW_IOS

#include <UIKit/UIAlertView.h>
#include "Common/ShutdownSequence.h"

@interface RainbowAlertViewDelegate : UIViewController <UIAlertViewDelegate>

@end

RainbowAlertViewDelegate *gAlertViewDelegate = nil;

void ShutdownSequence::operator()()
{
	if (gAlertViewDelegate)
		return;

	gAlertViewDelegate = [[RainbowAlertViewDelegate alloc] init];
	UIAlertView *alert =
		[[UIAlertView alloc] initWithTitle:@"Crashed! x_x"
		                           message:@"We're sincerely sorry for this."
		                          delegate:gAlertViewDelegate
		                 cancelButtonTitle:@"OK"
		                 otherButtonTitles:nil];
	[alert show];
}

@implementation RainbowAlertViewDelegate

- (void)alertView
{
	//[[UIApplication sharedApplication] applicationWillTerminate];
	exit(1);
}

@end

#endif
