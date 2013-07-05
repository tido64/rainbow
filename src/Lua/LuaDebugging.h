#ifndef LUA_DEBUGGING_H_
#define LUA_DEBUGGING_H_

#include <lua.hpp>

#include "Common/Constants.h"
#include "Common/Debug.h"

namespace Rainbow
{
	namespace Lua
	{
		namespace
		{
			const int kLuaMaxStack = 256;

			const char *const kLuaHookNames[] = {
				"call", "return", "line", "count", "tail call"
			};

			struct Line
			{
				int currentline;
				int nparams;
				const char *source;
			};

			char g_command = 0;
			int g_finish = kMaxInt;
			int g_level = -1;
			Line g_callstack[kLuaMaxStack] = { { 0, 0, nullptr } };
		}

		/// Dump stack traceback to stdout.
		void backtrace(lua_State *L);

		/// Dump locals to stdout.
		void dump_locals(lua_State *L, lua_Debug *ar);

		/// Dump stack to stdout.
		void dump_stack(lua_State *L);

		/// Passed to \c lua_sethook(). Not to be called directly.
		void lua_Hook(lua_State *L, lua_Debug *ar);

		/// Print the value at index \p n.
		void print_value(lua_State *L, const int n);

		void backtrace(lua_State *L)
		{
			int level = 0;
			lua_Debug ar;
			R_ERROR("* stack traceback\n");
			while (lua_getstack(L, level, &ar))
			{
				lua_getinfo(L, "nSl", &ar);
				R_ERROR("    frame #%i: %s at %s:%d\n",
						level,
						ar.name ? ar.name : "[internal function]",
						ar.source,
						ar.currentline);
				++level;
			}
		}

		void dump_locals(lua_State *L, lua_Debug *ar)
		{
			int i = 0;
			const char *local = lua_getlocal(L, ar, ++i);
			while (local)
			{
				if (*local == '(')
				{
					lua_pop(L, 1);
					break;
				}
				if (i == 1)
					R_DEBUG("* locals\n");
				R_DEBUG("    %i: %s = ", i, local);
				print_value(L, -1);
				R_DEBUG("\n");
				lua_pop(L, 1);
				local = lua_getlocal(L, ar, ++i);
			}
		}

		void dump_stack(lua_State *L)
		{
			const int top = lua_gettop(L);
			if (top == 0)
				return;

			R_DEBUG("* stack\n");
			for (int i = 1; i <= top; ++i)
			{
				R_DEBUG("    %i: ", i);
				print_value(L, i);
				R_DEBUG("\n");
			}
		}

