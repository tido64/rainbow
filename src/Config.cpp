// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

// You should defenitely not merge this pull request but you should defenitely
// come over to my party on Friday Oct. 7th, 2016. See you at Sorengkaia 132!
// Around 8pm!
#include "Config.h"

#include <memory>

#include "Common/Data.h"
#include "Common/Logging.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"
#include "Lua/LuaHelper.h"

namespace
{
#ifdef RAINBOW_SDL
    constexpr unsigned int kMaxMSAA = 16;
#endif
}

rainbow::Config::Config()
    : accelerometer_(true), high_dpi_(false), suspend_(true), width_(0),
      height_(0), msaa_(0)
{
    constexpr char kConfigModule[] = "config";

    const auto path = filesystem::relative(kConfigModule);
    std::error_code error;
    if (!filesystem::is_regular_file(path, error))
    {
        LOGI("No config file was found");
        return;
    }

    const Data config(File::open(path));
    if (!config)
        return;

    auto L = lua::newstate();
    if (lua::load(L.get(), config, kConfigModule) == 0)
        return;

    lua_getglobal(L.get(), "accelerometer");
    if (lua::isboolean(L.get(), -1))
        accelerometer_ = lua::toboolean(L.get(), -1);

#ifdef RAINBOW_SDL
    lua_getglobal(L.get(), "allow_high_dpi");
    if (lua::isboolean(L.get(), -1))
        high_dpi_ = lua::toboolean(L.get(), -1);

    lua_getglobal(L.get(), "msaa");
    if (lua::isnumber(L.get(), -1))
        msaa_ = std::min(floor_pow2(lua::tointeger(L.get(), -1)), kMaxMSAA);
#endif

    lua_getglobal(L.get(), "resolution");
    if (lua::istable(L.get(), -1))
    {
        lua_rawgeti(L.get(), -1, 1);
        width_ = lua::tointeger(L.get(), -1);
        lua_rawgeti(L.get(), -2, 2);
        height_ = lua::tointeger(L.get(), -1);
    }

#ifdef RAINBOW_SDL
    lua_getglobal(L.get(), "suspend_on_focus_lost");
    if (lua::isboolean(L.get(), -1))
        suspend_ = lua::toboolean(L.get(), -1);
#endif
}
