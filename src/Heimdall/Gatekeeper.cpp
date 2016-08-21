// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/Gatekeeper.h"

#include "Common/Data.h"
#include "Common/String.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"
#include "Input/Pointer.h"

using heimdall::Gatekeeper;
using rainbow::string_view;

#if USE_LUA_SCRIPT
#include "Lua/LuaHelper.h"
#include "Lua/LuaScript.h"

namespace
{
    class Library
    {
    public:
        explicit Library(const char* path);

        auto name() const -> const string_view& { return name_; }
        auto open() const -> Data;

        explicit operator bool() const { return path_ != nullptr; }

    private:
        string_view name_;
        const char* path_;
    };
}
#endif  // USE_LUA_SCRIPT

Gatekeeper::Gatekeeper()
    : overlay_activator_(&overlay_)
#if USE_LUA_SCRIPT
    , monitor_(rainbow::filesystem::current_path())
#endif  // USE_LUA_SCRIPT
{}

void Gatekeeper::init(const Vec2i& screen)
{
    overlay_.initialize(scenegraph_);
    input().subscribe(&overlay_);
    input().subscribe(&overlay_activator_);

    Director::init(screen);
    if (terminated())
        return;

#if USE_LUA_SCRIPT
    monitor_.set_callback([this](const char* path) {
        auto file = rainbow::make_string_copy(path);
        std::lock_guard<std::mutex> lock(changed_files_mutex_);
        changed_files_.emplace(std::move(file));
    });
#endif  // USE_LUA_SCRIPT
}

void Gatekeeper::update(unsigned long dt)
{
#if USE_LUA_SCRIPT
    lua_State* L = static_cast<LuaScript*>(script())->state();
    while (!changed_files_.empty())
    {
        std::unique_ptr<char[]> file;
        {
            std::lock_guard<std::mutex> lock(changed_files_mutex_);
            file = std::move(changed_files_.front());
            changed_files_.pop();
        }

        const Library library(file.get());
        if (!library)
            continue;

        LOGI("Reloading '%s'...", library.name().data());
        rainbow::lua::reload(L, library.open(), library.name());
    }
#endif  // USE_LUA_SCRIPT

    Director::update(dt);

    if (!overlay_.is_enabled())
        overlay_activator_.update(dt);

    scenegraph_.update(dt);
}

#if USE_LUA_SCRIPT
Library::Library(const char* path) : path_(path)
{
    string_view filename = rainbow::filesystem::absolute(path).filename();
    if (filename.length() < 5 || !rainbow::ends_with(filename, ".lua"))
    {
        path_ = nullptr;
        return;
    }

    name_ = string_view{filename.data(), filename.length() - 4};
}

auto Library::open() const -> Data
{
#if defined(RAINBOW_OS_MACOS)
    return Data{File::open(rainbow::filesystem::absolute(path_))};
#elif defined(RAINBOW_OS_WINDOWS)
    return Data::load_asset(path_);
#else
    return {};
#endif  // RAINBOW_OS_MACOS
}
#endif  // USE_LUA_SCRIPT
