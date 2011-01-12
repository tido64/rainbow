//
//  Controls.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 6/27/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include <Rainbow/Hardware/Controls.h>

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

void Controls::touch_began(const Touch *touches, const unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		this->touches.insert(*touches);
		CCLOG(@"Touch with id #%i detected at (%i,%i)", touches->hash, touches->x, touches->y);
		++touches;
	}
	CCLOG(@"Touch count: %u", this->touches.size());
}

void Controls::touch_canceled()
{
	this->touches.clear();
}

void Controls::touch_ended(const Touch *touches, const unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		this->touches.erase(this->touches.find(*touches));
		++touches;
	}
	CCLOG(@"Touch count: %u", this->touches.size());
}

void Controls::touch_moved(const Touch *touches, const unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		//const Touch &t = *this->touches.find(*touches);
		CCLOG(@"Touch with id #%i moved to (%i,%i)", touches->hash, touches->x, touches->y);
		++touches;
	}
}
