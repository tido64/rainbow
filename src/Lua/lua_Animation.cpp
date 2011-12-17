#include "Graphics/Animation.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_Animation.h"

const char lua_Animation::class_name[] = "animation";
const LuaMachine::Method<lua_Animation> lua_Animation::methods[] = {
	{ "is_stopped",  &lua_Animation::is_stopped },
	{ "set_fps",     &lua_Animation::set_fps },
	{ "set_frames",  &lua_Animation::set_frames },
	{ "set_loop",    &lua_Animation::set_loop },
	{ "play",        &lua_Animation::play },
	{ "stop",        &lua_Animation::stop },
	{ 0, 0 }
};

lua_Animation::lua_Animation(lua_State *L)
{
	Sprite *sprite = LuaMachine::wrapper<lua_Sprite>(L, 1)->raw_ptr();

	// Count number of frames
	unsigned int count = 0;
	lua_pushnil(L);
	while (lua_next(L, 2))
	{
		++count;
		lua_pop(L, 1);
	}

	// Allocate frames array
	unsigned int *const frames = new unsigned int[count + 1];
	unsigned int *frame = frames;

	// Populate frames array
	lua_pushnil(L);
	while (lua_next(L, 2))
	{
		*frame = lua_tointeger(L, -1);
		++frame;
		lua_pop(L, 1);
	}
	frames[count] = 0;

	const unsigned int fps = lua_tointeger(L, 3);
	const bool loop = (lua_gettop(L) == 4) ? lua_toboolean(L, 4) : true;
	this->animation = new Animation(sprite, frames, fps, loop);
}

lua_Animation::~lua_Animation()
{
	delete this->animation;
}

int lua_Animation::is_stopped(lua_State *L)
{
	lua_pushboolean(L, this->animation->is_stopped());
	return 1;
}

int lua_Animation::set_fps(lua_State *L)
{
	assert(lua_gettop(L) == 1
	       || !"Rainbow::Lua::Animation::set_fps: Frames per second required");

	this->animation->set_timeout(1000.0f / lua_tointeger(L, 1));
	return 0;
}

int lua_Animation::set_frames(lua_State *L)
{
	assert(lua_gettop(L) > 1
	       || !"Rainbow::Lua::Animation::set_frames: Animations require more than one frame to be called animation");

	const unsigned int framec = lua_gettop(L);
	unsigned int *const frames = new unsigned int[framec + 1];
	for (unsigned int i = 0; i < framec; ++i)
		frames[i] = lua_tointeger(L, i);
	frames[framec] = 0;
	this->animation->set_frames(frames);
	return 0;
}

int lua_Animation::set_loop(lua_State *L)
{
	assert(lua_gettop(L) == 1
	       || !"Rainbow::Lua::Animation::set_loop: Missing parameter");

	this->animation->set_loop(lua_toboolean(L, 1));
	return 0;
}

int lua_Animation::play(lua_State *)
{
	this->animation->start();
	return 0;
}

int lua_Animation::stop(lua_State *)
{
	this->animation->stop();
	this->animation->reset();
	return 0;
}
