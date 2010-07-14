/*
 *  Platform.h
 *  OnWire
 *
 *  Created by Tommy Nguyen on 6/26/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#if defined(ANDROID)
#	define ONWIRE_ANDROID
#elif defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
#	import "cocos2d.h"
#	define ctons_str(s) [NSString stringWithUTF8String: (s)];
#	define ONWIRE_IOS
#endif

#endif
