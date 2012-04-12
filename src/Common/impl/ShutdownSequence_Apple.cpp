// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Platform.h"
#ifdef RAINBOW_IOS

#include <UIKit/UIAlertView.h>
#include "Common/ShutdownSequence.h"

@interface RainbowAlertViewDelegate : UIViewController <UIAlertViewDelegate> {
}
@end

@implementation RainbowAlertViewDelegate

- (void)alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	//[[UIApplication sharedApplication] applicationWillTerminate];
	exit(1);
}

@end

void ShutdownSequence::operator()()
{
	static bool crashed = false;
	if (crashed)
		return;
	crashed = true;

	RainbowAlertViewDelegate *rad = [RainbowAlertViewDelegate alloc];

	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Crashed! >_<"
		message:@"We're sincerely sorry for this."
		delegate:rad
		cancelButtonTitle:@"OK"
		otherButtonTitles:nil];
	[alert show];
}

#endif
