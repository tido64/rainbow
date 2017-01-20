// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_RECORDER_H_
#define LUA_RECORDER_H_

#include "ConFuoco/Recorder.h"
#include "Lua/LuaBind.h"

NS_RAINBOW_LUA_BEGIN
{
    class Recorder : private Bind<Recorder>
    {
    public:
        static constexpr bool is_constructible = true;
        static const char class_name[];
        static const luaL_Reg functions[];

        Recorder(lua_State*);

    private:
        static int get_average_power(lua_State*);
        static int get_low_pass(lua_State*);
        static int get_peak_power(lua_State*);
        static int pause(lua_State*);
        static int record(lua_State*);
        static int stop(lua_State*);
        static int update(lua_State*);

        ConFuoco::Recorder recorder;
    };
} NS_RAINBOW_LUA_END

#endif
