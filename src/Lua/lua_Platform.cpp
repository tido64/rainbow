#include "Platform.h"
#if defined(RAINBOW_IOS) || defined(RAINBOW_MAC)
#	include <sys/types.h>
#	include <sys/sysctl.h>
#elif defined(RAINBOW_ANDROID) || defined(RAINBOW_UNIX)
#	include <unistd.h>
#elif defined(RAINBOW_WIN)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

#include "Common/Debug.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_Platform.h"

namespace Rainbow
{
	namespace Lua
	{
		namespace Platform
		{
			void init(lua_State *L)
			{
				lua_pushliteral(L, "platform");
				lua_createtable(L, 0, 4);

				lua_rawsetfield(L, lua_pushboolean, RAINBOW_ACCELERATED, "accelerometer");

				// Retrieve physical memory
				lua_pushliteral(L, "memory");
				{
					int memory = static_cast<unsigned int>(-1) >> 1;
					R_ASSERT(memory > 0, "Failed to determine INT_MAX");

				#if defined(RAINBOW_IOS) || defined(RAINBOW_MAC)

					int mib[2] = { CTL_HW, HW_MEMSIZE };
					int64_t memsize;
					size_t length = sizeof(memsize);
					sysctl(mib, 2, &memsize, &length, nullptr, 0);
					memsize /= 1024 * 1024;
					if (memsize < static_cast<int64_t>(memory))
						memory = memsize;

				#elif defined(RAINBOW_ANDROID) || defined(RAINBOW_UNIX)

					size_t phys = sysconf(_SC_PAGE_SIZE) / 1024;
					phys *= sysconf(_SC_PHYS_PAGES) / 1024;
					if (phys < static_cast<size_t>(memory))
						memory = phys;

				#elif defined(RAINBOW_WIN)

					MEMORYSTATUSEX status;
					status.dwLength = sizeof(status);
					GlobalMemoryStatusEx(&status);
					const size_t phys = status.ullTotalPhys / (1024 * 1024);
					if (phys < static_cast<size_t>(memory))
						memory = phys;

				#endif

					lua_pushinteger(L, memory);
				}
				lua_rawset(L, -3);

				// rainbow.platform.screen
				lua_pushliteral(L, "screen");
				lua_createtable(L, 0, 3);
				lua_rawsetfield(L, lua_pushboolean, RAINBOW_TOUCHED, "touch");
				lua_rawset(L, -3);

				lua_rawset(L, -3);
			}

			void update(lua_State *L, const unsigned int width, const unsigned int height)
			{
				lua_getglobal(L, "rainbow");
				lua_pushliteral(L, "platform");
				lua_rawget(L, -2);
				lua_pushliteral(L, "screen");
				lua_rawget(L, -2);

				lua_rawsetfield(L, lua_pushnumber, width, "width");
				lua_rawsetfield(L, lua_pushnumber, height, "height");

				lua_pop(L, 3);
			}
		}
	}
}
