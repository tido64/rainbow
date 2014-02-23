// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Data.h"
#include "ConFuoco/Mixer.h"
#include "Director.h"
#include "Graphics/TextureManager.h"
#include "Lua/lua_Platform.h"

namespace Rainbow
{
	Director::Director()
	    : active_(true), terminated_(false), lua_(&scenegraph_), input_(lua_)
	{ }

	void Director::init(const Data &main, const int width, const int height)
	{
		R_ASSERT(main, "Failed to load 'main.lua'");

		Lua::Platform::update(this->lua_, width, height);
		if (this->lua_.init(main) != LUA_OK || this->lua_.update(0) != LUA_OK)
		{
			this->terminate();
			return;
		}
		this->scenegraph_.update(0);
	}

	void Director::update(const unsigned long dt)
	{
		R_ASSERT(!this->terminated_, "App should have terminated by now");

		ConFuoco::Mixer::Instance->update();
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
		ConFuoco::Mixer::Instance->suspend(false);
	}

	void Director::on_focus_lost()
	{
		this->active_ = false;
		ConFuoco::Mixer::Instance->suspend(true);
	}

	void Director::on_memory_warning()
	{
		R_ASSERT(!this->terminated_, "App should have terminated by now");

		lua_gc(this->lua_, LUA_GCCOLLECT, 0);
		TextureManager::Instance->purge();
	}
}
