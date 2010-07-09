/*
 *  Sprite.h
 *  OnWire
 *
 *	Platform independent sprite super class.
 *
 *  Created by Tommy Nguyen on 6/28/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#include "Platform.h"

#if defined(ONWIRE_ANDROID)
#	// Insert definitions
#elif defined(ONWIRE_IOS)
#	define Point(x, y) CGPointMake((x), (y))
#	define SpriteObject CCSprite
#endif

class Sprite
{
public:
	const float accel_x, accel_y;

	Sprite(const char *gfx, const float x = 0, const float y = 0) : accel_x(x), accel_y(y), pos_x(0), pos_y(0)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		this->sprite = [CCSprite spriteWithFile: [NSString stringWithUTF8String:gfx]];

	#endif
	}

	~Sprite() { delete this->sprite; }

	void accelerate(const float x, const float y)
	{
		this->set_position(
			this->pos_x - (y * this->accel_x),
			this->pos_y + (x * this->accel_y)
		);
	}

	SpriteObject *get_object() { return this->sprite; }

	void scale(const float x, const float y)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		this->sprite.scaleX = x;
		this->sprite.scaleY = y;

	#endif
	}

	void set_position(const float x, const float y)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		this->sprite.position = ccp(x, y);

	#endif

		this->pos_x = x;
		this->pos_y = y;
	}

	virtual void update() = 0;

protected:
	float pos_x, pos_y;
	SpriteObject *sprite;
};

#endif
