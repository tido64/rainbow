// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Nim/Nim.h"

#ifdef __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#include "nimcache/main.h"
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif

void Nim::init(const Vec2i& screen)
{
	NimMain();
	::init(&scenegraph(), screen.x, screen.y);
	input().subscribe(this);
}

void Nim::update(const unsigned long dt) { ::update(dt); }

void Nim::on_memory_warning() {}

bool Nim::on_key_down_impl(const Key&) { return true; }

bool Nim::on_key_up_impl(const Key&) { return true; }

bool Nim::on_pointer_began_impl(const unsigned int, const Pointer*)
{
	return true;
}

bool Nim::on_pointer_canceled_impl() { return true; }

bool Nim::on_pointer_ended_impl(const unsigned int, const Pointer*)
{
	return true;
}

bool Nim::on_pointer_moved_impl(const unsigned int, const Pointer*)
{
	return true;
}

GameBase* GameBase::create(rainbow::Director& director)
{
	return new Nim(director);
}
