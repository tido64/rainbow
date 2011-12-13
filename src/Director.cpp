/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Director.h"
#include "Physics.h"
#include "Common/Chrono.h"
#include "ConFuoco/Mixer.h"
#include "Input/Input.h"
#include "Lua/lua_Platform.h"

using Rainbow::ConFuoco::Mixer;

Director::Director()
{
	Input::Instance().set_state(this->lua.L);
}

void Director::init(const char *const script)
{
	this->lua.load(&this->scenegraph, script);
	this->lua.call("init");
	this->lua.update();
}

void Director::set_video(const int w, const int h)
{
	Input::Instance().set_height(h);
	lua_Platform::update(this->lua.L, w, h);
}

void Director::update(const unsigned long t)
{
	Chrono::Instance().update(t);
	Mixer::Instance().update();
	Physics::Instance().step(Chrono::Instance().diff() * (1.0f / 1000.0f));
	this->lua.update();
	this->scenegraph.update();
}
