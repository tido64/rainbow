// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUADEBUGGING_H_
#define LUA_LUADEBUGGING_H_

#include <limits>

#include <lua.hpp>

#include "Common/Logging.h"
#include "Lua/LuaMacros.h"

NS_RAINBOW_LUA_BEGIN
{
    namespace
    {
        const int kLuaMaxStack = 256;

        const char* const kLuaHookNames[]{
            "call", "return", "line", "count", "tail call"};

        struct Line
        {
            int currentline;
            int nparams;
            const char* source;
        };

        char g_command = 0;
        int g_finish = std::numeric_limits<int>::max();
        int g_level = -1;
        Line g_callstack[kLuaMaxStack] = {{0, 0, nullptr}};
    }

    /// <summary>Dumps stack traceback to stdout.</summary>
    void backtrace(lua_State* L);

    /// <summary>Dumps locals to stdout.</summary>
    void dump_locals(lua_State* L, lua_Debug* ar);

    /// <summary>Dumps stack to stdout.</summary>
    void dump_stack(lua_State* L);

    /// <summary>
    ///   Passed to <see cref="lua_sethook"/>. Not to be called directly.
    /// </summary>
    void lua_Hook(lua_State* L, lua_Debug* ar);

    /// <summary>Prints the value at index <paramref name="n"/>.</summary>
    void print_value(lua_State* L, int n);

    void backtrace(lua_State* L)
    {
        int level = 0;
        lua_Debug ar;
        puts("* stack traceback");
        while (lua_getstack(L, level, &ar))
        {
            lua_getinfo(L, "nSl", &ar);
            printf("    frame #%i: %s at %s:%d\n",
                   level,
                   (ar.name ? ar.name : "[internal function]"),
                   ar.source,
                   ar.currentline);
            ++level;
        }
    }

    void dump_locals(lua_State* L, lua_Debug* ar)
    {
        int i = 0;
        const char* local = lua_getlocal(L, ar, ++i);
        while (local)
        {
            if (*local == '(')
            {
                lua_pop(L, 1);
                break;
            }
            if (i == 1)
                printf("* locals\n");
            printf("    %i: %s = ", i, local);
            print_value(L, -1);
            printf("\n");
            lua_pop(L, 1);
            local = lua_getlocal(L, ar, ++i);
        }
    }

    void dump_stack(lua_State* L)
    {
        const int top = lua_gettop(L);
        if (top == 0)
            return;

        printf("* stack\n");
        for (int i = 1; i <= top; ++i)
        {
            printf("    %i: ", i);
            print_value(L, i);
            printf("\n");
        }
    }

    void lua_Hook(lua_State* L, lua_Debug* ar)
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
                        g_finish = std::numeric_limits<int>::max();
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
                        LOGE("Max stack size reached!");
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
                            const char* local = lua_getlocal(L, ar, ++i);
                            while (local && *local != '(')
                                local = lua_getlocal(L, ar, ++i);
                            g_callstack[g_level].nparams = i - 1;
                        }
                        g_callstack[g_level].currentline = ar->currentline;
                        g_callstack[g_level].source = ar->source;

                        // Jump over require() and any C-calls
                        jump = (ar->name && strcmp("require", ar->name) == 0) ||
                               *ar->what == 'C';
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
        printf("* %s ", kLuaHookNames[ar->event]);
        if (ar->name)
        {
            printf("%s(", ar->name);

            const int nparams =
                (ar->event == 1 ? g_callstack[g_level + 1].nparams
                                : g_callstack[g_level].nparams);
            if (*ar->what == 'C')
            {
                for (int i = 1; i <= nparams; ++i)
                {
                    if (i == 1)
                        printf("[1]=");
                    else
                        printf(", [%i]=", i);
                    print_value(L, i);
                }
            }
            else
            {
                for (int i = 1; i <= nparams; ++i)
                {
                    const char* local = lua_getlocal(L, ar, i);
                    if (lua_type(L, -1) == LUA_TFUNCTION)
                    {
                        lua_pop(L, 1);
                        break;
                    }
                    else if (i == 1)
                        printf("%s=", local);
                    else
                        printf(", %s=", local);
                    print_value(L, -1);
                    lua_pop(L, 1);
                }
            }
            printf(") ");
        }

        if (jump)
        {
            g_finish = g_level - 1;
            printf("at %s:%i\n",
                   g_callstack[g_finish].source,
                   g_callstack[g_finish].currentline);
            return;
        }
        else
        {
            printf("at %s:%i\n",
                   g_callstack[g_level].source,
                   g_callstack[g_level].currentline);
        }

        char input[16];
        while (true)
        {
            printf("(rdb) ");
            if (!fgets(input, sizeof(input), stdin))
                break;
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
                    printf("Available debugger commands\n\n");
                    printf("backtrace  -- Dump stack traceback.\n");
                    printf("continue   -- Continue execution.\n");
                    printf("finish     -- Finish the current function call.\n");
                    printf("next       -- Step over current line.\n");
                    printf("print      -- Dump local variables.\n");
                    printf("quit       -- Abort execution.\n");
                    printf("stack      -- Dump the stack.\n\n");
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

    void print_value(lua_State* L, int n)
    {
        switch (lua_type(L, n))
        {
            case LUA_TNIL:
                printf("nil");
                break;
            case LUA_TNUMBER:
                printf(LUA_NUMBER_FMT, lua_tonumber(L, n));
                break;
            case LUA_TBOOLEAN:
                printf("%s", (lua_toboolean(L, n) ? "true" : "false"));
                break;
            case LUA_TSTRING:
                printf("\"%s\"", lua_tostring(L, n));
                break;
            case LUA_TTABLE:
                printf("[table]");
                break;
            case LUA_TFUNCTION:
                printf("[function: %p]", lua_topointer(L, n));
                break;
            case LUA_TUSERDATA:
                printf("[userdata: %p]", lua_touserdata(L, n));
                break;
            case LUA_TTHREAD:
                printf("[thread]");
                break;
            case LUA_TLIGHTUSERDATA:
                printf("[lightuserdata: %p]", lua_topointer(L, n));
                break;
            default:
                printf("[unknown]");
                break;
        }
    }
} NS_RAINBOW_LUA_END

#endif
