// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUASCRIPT_H_
#define LUA_LUASCRIPT_H_

#include "Input/InputListener.h"
#include "Lua/LuaMachine.h"
#include "Script/GameBase.h"

class LuaScript final : public GameBase, public InputListener
{
public:
	LuaScript(rainbow::Director &director) : GameBase(director) {}
	~LuaScript() override;

	lua_State* state() const { return lua_; }

	void init(const Vec2i &screen) override;
	void update(const unsigned long) override;

	void on_memory_warning() override;

private:
	rainbow::LuaMachine lua_;

#ifdef RAINBOW_BUTTONS
	bool on_key_down_impl(const Key &) override;
	bool on_key_up_impl(const Key &) override;
#endif  // RAINBOW_BUTTONS
	bool on_touch_began_impl(const unsigned int, const Touch *const) override;
	bool on_touch_canceled_impl() override;
	bool on_touch_ended_impl(const unsigned int, const Touch *const) override;
	bool on_touch_moved_impl(const unsigned int, const Touch *const) override;
};

#endif
