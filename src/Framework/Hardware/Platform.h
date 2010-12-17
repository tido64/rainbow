/// Platform specific definitions. Makes sure correct code is being compiled.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef PLATFORM_H_
#define PLATFORM_H_

#if defined(ANDROID)
#	define ONWIRE_ANDROID
#elif defined(TARGET_OS_MAC) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
#	define ONWIRE_IOS
#	define ctons_str(s)  [NSString stringWithUTF8String:(s)];
#endif

#endif
