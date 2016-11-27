// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/Gatekeeper.h"

#if USE_LUA_SCRIPT
#   include "Common/Data.h"
#   include "Common/String.h"
#   include "FileSystem/File.h"
#   include "FileSystem/FileSystem.h"
#   include "Lua/LuaHelper.h"
#   include "Lua/LuaScript.h"

using rainbow::LuaScript;
#endif  // USE_LUA_SCRIPT

using heimdall::Gatekeeper;
using rainbow::Data;
using rainbow::File;
using rainbow::Vec2i;

namespace
{
#if USE_LUA_SCRIPT
    auto open_module(const rainbow::filesystem::Path& path) -> Data
    {
#if defined(RAINBOW_OS_MACOS)
        return Data{File::open(path)};
#elif defined(RAINBOW_OS_WINDOWS)
        return Data::load_asset(path.string().c_str());
#else
        NOT_USED(path);
        return {};
#endif  // RAINBOW_OS_MACOS
    }
#endif  // USE_LUA_SCRIPT
}

Gatekeeper::Gatekeeper()
    : overlay_activator_(&overlay_)
#if USE_LUA_SCRIPT
    , monitor_(rainbow::filesystem::current_path())
#endif  // USE_LUA_SCRIPT
{
}

void Gatekeeper::init(const Vec2i& screen)
{
    overlay_.initialize();
    director_.input().subscribe(&overlay_);
    director_.input().subscribe(&overlay_activator_);

    director_.init(screen);
    if (director_.terminated())
        return;

#if USE_LUA_SCRIPT
    monitor_.set_callback([this](const char* path) {
        auto p = rainbow::filesystem::absolute(path);
        rainbow::string_view filename = p.filename();
        if (filename.length() < 5 || !rainbow::ends_with(filename, ".lua"))
            return;

        const size_t offset = filename.data() - p.c_str();
        const size_t length = filename.length() - 4;
        std::lock_guard<std::mutex> lock(changed_files_mutex_);
        changed_files_.emplace([this, path = std::move(p), offset, length] {
            rainbow::string_view name{path.c_str() + offset, length};
            LOGI("Reloading '%s'...", name.data());
            lua_State* L = static_cast<LuaScript*>(director_.script())->state();
            rainbow::lua::reload(L, open_module(path), name);
        });
    });
#endif  // USE_LUA_SCRIPT
}

void Gatekeeper::update(uint64_t dt)
{
#if USE_LUA_SCRIPT
    while (!changed_files_.empty())
    {
        std::function<void()> reload_module;
        {
            std::lock_guard<std::mutex> lock(changed_files_mutex_);
            reload_module = std::move(changed_files_.front());
            changed_files_.pop();
        }

        reload_module();
    }
#endif  // USE_LUA_SCRIPT

    director_.update(dt);

    if (!overlay_.is_enabled())
        overlay_activator_.update(dt);

    overlay_.update(dt);
}
