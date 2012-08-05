// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Director.h"
#include "Physics.h"
#include "Common/Chrono.h"
#include "ConFuoco/Mixer.h"
#include "Graphics/TextureManager.h"
#include "Input/Input.h"
#include "Lua/lua_Platform.h"

Director::Director() : lua(&this->scenegraph)
{
	Input::Instance().set_state(this->lua);
}

void Director::init(const Data &main)
{
	LuaMachine::load(this->lua, main, "main");
	if (this->lua.call("init") != LUA_OK || this->lua.update(0) != LUA_OK)
		this->shutdown();
	else
		this->scenegraph.update();
}

void Director::set_video(const int w, const int h)
{
	Rainbow::Lua::Platform::update(this->lua, w, h);
}

void Director::update(const unsigned long t)
{
	Chrono::Instance().update(t);
	ConFuoco::Mixer::Instance().update();
#ifdef USE_PHYSICS
	Physics::Instance().step(Chrono::Instance().diff() * 0.001f);
#endif
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
