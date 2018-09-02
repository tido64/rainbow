// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/JavaScript/JavaScript.jsc.h"

using rainbow::GameBase;
using rainbow::JavaScript;

JavaScript::JavaScript(Director& director)
    : GameBase(director), context_group_{JSContextGroupCreate()},
      global_context_{
          JSGlobalContextCreateInGroup(context_group_.get(), nullptr)},
      has_pointer_events_(false)
{
    auto global = JSContextGetGlobalObject(global_context_.get());
    NOT_USED(global);
}

bool JavaScript::update_controller_id(unsigned int i)
{
    NOT_USED(i);
    return true;
}

void JavaScript::init_impl(const Vec2i& screen_size)
{
    NOT_USED(screen_size);
}

void JavaScript::update_impl(uint64_t dt)
{
    NOT_USED(dt);
}

void JavaScript::on_memory_warning_impl() {}

bool JavaScript::on_controller_connected_impl(unsigned int i)
{
    NOT_USED(i);
    return true;
}

bool JavaScript::on_controller_disconnected_impl(unsigned int i)
{
    NOT_USED(i);
    return true;
}

bool JavaScript::on_pointer_began_impl(const ArrayView<Pointer>& pointers)
{
    has_pointer_events_ = true;
    NOT_USED(pointers);
    return true;
}

bool JavaScript::on_pointer_canceled_impl()
{
    has_pointer_events_ = false;
    return true;
}

bool JavaScript::on_pointer_ended_impl(const ArrayView<Pointer>& pointers)
{
    has_pointer_events_ = true;
    NOT_USED(pointers);
    return true;
}

bool JavaScript::on_pointer_moved_impl(const ArrayView<Pointer>& pointers)
{
    has_pointer_events_ = true;
    NOT_USED(pointers);
    return true;
}

auto GameBase::create(rainbow::Director& director) -> std::unique_ptr<GameBase>
{
    return std::make_unique<JavaScript>(director);
}
