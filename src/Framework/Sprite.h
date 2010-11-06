/// Platform independent sprite super class.

/// Sprites aree created by extending this class. Initialised from a bitmap
/// file. Support for JPEG/PNG varies with platform.
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef SPRITE_H_
#define SPRITE_H_

#include "../Hardware/Platform.h"

#if defined(ONWIRE_ANDROID)
#	// Insert definitions
#elif defined(ONWIRE_IOS)

#define Point(x, y) CGPointMake((x), (y))
typedef CCSprite RealSprite;

#endif

class Sprite
{
public:
	const float
		accel_x,	///< Acceleration in x-direction
		accel_y;	///< Acceleration in y-direction

	/// Creates a sprite given a file, and optionally how fast it accelerates.
	/// All sprites are created at (0,0).
	/// \param file Path to bitmap file
	/// \param x Acceleration constant in x-direction
	/// \param y Acceleration constant in y-direction
	Sprite(const char *file, const float x = 0, const float y = 0) :
		accel_x(x), accel_y(y), pos_x(0), pos_y(0)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		this->sprite = [CCSprite spriteWithFile: [NSString stringWithUTF8String:file]];

	#endif
	}

	~Sprite() { delete this->sprite; }

	/// Sets traveling speed.
	/// \param x Speed in x-direction
	/// \param y Speed in y-direction
	void accelerate(const float x, const float y)
	{
		this->set_position(
			this->pos_x - (y * this->accel_x),
			this->pos_y + (x * this->accel_y)
		);
	}

	/// Returns the sprite object. Used by the director.
	/// \return Actual sprite object
	RealSprite *get_object() { return this->sprite; }

	/// Scales the sprite.
	/// \param f Scaling factor
	void scale(const float f)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		this->sprite.scale = f;

	#endif
	}

	/// Scales the sprite.
	/// \param x Scaling factor in x-direction
	/// \param y Scaling factor in y-direction
	void scale(const float x, const float y)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		this->sprite.scaleX = x;
		this->sprite.scaleY = y;

	#endif
	}

	/// Sets the position of the sprite.
	/// \param x The x-coordinate
	/// \param y The y-coordinate
	void set_position(const float x, const float y)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		this->sprite.position = ccp(x, y);

	#endif

		this->pos_x = x;
		this->pos_y = y;
	}

	/// Updates the state of this sprite. Must be overriden.
	virtual void update() = 0;

protected:
	float
		pos_x,				///< Position of this sprite (x-coordinate)
		pos_y;				///< Position of this sprite (y-coordinate)
	RealSprite *sprite;		///< The actual sprite object
};

#endif
