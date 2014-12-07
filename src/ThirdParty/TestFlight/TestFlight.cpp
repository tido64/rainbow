// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/TestFlight/TestFlight.h"

#include "Common/Logging.h"
#include "Lua/LuaHelper.h"
#include "Lua/LuaSyntax.h"

#ifdef USE_TESTFLIGHT_SDK
#	ifdef RAINBOW_OS_IOS
#		import <TestFlight/TestFlight.h>
#	endif
#endif

namespace
{
	int log(lua_State *L)
	{
		// TestFlight.log(message)
		Rainbow::Lua::Argument<char*>::is_required(L, 1);

		const char *message = lua_tostring(L, 1);
#ifdef USE_TESTFLIGHT_SDK
#ifdef RAINBOW_OS_IOS
		TFLog(@"%s", message);
#endif  // RAINBOW_OS_IOS
#else
		LOGD("%s", message);
#endif  // USE_TESTFLIGHT_SDK
		return 0;
		static_cast<void>(message);
	}

	int pass_checkpoint(lua_State *L)
	{
		// TestFlight.pass_checkpoint(checkpoint)
		Rainbow::Lua::Argument<char*>::is_required(L, 1);

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
		LOGD("Passed checkpoint: \"%s\"", checkpoint);
#endif  // USE_TESTFLIGHT_SDK
		return 0;
		static_cast<void>(checkpoint);
	}
}

namespace TF
{
	namespace Lua
	{
		void init(lua_State *L)
		{
			lua_createtable(L, 0, 2);
			luaR_rawsetcfunction(L, "log", &log);
			luaR_rawsetcfunction(L, "pass_checkpoint", &pass_checkpoint);
			lua_setglobal(L, "testflight");
		}
	}
}
