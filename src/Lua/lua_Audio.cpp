// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Audio.h"

#include "Common/Data.h"
#include "ConFuoco/Mixer.h"
#include "Lua/LuaHelper.h"

using ConFuoco::Mixer;

#ifndef USE_FMOD_STUDIO

#include "ConFuoco/Sound.h"
#include "Lua/lua_Recorder.h"

using ConFuoco::Sound;

namespace
{
	const char kChannelType[] = "ConFuoco::Mixer::Channel";
	const char kSoundType[] = "ConFuoco::Sound";

	Mixer::Channel* tochannel(lua_State *L)
	{
		return static_cast<Mixer::Channel*>(
		    Rainbow::Lua::topointer(L, kChannelType));
	}

	int set_gain(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 1) ||
		           (lua_istable(L, 1) && lua_isnumber(L, 2)),
		           "rainbow.audio.set_gain([<channel>,] volume)");

		if (lua_gettop(L) == 2)
		{
			const float gain = lua_tonumber(L, 2);
			lua_pop(L, 1);

			Mixer::Channel *ch = tochannel(L);
			if (!ch)
				return 0;

			Mixer::Instance->set_gain(ch, gain);
		}
		else
			Mixer::Instance->set_gain(lua_tonumber(L, 1));
		return 0;
	}

	int set_pitch(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 1),
		           "rainbow.audio.set_pitch(pitch)");

		Mixer::Instance->set_pitch(lua_tonumber(L, 1));
		return 0;
	}

	int clear(lua_State *)
	{
		Mixer::Instance->clear();
		return 0;
	}

	int create_sound(lua_State *L)
	{
		LUA_ASSERT(lua_isstring(L, 1) &&
		           (lua_isnumber(L, 2) || lua_isnone(L, 2)) &&
		           (lua_isnumber(L, 3) || lua_isnone(L, 3)),
		           "rainbow.audio.create_sound(file, type = STATIC, loops = -1)");

		const char *file = lua_tostring(L, 1);
		const int type =
		    luaR_optinteger(L, 2, static_cast<int>(Sound::Type::Static));
		const int loops = luaR_optinteger(L, 3, -1);
		Rainbow::Lua::pushpointer(
		    L,
		    Mixer::Instance->create_sound(
		        file, static_cast<Sound::Type>(type), loops),
		    kSoundType);
		return 1;
	}

	int delete_sound(lua_State *L)
	{
		LUA_ASSERT(lua_istable(L, 1), "rainbow.audio.delete_sound(<sound>)");

		delete static_cast<Sound*>(Rainbow::Lua::topointer(L, kSoundType));
		return 0;
	}

	int pause(lua_State *L)
	{
		LUA_ASSERT(lua_istable(L, 1), "rainbow.audio.pause(<channel>)");

		Mixer::Channel *ch = tochannel(L);
		if (!ch)
			return 0;

		Mixer::Instance->pause(ch);
		return 0;
	}

	int play(lua_State *L)
	{
		LUA_ASSERT(lua_istable(L, 1), "rainbow.audio.play(<sound>)");

		Sound *snd =
		    static_cast<Sound*>(Rainbow::Lua::topointer(L, kSoundType));
		Rainbow::Lua::pushpointer(L, Mixer::Instance->play(snd), kChannelType);
		return 1;
	}

	int stop(lua_State *L)
	{
		LUA_ASSERT(lua_istable(L, 1), "rainbow.audio.stop(<sound>)");

		Mixer::Channel *ch = tochannel(L);
		if (!ch)
			return 0;

		Mixer::Instance->stop(ch);
		return 0;
	}
}

NS_RAINBOW_LUA_MODULE_BEGIN(Audio)
{
	void init(lua_State *L)
	{
		lua_pushliteral(L, "audio");
		lua_createtable(L, 0, 16);

		luaR_rawsetfield(L,
		                 lua_pushinteger,
		                 static_cast<int>(Sound::Type::Static),
		                 "STATIC");
		luaR_rawsetfield(L,
		                 lua_pushinteger,
		                 static_cast<int>(Sound::Type::Stream),
		                 "STREAM");

		luaR_rawsetcclosurefield(L, &set_gain, "set_gain");
		luaR_rawsetcclosurefield(L, &set_pitch, "set_pitch");
		luaR_rawsetcclosurefield(L, &clear, "clear");
		luaR_rawsetcclosurefield(L, &create_sound, "create_sound");
		luaR_rawsetcclosurefield(L, &delete_sound, "delete_sound");
		luaR_rawsetcclosurefield(L, &pause, "pause");
		luaR_rawsetcclosurefield(L, &play, "play");
		luaR_rawsetcclosurefield(L, &stop, "stop");

		reg<Recorder>(L);

		lua_rawset(L, -3);
	}
} NS_RAINBOW_LUA_MODULE_END(Audio)

