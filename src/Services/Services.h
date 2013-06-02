#ifndef SERVICES_SERVICES_H_
#define SERVICES_SERVICES_H_

#include "Services/TestFlight.h"

namespace Rainbow
{
	namespace Services
	{
		void init(lua_State *L)
		{
			TestFlight::init(L);
		}
	}
}

#endif
