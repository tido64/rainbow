// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

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
