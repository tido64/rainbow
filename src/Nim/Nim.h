// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef NIM_NIM_H_
#define NIM_NIM_H_

#include "Input/InputListener.h"
#include "Script/GameBase.h"

class Nim final : public GameBase, public InputListener
{
public:
	Nim(rainbow::Director& director) : GameBase(director) {}

	void init(const Vec2i& screen) override;
	void update(const unsigned long) override;

	void on_memory_warning() override;

private:
	bool on_key_down_impl(const Key&) override;
	bool on_key_up_impl(const Key&) override;

	bool on_pointer_began_impl(const unsigned int, const Pointer*) override;
	bool on_pointer_canceled_impl() override;
	bool on_pointer_ended_impl(const unsigned int, const Pointer*) override;
	bool on_pointer_moved_impl(const unsigned int, const Pointer*) override;
};

#endif
