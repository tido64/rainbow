// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/Gatekeeper.h"

#include "Common/Data.h"
#include "Common/String.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"
#include "Input/Pointer.h"
#include "Resources/Inconsolata.otf.h"
#include "Resources/NewsCycle-Regular.ttf.h"

using heimdall::Gatekeeper;
using rainbow::is_equal;
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
    , monitor_(Path::current())
#endif  // USE_LUA_SCRIPT
{}

void Gatekeeper::init(const Vec2i& screen)
{
    pre_init(screen);
    Director::init(screen);
    if (terminated())
        return;

    post_init();
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

    if (!overlay_.is_visible())
        overlay_activator_.update(dt);
    update_components();
    scenegraph_.update(dt);
}

void Gatekeeper::post_init()
{
    const Vec2i& res = renderer().resolution();
    const unsigned int pt = res.y / 64;
    auto console_font = make_shared<FontAtlas>(  //
        "rainbow://assets/Inconsolata.otf",
        Data::from_bytes(Inconsolata_otf),
        pt);
    auto ui_font = make_shared<FontAtlas>(  //
        "rainbow://assets/NewsCycleRegular.ttf",
        Data::from_bytes(NewsCycle_Regular_ttf),
        (pt << 1) + (pt >> 1));
    const float y = res.y - console_font->height();
    const Vec2f position(
        res.x / 128, y - console_font->height() - ui_font->height());
    perf_->init_button(position, std::move(ui_font));
    perf_->init_graph(std::move(console_font));
    overlay_.add_child(perf_->button().drawable());

#if USE_LUA_SCRIPT
    monitor_.set_callback([this](const char* path) {
        auto file = rainbow::make_string_copy(path);
        std::lock_guard<std::mutex> lock(changed_files_mutex_);
        changed_files_.emplace(std::move(file));
    });
#endif  // USE_LUA_SCRIPT
}

void Gatekeeper::pre_init(const Vec2i& screen)
{
    perf_ = std::make_unique<PerformanceOverlay>();
    scenegraph_.add_child(perf_->node());

    overlay_.init(scenegraph_, screen);

    input().subscribe(this);
    input().subscribe(&overlay_activator_);
}

void Gatekeeper::update_components()
{
    const auto& projection = Renderer::Get()->projection();
    if (projection != projection_)
    {
        projection_ = projection;
        if (overlay_.is_visible())
            overlay_.update(projection);
        perf_->set_origin(projection.bottom_left());
    }
}

bool Gatekeeper::on_pointer_began_impl(const ArrayView<Pointer>& pointers)
{
    for (auto&& p : pointers)
    {
        if (perf_->button().hit_test(Vec2i(p.x, p.y)))
            pressed_[p.hash] = &perf_->button();
    }
    return overlay_.is_visible();
}

bool Gatekeeper::on_pointer_canceled_impl()
{
    pressed_.clear();
    return overlay_.is_visible();
}

bool Gatekeeper::on_pointer_ended_impl(const ArrayView<Pointer>& pointers)
{
    if (overlay_.is_visible() && !overlay_activator_.is_activated())
    {
        if (!pressed_.empty())
        {
            for (auto&& p : pointers)
            {
                auto button = pressed_.find(p.hash);
                if (button != pressed_.end())
                {
                    if (button->second->hit_test(Vec2i(p.x, p.y)))
                        button->second->press();
                    pressed_.erase(button);
                }
            }
        }
        else
            overlay_.hide();
        return true;
    }
    else if (!pressed_.empty())
        pressed_.clear();

    return false;
}

bool Gatekeeper::on_pointer_moved_impl(const ArrayView<Pointer>&)
{
    return overlay_.is_visible();
}

#if USE_LUA_SCRIPT
Library::Library(const char* path) : path_(path)
{
    string_view filename = Path::basename(path_);
    if (filename.length() < 5 || !rainbow::ends_with(filename, ".lua"))
    {
        path_ = nullptr;
        return;
    }

    name_ = string_view{filename.data(), filename.length() - 4};
}

Data Library::open() const
{
#if defined(RAINBOW_OS_MACOS)
    return Data(File::open(path_));
#elif defined(RAINBOW_OS_WINDOWS)
    return Data::load_asset(path_);
#else
    return {};
#endif  // RAINBOW_OS_MACOS
}
#endif  // USE_LUA_SCRIPT
