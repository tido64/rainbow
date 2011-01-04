//
//  Controls.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 6/27/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include <Rainbow/Hardware/Controls.h>

Controls::Controls()
{
#if defined(ONWIRE_ANDROID)

	// controls

#elif defined(ONWIRE_IOS)

	touches = CFDictionaryCreateMutable(0, 3, &kHashDictionaryKeyCallBacks, &kTouchDictionaryValueCallBacks);

#endif
}

Controls::~Controls()
{
#if defined(ONWIRE_ANDROID)

#elif defined(ONWIRE_IOS)

	CFRelease(touches);

#endif
}

void Controls::accelerate(const float x, const float y)
{
	if (this->sprites.size() == 0) return;

	bool should_move = false;
	//float final_x, final_y;

	// Pitch
	if (x > 0.25) // upwards
	{
		should_move = true;
	}
	else if (x < -0.25) // downwards
	{
		should_move = true;
	}

	// Roll
	if (y < -0.25) // right
	{
		should_move = true;
	}
	else if (y > 0.25) // left
	{
		should_move = true;
	}

	if (!should_move) return;

	// Loop through sprites and update their positions
	//for (unsigned int i = 0; i < this->sprite_count; ++i)
	//	(this->sprites + i)->accelerate(x, y);

/*
	if(acceleration.x > 0.25) {  // tilting the device upwards
        destX = currentX - (acceleration.y * kPlayerSpeed);
        destY = currentY + (acceleration.x * kPlayerSpeed);
        shouldMove = YES;
    } else if (acceleration.x < -0.25) {  // tilting the device downwards
        destX = currentX - (acceleration.y * kPlayerSpeed);
        destY = currentY + (acceleration.x * kPlayerSpeed);
        shouldMove = YES;
    } else if(acceleration.y < -0.25) {  // tilting the device to the right
        destX = currentX - (acceleration.y * kPlayerSpeed);
        destY = currentY + (acceleration.x * kPlayerSpeed);
        shouldMove = YES;
    } else if (acceleration.y > 0.25) {  // tilting the device to the left
        destX = currentX - (acceleration.y * kPlayerSpeed);
        destY = currentY + (acceleration.x * kPlayerSpeed);
        shouldMove = YES;
    } else {
        destX = currentX;
        destY = currentY;
    }

 if(shouldMove) {
 CGSize wins = [[CCDirector sharedDirector] winSize];
 // ensure we aren't moving out of bounds
 if(destX < 30 || destX > wins.width - 30 || destY < 30 || destY > wins.height - 100) {
 // do nothing
 } else {
 CCAction *action = [CCMoveTo actionWithDuration:1 position: CGPointMake(destX, destY)];
 [action setTag:kHeroMovementAction];
 [playerSprite runAction:action];
 }
 } else {
 // should stop
 [playerSprite stopActionByTag:kHeroMovementAction];
 }
*/
}

void Controls::add_listener(Sprite *sprite)
{
	this->sprites.add(sprite);
}

void Controls::touch_began(Touch *t, const unsigned int c)
{
#if defined(ONWIRE_ANDROID)

#elif defined(ONWIRE_IOS)

	for (unsigned int i = 0; i < c; ++i)
	{
		CFDictionarySetValue(touches, reinterpret_cast<void *>(t[i].hash), t + i);
		CCLOG(@"Touch with id #%i detected at (%i,%i)", t[i].hash, static_cast<int>(t[i].x), static_cast<int>(t[i].y));
	}
	CCLOG(@"Hash count: %d", CFDictionaryGetCount(touches));

#endif
}

void Controls::touch_canceled()
{
#if defined(ONWIRE_ANDROID)

#elif defined(ONWIRE_IOS)

	CFDictionaryRemoveAllValues(touches);
	CCLOG(@"Stop!");

#endif
}

void Controls::touch_ended(Touch *t, const unsigned int c)
{
#if defined(ONWIRE_ANDROID)

#elif defined(ONWIRE_IOS)

	for (unsigned int i = 0; i < c; ++i)
	{
		CFDictionaryRemoveValue(touches, reinterpret_cast<void *>(t[i].hash));
		CCLOG(@"Finger with id #%i lifted at (%i,%i)", t[i].hash, static_cast<int>(t[i].x), static_cast<int>(t[i].y));
	}
	CCLOG(@"Hash count: %d", CFDictionaryGetCount(touches));

#endif
}

void Controls::touch_moved(Touch *t, const unsigned int c)
{
#if defined(ONWIRE_ANDROID)

#elif defined(ONWIRE_IOS)

	Touch *curr;
	for (unsigned int i = 0; i < c; ++i)
	{
		curr = t + i;
		const Touch *orig = static_cast<const Touch *>(CFDictionaryGetValue(touches, reinterpret_cast<void *>(curr->hash)));
		CCLOG(@"Touch with id #%i moved from (%i,%i) to (%i,%i)", curr->hash,
			static_cast<int>(orig->x), static_cast<int>(orig->y),
			static_cast<int>(curr->x), static_cast<int>(curr->y));
	}

#endif
}
