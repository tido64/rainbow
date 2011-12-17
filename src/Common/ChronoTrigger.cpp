#include "Common/ChronoTrigger.h"

ChronoTrigger::~ChronoTrigger() { }

void ChronoTrigger::update()
{
	if (this->stopped)
		return;

	this->accumulated += Chrono::Instance().diff();
	if (this->accumulated >= this->trigger)
	{
		this->tick();
		this->accumulated = 0;
	}
}