#else  // USE_FMOD_STUDIO

#include "Common/Debug.h"
#include "FileSystem/Path.h"
#include "Lua/LuaBind.h"

#define FMOD_NS "FMOD"

namespace
{
	using FMOD::Studio::Bank;
	using Rainbow::Lua::Bind;

	const char kBankType[] = "FMOD::Studio::Bank";

	class FMODStudioEventInstance : public Bind<FMODStudioEventInstance>
	{
		friend Bind;

	public:
		explicit FMODStudioEventInstance(lua_State *);
		~FMODStudioEventInstance();

	private:
		static int getVolume(lua_State *);
		static int setVolume(lua_State *);
		static int getPitch(lua_State *);
		static int setPitch(lua_State *);
		static int getPaused(lua_State *);
		static int setPaused(lua_State *);
		static int start(lua_State *);
		static int stop(lua_State *);
		static int getTimelinePosition(lua_State *);
		static int setTimelinePosition(lua_State *);

		FMOD::Studio::EventInstance *instance;
	};

	int loadBank(lua_State *L)
	{
		LUA_ASSERT(lua_isstring(L, 1), FMOD_NS ".loadBank(\"path/to/bank\")");

		const char *const file = lua_tostring(L, 1);
		const Path path(file, Path::RelativeTo::CurrentPath);
		if (!path.is_file())
		{
			R_ERROR("[Rainbow] No such file: %s\n",
			        static_cast<const char*>(path));
			R_ASSERT(path.is_file(), "Failed to find sound bank");
			return 0;
		}
		Bank *bank = nullptr;
		if ((*Mixer::Instance)->loadBankFile(
		        path, FMOD_STUDIO_LOAD_BANK_NORMAL, &bank) != FMOD_OK)
		{
			return 0;
		}
		Rainbow::Lua::pushpointer(L, bank, kBankType);
		return 1;
	}

	int unloadBank(lua_State *L)
	{
		LUA_ASSERT(lua_istable(L, 1), FMOD_NS ".unloadBank(<bank>)");

		Bank *bank = static_cast<Bank*>(Rainbow::Lua::topointer(L, kBankType));
		if (!bank)
			return 0;
		bank->unload();
		return 0;
	}

	int unloadAll(lua_State *)
	{
		(*Mixer::Instance)->unloadAll();
		return 0;
	}

	int createInstance(lua_State *L)
	{
		LUA_ASSERT(lua_isstring(L, 1), FMOD_NS ".createInstance(event)");

		const char *const event = lua_tostring(L, 1);
		FMOD::Studio::ID id{ 0, 0, 0, { 0 } };
		if ((*Mixer::Instance)->lookupID(event, &id) != FMOD_OK)
			return 0;

		FMOD::Studio::EventDescription *description = nullptr;
		if ((*Mixer::Instance)->getEvent(
		        &id, FMOD_STUDIO_LOAD_BEGIN_NOW, &description) != FMOD_OK)
		{
			return 0;
		}

		FMOD::Studio::EventInstance *instance = nullptr;
		if (description->createInstance(&instance) != FMOD_OK)
			return 0;

		lua_settop(L, 0);
		lua_pushlightuserdata(L, instance);
		return Rainbow::Lua::alloc<FMODStudioEventInstance>(L);
	}
}

NS_RAINBOW_LUA_MODULE_BEGIN(Audio)
{
	void init(lua_State *L)
	{
		lua_createtable(L, 0, 4);

		// Bank control
		luaR_rawsetcclosurefield(L, &loadBank, "loadBank");
		luaR_rawsetcclosurefield(L, &unloadBank, "unloadBank");
		luaR_rawsetcclosurefield(L, &unloadAll, "unloadAll");

		// Playback control
		luaR_rawsetcclosurefield(L, &createInstance, "createInstance");

		lua_setglobal(L, FMOD_NS);

		reg<FMODStudioEventInstance>(L);
	}
} NS_RAINBOW_LUA_MODULE_END(Audio)

