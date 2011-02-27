//
//  Controls.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 6/27/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include "Controls.h"

void Controls::accelerate(const Acceleration &a)
{
	static const float threshold = 0.20f;

	bool should_move = false;
	//float final_x, final_y;

	if (a.x > threshold) // right
	{
		should_move = true;
	}
	else if (a.x < -threshold) // left
	{
		should_move = true;
	}

	if (a.y > threshold) // down
	{
		should_move = true;
	}
	else if (a.y < -threshold) // up
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

void Controls::add_touchable(Touchable *minion)
{
	this->touchables.push_back(minion);
}

void Controls::touch_began(const Touch *touches, const unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		this->touches[touches->hash] = *touches;
		//CCLOG(@"Touch id #%i detected at (%i,%i)", touches->hash, touches->x, touches->y);
		++touches;
	}
	//CCLOG(@"Touch count: %u", this->touches.size());

	for (unsigned int i = 0; i < this->touchables.size(); ++i)
		this->touchables[i]->touch_began(this->touches);
}

void Controls::touch_canceled()
{
	for (unsigned int i = 0; i < this->touchables.size(); ++i)
		this->touchables[i]->touch_canceled();
	this->touches.clear();
	//CCLOG(@"Touch count: %u", this->touches.size());
}

void Controls::touch_ended(const Touch *touches, const unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		this->touches.erase(touches->hash);
		++touches;
	}
	//CCLOG(@"Touch count: %u", this->touches.size());
	for (unsigned int i = 0; i < this->touchables.size(); ++i)
		this->touchables[i]->touch_ended(this->touches);
}

void Controls::touch_moved(const Touch *touches, const unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		this->touches[touches->hash] = *touches;

		//Touch *t = &this->touches[touches->hash];
		//CCLOG(@"Touch id #%i: (%i,%i) -> (%i,%i)", t->hash, t->orig.x, t->orig.y, t->x, t->y);

		++touches;
	}
	for (unsigned int i = 0; i < this->touchables.size(); ++i)
		this->touchables[i]->touch_moved(this->touches);
}
