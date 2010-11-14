/*
 *  Touch.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 11/14/10.
 *  Copyright 2010 Ninja Unicorn. All rights reserved.
 *
 */

#include "Touch.h"

#ifdef ONWIRE_IOS

Touch *get_touch_set(NSSet *touches)
{
	Touch *f = new Touch[touches.count];
	unsigned int i = 0;
	for (UITouch *touch in touches)
	{
		CGPoint p = [[CCDirector sharedDirector] convertToGL:[touch locationInView:[touch view]]];
		f[i].hash = touch.hash;
		f[i].x = p.x;
		f[i++].y = p.y;
	}
	return f;
}

// CFDictionaryKeyCallBacks

const CFDictionaryKeyCallBacks kHashDictionaryKeyCallBacks = { 0, 0, 0, uint_copy_description, uint_equal, uint_hash };

Boolean uint_equal(const void *a, const void *b)
{
	return reinterpret_cast<unsigned int>(a) == reinterpret_cast<unsigned int>(b);
}

CFHashCode uint_hash(const void *ptr)
{
	return static_cast<const Touch *>(ptr)->hash;
}

CFStringRef uint_copy_description(const void *ptr)
{
	return CFStringCreateWithFormat(0, 0, CFSTR("%d"), static_cast<const Touch *>(ptr)->hash);
}

// CFDictionaryValueCallBacks

const CFDictionaryValueCallBacks kTouchDictionaryValueCallBacks = { 0, touch_retain, touch_release, touch_copy_description, touch_equal };

const void *touch_retain(CFAllocatorRef a, const void *ptr)
{
	Touch *f = static_cast<Touch *>(CFAllocatorAllocate(a, sizeof(Touch), 0));
	const Touch *p = static_cast<const Touch *>(ptr);
	f->hash = p->hash;
	f->x = p->x;
	f->y = p->y;
	return f;
}

void touch_release(CFAllocatorRef a, const void *ptr)
{
	//CFAllocatorDeallocate(a, (Touch *)ptr);
	CFAllocatorDeallocate(a, reinterpret_cast<Touch *>(const_cast<void *>(ptr)));
}

Boolean touch_equal(const void *a, const void *b)
{
	return static_cast<const Touch *>(a)->hash == static_cast<const Touch *>(b)->hash;
}

CFStringRef touch_copy_description(const void *ptr)
{
	const Touch *p = static_cast<const Touch *>(ptr);
	return CFStringCreateWithFormat(0, 0, CFSTR("Touch #%d at (%d,%d)"), p->hash, static_cast<int>(p->x), static_cast<int>(p->y));
}

#endif
