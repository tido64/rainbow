// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Common/ChronoTrigger.h"

ChronoTrigger::~ChronoTrigger() { }

void ChronoTrigger::update()
{
	if (this->stopped)
		return;

	this->accumulated += Chrono::Instance().diff();
	while (this->accumulated >= this->trigger)
	{
		this->tick();
		this->accumulated -= this->trigger;
	}
}
