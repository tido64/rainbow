#ifndef LUA_PLATFORM_H_
#define LUA_PLATFORM_H_

#include <lua.hpp>

#include "Platform.h"
#if defined(RAINBOW_ANDROID) || defined(RAINBOW_UNIX)
#	include <unistd.h>
#elif defined(RAINBOW_IOS)
#	include <sys/types.h>
#	include <sys/sysctl.h>
#elif defined(RAINBOW_WIN)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

namespace Rainbow
{
	namespace Lua
	{
		class Platform
		{
			friend void init(lua_State *);

		public:
			/// Updates rainbow.platform.screen
			static void update(lua_State *L, const unsigned int width, const unsigned int height)
			{
				lua_getglobal(L, "rainbow");
				lua_getfield(L, -1, "platform");
				lua_getfield(L, -1, "screen");

				lua_pushnumber(L, width);
				lua_setfield(L, -2, "width");

				lua_pushnumber(L, height);
				lua_setfield(L, -2, "height");

				lua_pop(L, 3);
			}

		private:
			static void init(lua_State *L)
			{
				lua_createtable(L, 0, 4);
				lua_pushvalue(L, -1);
				lua_setfield(L, -3, "platform");

				lua_pushboolean(L, RAINBOW_ACCELERATED);
				lua_setfield(L, -2, "accelerometer");

				// Retrieve physical memory
				{
					int memory = static_cast<unsigned int>(-1) >> 1;
					R_ASSERT(memory > 0, "Failed to determine INT_MAX");

				#if defined(RAINBOW_ANDROID) || defined(RAINBOW_UNIX)

					size_t phys = sysconf(_SC_PAGE_SIZE) / 1024;
					phys *= sysconf(_SC_PHYS_PAGES) / 1024;
					if (phys < static_cast<size_t>(memory))
						memory = phys;

				#elif defined(RAINBOW_IOS)

					int mib[2] = { CTL_HW, HW_MEMSIZE };
					int64_t memsize;
					size_t length = sizeof(memsize);
					sysctl(mib, 2, &memsize, &length, nullptr, 0);
					memsize /= 1024 * 1024;
					if (memsize < static_cast<int64_t>(memory))
						memory = memsize;

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
				lua_setfield(L, -2, "memory");

				// rainbow.platform.screen
				lua_createtable(L, 0, 2);
				lua_pushvalue(L, -1);
				lua_setfield(L, -3, "screen");

				lua_pushboolean(L, RAINBOW_TOUCHED);
				lua_setfield(L, -2, "touch");

				lua_pop(L, 2);
			}

			Platform();
			Platform(const Platform &);
			Platform& operator=(const Platform &);
		};
	}
}

#endif
