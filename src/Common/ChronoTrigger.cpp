#include "Common/ChronoTrigger.h"

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
