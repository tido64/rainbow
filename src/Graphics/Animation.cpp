/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Common/Chrono.h"
#include "Graphics/Animation.h"

float Animation::get_progress()
{
	float p = 0.0f;
	if (!this->tm_start)
		this->tm_start = Chrono::Instance().current();
	else
		p = (Chrono::Instance().current() - this->tm_start) * this->duration;
	return (p >= 1.0f) ? 1.0f : p;
}