template<>
const char FMODStudioEventInstance::Bind::class_name[] =
    "FMOD::Studio::EventInstance";

template<>
const bool FMODStudioEventInstance::Bind::is_constructible = false;

template<>
const luaL_Reg FMODStudioEventInstance::Bind::functions[] = {
	{ "getVolume",            &FMODStudioEventInstance::getVolume },
	{ "setVolume",            &FMODStudioEventInstance::setVolume },
	{ "getPitch",             &FMODStudioEventInstance::getPitch },
	{ "setPitch",             &FMODStudioEventInstance::setPitch },
	{ "getPaused",            &FMODStudioEventInstance::getPaused },
	{ "setPaused",            &FMODStudioEventInstance::setPaused },
	{ "start",                &FMODStudioEventInstance::start },
	{ "stop",                 &FMODStudioEventInstance::stop },
	{ "getTimelinePosition",  &FMODStudioEventInstance::getTimelinePosition },
	{ "setTimelinePosition",  &FMODStudioEventInstance::setTimelinePosition },
	{ nullptr, nullptr },
};

FMODStudioEventInstance::FMODStudioEventInstance(lua_State *L)
    : instance(static_cast<FMOD::Studio::EventInstance*>(lua_touserdata(L, 1)))
{ }

FMODStudioEventInstance::~FMODStudioEventInstance()
{
	this->instance->release();
}

int FMODStudioEventInstance::getVolume(lua_State *L)
{
	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	float volume = 0.0f;
	self->instance->getVolume(&volume);
	lua_pushnumber(L, volume);
	return 1;
}

int FMODStudioEventInstance::setVolume(lua_State *L)
{
	LUA_ASSERT(lua_isnumber(L, 2), "<eventInstance>:setVolume(volume)");

	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	self->instance->setVolume(lua_tonumber(L, 2));
	return 0;
}

int FMODStudioEventInstance::getPitch(lua_State *L)
{
	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	float pitch = 0.0f;
	self->instance->getPitch(&pitch);
	lua_pushnumber(L, pitch);
	return 1;
}

int FMODStudioEventInstance::setPitch(lua_State *L)
{
	LUA_ASSERT(lua_isnumber(L, 2), "<eventInstance>:setPitch(volume)");

	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	self->instance->setPitch(lua_tonumber(L, 2));
	return 0;
}

int FMODStudioEventInstance::getPaused(lua_State *L)
{
	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	bool paused = false;
	self->instance->getPaused(&paused);
	lua_pushboolean(L, paused);
	return 1;
}

int FMODStudioEventInstance::setPaused(lua_State *L)
{
	LUA_ASSERT(lua_isboolean(L, 2), "<eventInstance>:setPaused(paused)");

	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	self->instance->setPaused(lua_toboolean(L, 2));
	return 0;
}

int FMODStudioEventInstance::start(lua_State *L)
{
	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	self->instance->start();
	return 0;
}

int FMODStudioEventInstance::stop(lua_State *L)
{
	LUA_ASSERT(lua_isboolean(L, 2) || lua_isnone(L, 2),
	           "<eventInstance>:stop(fadeout = false)");

	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	self->instance->stop(lua_toboolean(L, 2) ? FMOD_STUDIO_STOP_ALLOWFADEOUT
	                                         : FMOD_STUDIO_STOP_IMMEDIATE);
	return 0;
}

int FMODStudioEventInstance::getTimelinePosition(lua_State *L)
{
	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	int position = 0;
	self->instance->getTimelinePosition(&position);
	lua_pushinteger(L, position);
	return 1;
}

int FMODStudioEventInstance::setTimelinePosition(lua_State *L)
{
	LUA_ASSERT(lua_isnumber(L, 2),
	           "<eventInstance>:setTimelinePosition(position)");

	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	self->instance->setTimelinePosition(lua_tointeger(L, 2));
	return 0;
}

#endif  // USE_FMOD_STUDIO
