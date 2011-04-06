//
//  Input.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 6/27/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include "Input.h"

#ifdef RAINBOW_TOUCHED

void Input::touch_began(const Touch *touches, const unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		this->touches[touches->hash] = *touches;
		++touches;
	}
	this->touched = true;
	//for (unsigned int i = 0; i < this->touchables.size(); ++i)
	//	this->touchables[i]->touch_began(this->touches);
}

void Input::touch_canceled()
{
	//for (unsigned int i = 0; i < this->touchables.size(); ++i)
	//	this->touchables[i]->touch_canceled();
	this->touches.clear();
	this->touched = true;
}

void Input::touch_ended(const Touch *touches, const unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		this->touches.erase(touches->hash);
		++touches;
	}
	//for (unsigned int i = 0; i < this->touchables.size(); ++i)
	//	this->touchables[i]->touch_ended(this->touches);
	this->touched = true;
}

void Input::touch_moved(const Touch *touches, const unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		this->touches[touches->hash] = *touches;
		//Touch *t = &this->touches[touches->hash];
		++touches;
	}
	//for (unsigned int i = 0; i < this->touchables.size(); ++i)
	//	this->touchables[i]->touch_moved(this->touches);
	this->touched = true;
}

#endif
