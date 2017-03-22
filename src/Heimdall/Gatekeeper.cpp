// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
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
using rainbow::czstring;

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
    : overlay_(director_.render_queue()), overlay_activator_(&overlay_)
#if USE_LUA_SCRIPT
    , monitor_(rainbow::filesystem::assets_path())
#endif  // USE_LUA_SCRIPT
{
}

void Gatekeeper::init(const Vec2i& screen)
{
    overlay_.initialize();
    director_.input().subscribe(overlay_);
    director_.input().subscribe(overlay_activator_);

    director_.init(screen);
    if (director_.terminated())
        return;

#if USE_LUA_SCRIPT
    monitor_.set_callback([this](czstring path) {
        auto p = rainbow::filesystem::absolute(path);
        std::string filename = p.filename();
        if (filename.length() < 5 || !rainbow::ends_with(filename, ".lua"))
            return;

        changed_files_->emplace([
            this,
            path = std::move(p),
            module = std::move(filename)
        ]() mutable {
            LOGI("Reloading '%s'...", module.c_str());
            module.erase(module.length() - 4, 4);
            auto L = static_cast<LuaScript*>(director_.script())->state();
            rainbow::lua::reload(L, open_module(path), module.c_str());
        });
    });
#endif  // USE_LUA_SCRIPT
}

void Gatekeeper::update(uint64_t dt)
{
#if USE_LUA_SCRIPT
    changed_files_.invoke([](auto& changed_files) {
        while (!changed_files->empty())
        {
            std::function<void()> reload_module =
                std::move(changed_files->front());
            changed_files->pop();
            changed_files.invoke_unlocked(
                [&reload_module](auto&) { reload_module(); });
        }
    });
#endif  // USE_LUA_SCRIPT

    director_.update(dt);

    if (!overlay_.is_enabled())
        overlay_activator_.update(dt);

    overlay_.update(dt);
}
