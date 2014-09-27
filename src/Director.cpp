// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Director.h"

#include <lua.hpp>

#include "Common/Data.h"
#include "Lua/lua_Platform.h"

namespace Rainbow
{
	Director::Director()
	    : active_(true), terminated_(false), error_(nullptr), input_(lua_)
	{
		if (!ConFuoco::Mixer::Instance)
			terminate("Failed to initialise audio engine");
		else if (!renderer_.init())
			terminate("Failed to initialise renderer");
		else if (lua_.init(&scenegraph_) != LUA_OK)
			terminate("Failed to initialise Lua");
	}

	Director::~Director()
	{
		// Lua must clean up before we tear down the graphics context.
		lua_.close();
	}

	void Director::draw()
	{
		Renderer::clear();
		scenegraph_.draw();
	}

	void Director::init(const Data &main, const Vec2i &screen)
	{
		R_ASSERT(main, "Failed to load 'main.lua'");

		renderer_.set_resolution(screen);
		Lua::Platform::update(lua_, screen);
		if (lua_.start(main) != LUA_OK || lua_.update(0) != LUA_OK)
		{
			terminate("Failed to start 'main.lua'");
			return;
		}
		scenegraph_.update(0);
	}

	void Director::update(const unsigned long dt)
	{
		R_ASSERT(!terminated_, "App should have terminated by now");

		mixer_.update();
		if (lua_.update(dt))
		{
			terminate();
			return;
		}
		scenegraph_.update(dt);
		input_.clear();
	}

	void Director::on_focus_gained()
	{
		R_ASSERT(!terminated_, "App should have terminated by now");

		active_ = true;
		mixer_.suspend(false);
	}

	void Director::on_focus_lost()
	{
		active_ = false;
		mixer_.suspend(true);
	}

	void Director::on_memory_warning()
	{
		R_ASSERT(!terminated_, "App should have terminated by now");

		lua_gc(lua_, LUA_GCCOLLECT, 0);
		TextureManager::Get()->purge();
	}

	void Director::terminate(const char *error)
	{
		terminate();
		error_ = error;
	}
}
