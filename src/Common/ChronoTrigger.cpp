// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

#include "Common/ChronoTrigger.h"

ChronoTrigger::~ChronoTrigger() { }

void ChronoTrigger::update(const unsigned long dt)
{
	if (this->stopped)
		return;

	this->accumulated += dt;
	while (this->accumulated >= this->trigger)
	{
		this->tick();
		this->accumulated -= this->trigger;
	}
}
