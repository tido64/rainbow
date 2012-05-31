// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Director.h"
#include "Physics.h"
#include "Common/Chrono.h"
#include "ConFuoco/Mixer.h"
#include "Input/Input.h"
#include "Lua/lua_Platform.h"

Director::Director()
{
	Input::Instance().set_state(this->lua.L);
}

void Director::init(const char *const script)
{
	if (this->lua.load(&this->scenegraph, script) || this->lua.call("init") || this->lua.update(0))
		this->shutdown();
	else
		this->scenegraph.update();
}

void Director::set_video(const int w, const int h)
{
	lua_Platform::update(this->lua.L, w, h);
}

void Director::update(const unsigned long t)
{
	Chrono::Instance().update(t);
	ConFuoco::Mixer::Instance().update();
#ifdef USE_PHYSICS
	Physics::Instance().step(Chrono::Instance().diff() * (1.0f / 1000.0f));
#endif
	if (this->lua.update(Chrono::Instance().diff()))
		this->shutdown();
	else
		this->scenegraph.update();
}

void Director::on_memory_warning()
{
	lua_gc(this->lua.L, LUA_GCCOLLECT, 0);
}
