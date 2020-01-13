// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/JavaScript/JavaScript.h"

#include "FileSystem/Bundle.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"
#include "Script/JavaScript/Audio.h"
#include "Script/JavaScript/Console.h"
#include "Script/JavaScript/Helper.h"
#include "Script/JavaScript/Input.h"
#include "Script/JavaScript/Module.h"
#include "Script/JavaScript/Modules.g.h"
#include "Script/JavaScript/RenderQueue.h"

#define ENSURE(x)                                                              \
    if (!(x))                                                                  \
    terminate(ErrorCode::ErrorExecutingScript)

using rainbow::Director;
using rainbow::GameBase;
using rainbow::JavaScript;
using rainbow::Pointer;
using rainbow::Vec2i;

namespace
{
    [[noreturn]] void on_fatal(void* udata, const char* msg)
    {
        if (msg != nullptr)
            LOGF("JavaScript: %s", msg);

        auto js = static_cast<JavaScript*>(udata);
        rainbow::duk::dump_context(js->context());

        std::terminate();
    }
}  // namespace

rainbow::duk::Context::Context(void* udata)
    : context_(duk_create_heap(nullptr, nullptr, nullptr, udata, &on_fatal))
{
}

rainbow::duk::Context::~Context()
{
    if (context_ == nullptr)
        return;

    duk_destroy_heap(context_);
}

JavaScript::JavaScript(Director& director) : GameBase(director), context_(this)
{
    if (context_ == nullptr)
        return;

    duk::module::initialize(context_);
    duk::console::initialize(context_);

    R_ASSERT(duk_get_top(context_) == 0, "We didn't clean up properly!");

    const auto rainbow = duk_push_bare_object(context_);
    duk::register_module(context_, rainbow, "Audio", &duk::initialize_audio);
    duk::register_module(context_, rainbow, "Input", [this](duk_context* ctx) {
        duk::initialize_input(ctx, input());
    });
    duk::register_module(
        context_, rainbow, "RenderQueue", [this](duk_context* ctx) {
            duk::initialize_renderqueue(ctx, render_queue());
        });
    duk::register_all_modules(context_, rainbow);
    duk_freeze(context_, rainbow);
    duk_put_global_literal(context_, "Rainbow");

    R_ASSERT(duk_get_top(context_) == 0, "We didn't clean up properly!");

    duk_push_global_object(context_);
    duk::get_prop_literal(context_, -1, "Rainbow");
    duk::get_prop_literal(context_, -1, "Sprite");
    duk_push_global_stash(context_);
    duk::get_prop_literal(context_, -2, "prototype");
    duk_put_prop_index(context_, -2, DUKR_IDX_SPRITE_PROTOTYPE);
    duk_pop_n(context_, 4);

    R_ASSERT(duk_get_top(context_) == 0, "We didn't clean up properly!");

    auto& bundle = filesystem::bundle();
    auto index_js = bundle.main_script();
    if (index_js == nullptr)
    {
        constexpr std::array<czstring, 3> kEntryPoints{
            "index.js",
            "index.bs.js",
            "main.jsbundle",
        };

        auto end = std::end(kEntryPoints);
        auto i = std::find_if(std::begin(kEntryPoints), end, [](auto&& path) {
            return filesystem::exists(path);
        });
        if (i == end)
        {
            LOGE("No script was found at these locations:");
            auto assets_path = is_empty(bundle.assets_path())
                                   ? "assets"
                                   : bundle.assets_path();
            for (auto&& path : kEntryPoints)
            {
                LOGE("    %s%s%s",
                     assets_path,
                     filesystem::path_separator(),
                     path);
            }
            terminate(ErrorCode::ScriptNotFound);
            return;
        }

        index_js = *i;
    }

    duk::push(context_, index_js);

    const auto data = File::read(index_js, FileType::Asset);
    if (duk_pcompile_lstring_filename(context_,
                                      DUK_COMPILE_STRICT,
                                      data.as<const char*>(),
                                      data.size()) != 0)
    {
        LOGF("JavaScript: %s", duk_safe_to_string(context_, -1));
        terminate(ErrorCode::ScriptCompilationFailed);
    }
    else
    {
        duk_pcall(context_, 0);
    }

    duk_pop(context_);
}

auto JavaScript::update_controller_id(uint32_t port) -> bool
{
    const auto controller_id = input().controller_states()[port].id();
    duk::update_controller_id(context_, port, controller_id);
    return true;
}

void JavaScript::init_impl(const Vec2i& screen_size)
{
    input().subscribe(*this);
    ENSURE(duk::call(context_, "init", screen_size.x, screen_size.y));
}

void JavaScript::update_impl(uint64_t dt)
{
    ENSURE(duk::call(context_, "update", dt));

    if (has_pointer_events_)
    {
        has_pointer_events_ = false;
        duk::clear_pointer_events(context_);
    }
}

void JavaScript::on_memory_warning_impl()
{
    ENSURE(duk::opt_call(context_, "onMemoryWarning"));

    // From Duktape documentation:
    // You may want to call this function twice to ensure even objects with
    // finalizers are collected. See http://duktape.org/api.html#duk_gc for more
    // details.
    duk_gc(context_, DUK_GC_COMPACT);
    duk_gc(context_, DUK_GC_COMPACT);
}

auto JavaScript::on_controller_connected_impl(uint32_t port) -> bool
{
    return update_controller_id(port);
}

auto JavaScript::on_controller_disconnected_impl(uint32_t port) -> bool
{
    return update_controller_id(port);
}

auto JavaScript::on_pointer_began_impl(const ArrayView<Pointer>& pointers)
    -> bool
{
    has_pointer_events_ = true;
    duk::update_pointer_event(context_, "pointersDown", pointers);
    return true;
}

auto JavaScript::on_pointer_canceled_impl() -> bool
{
    has_pointer_events_ = false;
    duk::clear_pointer_events(context_);
    return true;
}

auto JavaScript::on_pointer_ended_impl(const ArrayView<Pointer>& pointers)
    -> bool
{
    has_pointer_events_ = true;
    duk::update_pointer_event(context_, "pointersUp", pointers);
    return true;
}

auto JavaScript::on_pointer_moved_impl(const ArrayView<Pointer>& pointers)
    -> bool
{
    has_pointer_events_ = true;
    duk::update_pointer_event(context_, "pointersMoved", pointers);
    return true;
}

#if 1
auto GameBase::create(rainbow::Director& director) -> std::unique_ptr<GameBase>
{
    return std::make_unique<JavaScript>(director);
}
#endif
