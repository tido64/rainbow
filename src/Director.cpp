// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <lua.hpp>

#include "Common/Data.h"
#include "Director.h"
#include "Graphics/TextureManager.h"
#include "Lua/lua_Platform.h"

namespace Rainbow
{
	Director::Director()
	    : active_(true), terminated_(false), error_(nullptr), input_(lua_)
	{
		if (!ConFuoco::Mixer::Instance)
			this->terminate("Failed to initialise audio engine");
		else if (!this->renderer_.init())
			this->terminate("Failed to initialise renderer");
		else if (this->lua_.init(&this->scenegraph_) != LUA_OK)
			this->terminate("Failed to initialise Lua");
	}

	Director::~Director()
	{
		// Lua must be allowed to clean up before we tear down the graphics
		// context.
		this->lua_.terminate();
	}

	void Director::draw()
	{
		Renderer::clear();
		Renderer::draw(this->scenegraph_);
	}

	void Director::init(const Data &main, const Vec2i &screen)
	{
		R_ASSERT(main, "Failed to load 'main.lua'");

		this->renderer_.set_resolution(screen);
		Lua::Platform::update(this->lua_, screen);
		if (this->lua_.start(main) != LUA_OK || this->lua_.update(0) != LUA_OK)
		{
			this->terminate("Failed to start 'main.lua'");
			return;
		}
		this->scenegraph_.update(0);
	}

	void Director::update(const unsigned long dt)
	{
		R_ASSERT(!this->terminated_, "App should have terminated by now");

		this->mixer_.update();
		if (this->lua_.update(dt))
		{
			this->terminate();
			return;
		}
		this->scenegraph_.update(dt);
		this->input_.clear();
	}

	void Director::on_focus_gained()
	{
		R_ASSERT(!this->terminated_, "App should have terminated by now");

		this->active_ = true;
		this->mixer_.suspend(false);
	}

	void Director::on_focus_lost()
	{
		this->active_ = false;
		this->mixer_.suspend(true);
	}

	void Director::on_memory_warning()
	{
		R_ASSERT(!this->terminated_, "App should have terminated by now");

		lua_gc(this->lua_, LUA_GCCOLLECT, 0);
		TextureManager::Instance->purge();
	}

	void Director::terminate(const char *error)
	{
		this->terminate();
		this->error_ = error;
	}
}
