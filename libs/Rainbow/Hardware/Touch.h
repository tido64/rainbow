/// Data structure for keeping track of touches.

/// iOS: Defines custom CFDictionaryKeyCallBacks and CFDictionaryValueCallBacks.
///
/// \see http://developer.apple.com/library/mac/#documentation/CoreFoundation/Conceptual/CFCollections/Tasks/creating.html
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef TOUCH_H_
#define TOUCH_H_

#include <Rainbow/Hardware/Platform.h>

struct Touch
{
	unsigned int hash;  ///< Unique hash value that distinguishes this touch from another
	float x;            ///< x-component of the touch location
	float y;            ///< y-component of the touch location

	Touch() : hash(0), x(0.0f), y(0.0f) { }
	Touch(const unsigned int h, const float &x, const float &y) :
		hash(h), x(x), y(x) { }
};

#if defined(ONWIRE_ANDROID)

#include <Rainbow/HashTable.h>

typedef std::tr1::unordered_map<unsigned int, Touch> Touches;

#elif defined(ONWIRE_IOS)

#include <cocos2d/cocos2d.h>

typedef CFMutableDictionaryRef Touches;

Touch *get_touch_set(NSSet *touches);

extern const CFDictionaryKeyCallBacks kHashDictionaryKeyCallBacks;
Boolean uint_equal(const void *a, const void *b);
CFHashCode uint_hash(const void *ptr);
CFStringRef uint_copy_description(const void *ptr);

extern const CFDictionaryValueCallBacks kTouchDictionaryValueCallBacks;
const void *touch_retain(CFAllocatorRef a, const void *ptr);
void touch_release(CFAllocatorRef a, const void *ptr);
Boolean touch_equal(const void *a, const void *b);
CFStringRef touch_copy_description(const void *ptr);

#endif

#endif