		void lua_Hook(lua_State *L, lua_Debug *ar)
		{
			if (lua_getinfo(L, "nSlufL", ar) == 0)
				return;

			bool jump = false;
			if (g_level > g_finish)
			{
				switch (ar->event)
				{
					case 0:  // call
						++g_level;
						return;
					case 1:  // return
						--g_level;
						if (g_level == g_finish)
						{
							g_finish = kMaxInt;
							break;
						}
						// fall through
					default:
						return;
				}
			}
			else
			{
				switch (ar->event)
				{
					case 0:  // call
						++g_level;
						if (g_level >= kLuaMaxStack)
						{
							if (g_level > g_finish)
								return;
							R_ERROR("Max stack size reached!\n");
							g_finish = g_level - 1;
							break;
						}
						else
						{
							if (*ar->what == 'C')
							{
								for (int i = 1; i <= lua_gettop(L); ++i)
								{
									if (lua_type(L, i) == LUA_TFUNCTION)
									{
										g_callstack[g_level].nparams = i - 1;
										break;
									}
								}
							}
							else
							{
								int i = 0;
								const char *local = lua_getlocal(L, ar, ++i);
								while (local && *local != '(')
									local = lua_getlocal(L, ar, ++i);
								g_callstack[g_level].nparams = i - 1;
							}
							g_callstack[g_level].currentline = ar->currentline;
							g_callstack[g_level].source = ar->source;

							// Jump over require() and any C-calls
							jump = (ar->name && strcmp("require", ar->name) == 0)
							       || *ar->what == 'C';
						}
						break;
					case 1:  // return
						--g_level;
						break;
					case 2:  // line
						g_callstack[g_level].currentline = ar->currentline;
						g_callstack[g_level].source = ar->source;
						break;
					case 3:  // count
					case 4:  // tail call
					default:
						break;
				}
			}

			/* (ar->namewhat && ar->namewhat[0] != '\0') ? ar->namewhat : kLuaHookNames[ar->event] */
			R_DEBUG("* %s ", kLuaHookNames[ar->event]);
			if (ar->name)
			{
				R_DEBUG("%s(", ar->name);

				const int nparams = (ar->event == 1)
				                  ? g_callstack[g_level + 1].nparams
				                  : g_callstack[g_level].nparams;
				if (*ar->what == 'C')
				{
					for (int i = 1; i <= nparams; ++i)
					{
						if (i == 1)
							R_DEBUG("[1]=");
						else
							R_DEBUG(", [%i]=", i);
						print_value(L, i);
					}
				}
				else
				{
					for (int i = 1; i <= nparams; ++i)
					{
						const char *local = lua_getlocal(L, ar, i);
						if (lua_type(L, -1) == LUA_TFUNCTION)
						{
							lua_pop(L, 1);
							break;
						}
						else if (i == 1)
							R_DEBUG("%s=", local);
						else
							R_DEBUG(", %s=", local);
						print_value(L, -1);
						lua_pop(L, 1);
					}
				}
				R_DEBUG(") ");
			}

			if (jump)
			{
				g_finish = g_level - 1;
				R_DEBUG("at %s:%i\n", g_callstack[g_finish].source, g_callstack[g_finish].currentline);
				return;
			}
			else
				R_DEBUG("at %s:%i\n", g_callstack[g_level].source, g_callstack[g_level].currentline);

			char input[16];
			while (true)
			{
				R_DEBUG("(rdb) ");
				fgets(input, sizeof(input), stdin);
				// TODO: Handle CTRL+C, CTRL+D, CTRL+Z etc.
				if (input[0] != '\n' && input[0] != '\r')
					g_command = input[0];
				switch (g_command)
				{
					case 'b':
						backtrace(L);
						break;
					case 'c':
						g_level = -1;
						lua_sethook(L, nullptr, 0, 0);
						return;
					case 'f':
						g_finish = g_level - 1;
						return;
					case 'h':
						R_DEBUG("Available debugger commands\n\n");
						R_DEBUG("backtrace  -- Dump stack traceback.\n");
						R_DEBUG("continue   -- Continue execution.\n");
						R_DEBUG("finish     -- Finish the current function call.\n");
						R_DEBUG("next       -- Step over current line.\n");
						R_DEBUG("print      -- Dump local variables.\n");
						R_DEBUG("quit       -- Abort execution.\n");
						R_DEBUG("stack      -- Dump the stack.\n\n");
						break;
					case 'n':
						return;
					case 'p':
						dump_locals(L, ar);
						break;
					case 'q':
						lua_sethook(L, nullptr, 0, 0);
						// TODO: Handle this more delicately.
						luaL_error(L, "user aborted");
						return;
					case 's':
						dump_stack(L);
						break;
					default:
						break;
				}
			}
		}

		void print_value(lua_State *L, const int n)
		{
			switch (lua_type(L, n))
			{
				case LUA_TNIL:
					R_DEBUG("nil");
					break;
				case LUA_TNUMBER:
					R_DEBUG(LUA_NUMBER_FMT, lua_tonumber(L, n));
					break;
				case LUA_TBOOLEAN:
					R_DEBUG("%s", lua_toboolean(L, n) ? "true" : "false");
					break;
				case LUA_TSTRING:
					R_DEBUG("\"%s\"", lua_tostring(L, n));
					break;
				case LUA_TTABLE:
					R_DEBUG("[table]");
					break;
				case LUA_TFUNCTION:
					R_DEBUG("[function: %p]", lua_topointer(L, n));
					break;
				case LUA_TUSERDATA:
					R_DEBUG("[userdata: %p]", lua_touserdata(L, n));
					break;
				case LUA_TTHREAD:
					R_DEBUG("[thread]");
					break;
				case LUA_TLIGHTUSERDATA:
					R_DEBUG("[lightuserdata: %p]", lua_topointer(L, n));
					break;
				default:
					R_DEBUG("[unknown]");
					break;
			}
		}
	}
}

#endif
