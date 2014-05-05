// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Services/TestFlight.h"

#include <lua.hpp>

#include "Common/Debug.h"
#include "Lua/LuaMacros.h"

#ifdef USE_TESTFLIGHT_SDK
#	ifdef RAINBOW_OS_IOS
#		import <TestFlight/TestFlight.h>
#	endif
#endif

namespace
{
	int log(lua_State *L)
	{
		LUA_ASSERT(lua_isstring(L, 1), "TestFlight.log(message)");

		const char *message = lua_tostring(L, 1);
	#ifdef USE_TESTFLIGHT_SDK
	#ifdef RAINBOW_OS_IOS
		TFLog(@"%s", message);
	#endif  // RAINBOW_OS_IOS
	#else
		R_DEBUG("%s\n", message);
	#endif  // USE_TESTFLIGHT_SDK
		return 0;
		static_cast<void>(message);
	}

	int pass_checkpoint(lua_State *L)
	{
		LUA_ASSERT(lua_isstring(L, 1),
		           "TestFlight.pass_checkpoint(checkpoint)");

		const char *checkpoint = lua_tostring(L, 1);
	#ifdef USE_TESTFLIGHT_SDK
	#ifdef RAINBOW_OS_IOS
		NSString *checkpointName = [[NSString alloc]
		    initWithBytesNoCopy:(void*)checkpoint
		                 length:strlen(checkpoint)
		               encoding:NSUTF8StringEncoding
		           freeWhenDone:NO];
		[::TestFlight passCheckpoint:checkpointName];
	#endif  // RAINBOW_OS_IOS
	#else
		R_DEBUG("Passed checkpoint: \"%s\"\n", checkpoint);
	#endif  // USE_TESTFLIGHT_SDK
		return 0;
		static_cast<void>(checkpoint);
	}
}

namespace Rainbow
{
	namespace Services
	{
		namespace TestFlight
		{
			void init(lua_State *L)
			{
				lua_createtable(L, 0, 2);
				luaR_rawsetcclosurefield(L, &log, "log");
				luaR_rawsetcclosurefield(L, &pass_checkpoint, "pass_checkpoint");
				lua_setglobal(L, "testflight");
			}
		}
	}
}
