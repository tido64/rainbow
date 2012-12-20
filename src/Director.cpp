// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Director.h"
#include "Common/Chrono.h"
#include "ConFuoco/Mixer.h"
#include "Graphics/TextureManager.h"
#include "Lua/lua_Platform.h"
#include "Lua/LuaHelper.h"

namespace Rainbow
{
	void Director::init(const Data &main)
	{
		Lua::load(this->lua, main, "main");
		if (Lua::call(this->lua, "init") != LUA_OK || this->lua.update(0) != LUA_OK)
			this->shutdown();
		else
			this->scenegraph.update();
	}

	void Director::set_video(const int w, const int h)
	{
		Lua::Platform::update(this->lua, w, h);
	}

	void Director::update(const unsigned long t)
	{
		Chrono::Instance().update(t);
		ConFuoco::Mixer::Instance->update();
		if (this->lua.update(Chrono::Instance().diff()))
			this->shutdown();
		else
			this->scenegraph.update();
	}

	void Director::on_memory_warning()
	{
		lua_gc(this->lua, LUA_GCCOLLECT, 0);
		TextureManager::Instance().purge();
	}
}
