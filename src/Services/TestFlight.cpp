#include "Lua/LuaHelper.h"
#include "Services/TestFlight.h"

#ifdef USE_TESTFLIGHT_SDK
#	ifdef RAINBOW_IOS
#		import <TestFlight/TestFlight.h>
#	endif
#endif

namespace Rainbow
{
	namespace Services
	{
		namespace TestFlight
		{
			namespace
			{
				int log(lua_State *L)
				{
					const char *message = luaR_tostring(L, -1);
				#ifdef USE_TESTFLIGHT_SDK
				#ifdef RAINBOW_IOS
					TFLog(@"%s", message);
				#endif  // RAINBOW_IOS
				#else
					R_DEBUG("%s\n", message);
				#endif  // USE_TESTFLIGHT_SDK
					return 0;
					static_cast<void>(message);
				}

				int pass_checkpoint(lua_State *L)
				{
					const char *checkpoint = luaR_tostring(L, -1);
				#ifdef USE_TESTFLIGHT_SDK
				#ifdef RAINBOW_IOS
					NSString *checkpointName = [[NSString alloc]
							initWithBytesNoCopy:(void*)checkpoint
							             length:strlen(checkpoint)
							           encoding:NSUTF8StringEncoding
							       freeWhenDone:NO];
					[::TestFlight passCheckpoint:checkpointName];
				#endif  // RAINBOW_IOS
				#else
					R_DEBUG("Passed checkpoint: \"%s\"\n", checkpoint);
				#endif  // USE_TESTFLIGHT_SDK
					return 0;
					static_cast<void>(checkpoint);
				}
			}

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
