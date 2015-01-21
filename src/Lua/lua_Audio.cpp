// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Audio.h"

#include "Common/Data.h"
#include "ConFuoco/Mixer.h"
#include "Lua/LuaHelper.h"
#include "Lua/LuaSyntax.h"

using ConFuoco::Mixer;

#ifndef USE_FMOD_STUDIO

#include "ConFuoco/Sound.h"
#include "Lua/lua_Recorder.h"

namespace ConFuoco
{
	class Channel;  // Dummy class for syntax checking.
}

using ConFuoco::Sound;

namespace
{
	const char kChannelType[] = "ConFuoco::Mixer::Channel";
	const char kSoundType[] = "ConFuoco::Sound";

	Mixer::Channel* tochannel(lua_State *L)
	{
		return static_cast<Mixer::Channel*>(
		    rainbow::lua::topointer(L, kChannelType));
	}

	int set_gain(lua_State *L)
	{
		// rainbow.audio.set_gain([<channel>,] volume)

		if (lua_gettop(L) >= 2)
		{
			rainbow::lua::Argument<ConFuoco::Channel>::is_required(L, 1);
			rainbow::lua::Argument<lua_Number>::is_required(L, 2);

			const float gain = lua_tonumber(L, 2);
			lua_pop(L, 1);

			Mixer::Channel *ch = tochannel(L);
			if (!ch)
				return 0;

			Mixer::Instance->set_gain(ch, gain);
		}
		else
		{
			rainbow::lua::Argument<lua_Number>::is_required(L, 1);

			Mixer::Instance->set_gain(lua_tonumber(L, 1));
		}
		return 0;
	}

	int set_pitch(lua_State *L)
	{
		// rainbow.audio.set_pitch(pitch)
		rainbow::lua::Argument<lua_Number>::is_required(L, 1);

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
		// rainbow.audio.create_sound(file, type = STATIC, loops = -1)
		rainbow::lua::Argument<char*>::is_required(L, 1);
		rainbow::lua::Argument<lua_Number>::is_optional(L, 2);
		rainbow::lua::Argument<lua_Number>::is_optional(L, 3);

		const char *file = lua_tostring(L, 1);
		const int type = rainbow::lua::optinteger(
		    L, 2, static_cast<int>(Sound::Type::Static));
		const int loops = rainbow::lua::optinteger(L, 3, -1);
		rainbow::lua::pushpointer(
		    L,
		    Mixer::Instance->create_sound(
		        file, static_cast<Sound::Type>(type), loops),
		    kSoundType);
		return 1;
	}

	int delete_sound(lua_State *L)
	{
		// rainbow.audio.delete_sound(<sound>)
		rainbow::lua::Argument<Sound>::is_required(L, 1);

		delete static_cast<Sound*>(rainbow::lua::topointer(L, kSoundType));
		return 0;
	}

	int pause(lua_State *L)
	{
		// rainbow.audio.pause(<channel>)
		rainbow::lua::Argument<ConFuoco::Channel>::is_required(L, 1);

		Mixer::Channel *ch = tochannel(L);
		if (!ch)
			return 0;

		Mixer::Instance->pause(ch);
		return 0;
	}

	int play(lua_State *L)
	{
		// rainbow.audio.play(<sound>)
		rainbow::lua::Argument<Sound>::is_required(L, 1);

		Sound *snd =
		    static_cast<Sound*>(rainbow::lua::topointer(L, kSoundType));
		rainbow::lua::pushpointer(L, Mixer::Instance->play(snd), kChannelType);
		return 1;
	}

	int stop(lua_State *L)
	{
		// rainbow.audio.stop(<sound>)
		rainbow::lua::Argument<Sound>::is_required(L, 1);

		Mixer::Channel *ch = tochannel(L);
		if (!ch)
			return 0;

		Mixer::Instance->stop(ch);
		return 0;
	}
}

NS_RAINBOW_LUA_MODULE_BEGIN(audio)
{
	void init(lua_State *L)
	{
		lua_pushliteral(L, "audio");
		lua_createtable(L, 0, 16);

		luaR_rawsetinteger(L, "STATIC", static_cast<int>(Sound::Type::Static));
		luaR_rawsetinteger(L, "STREAM", static_cast<int>(Sound::Type::Stream));

		luaR_rawsetcfunction(L, "set_gain", &set_gain);
		luaR_rawsetcfunction(L, "set_pitch", &set_pitch);
		luaR_rawsetcfunction(L, "clear", &clear);
		luaR_rawsetcfunction(L, "create_sound", &create_sound);
		luaR_rawsetcfunction(L, "delete_sound", &delete_sound);
		luaR_rawsetcfunction(L, "pause", &pause);
		luaR_rawsetcfunction(L, "play", &play);
		luaR_rawsetcfunction(L, "stop", &stop);

		reg<Recorder>(L);

		lua_rawset(L, -3);
	}
} NS_RAINBOW_LUA_MODULE_END(audio)

#else  // USE_FMOD_STUDIO

#include "Common/Logging.h"
#include "FileSystem/Path.h"
#include "Lua/LuaBind.h"

namespace  // FMOD Low Level API
{
	using rainbow::lua::Bind;

	FMOD_RESULT getLowLevelSystem(FMOD::System **system)
	{
		return (*Mixer::Instance)->getLowLevelSystem(system);
	}

	class FMODChannel : public Bind<FMODChannel>
	{
		friend Bind;

	public:
		explicit FMODChannel(lua_State *);

		FMOD::Channel* get() const;

	private:
		static int stop(lua_State *);
		static int setPaused(lua_State *);
		static int getPaused(lua_State *);
		static int setVolume(lua_State *);
		static int getVolume(lua_State *);
		static int setVolumeRamp(lua_State *);
		static int getVolumeRamp(lua_State *);
		static int getAudibility(lua_State *);
		static int setPitch(lua_State *);
		static int getPitch(lua_State *);
		static int setMute(lua_State *);
		static int getMute(lua_State *);
		static int setReverbProperties(lua_State *);
		static int getReverbProperties(lua_State *);
		static int setLowPassGain(lua_State *);
		static int getLowPassGain(lua_State *);
		static int setCallback(lua_State *);
		static int isPlaying(lua_State *);
		static int setPan(lua_State *);

		static int setLoopCount(lua_State *);
		static int getLoopCount(lua_State *);
		static int setLoopPoints(lua_State *);
		static int getLoopPoints(lua_State *);

		FMOD::Channel *channel_;
	};

	class FMODSound : public Bind<FMODSound>
	{
		friend Bind;

	public:
		explicit FMODSound(lua_State *);
		~FMODSound();

		FMOD::Sound* get() const;

	private:
		static int setLoopCount(lua_State *);
		static int getLoopCount(lua_State *);
		static int setLoopPoints(lua_State *);
		static int getLoopPoints(lua_State *);

		FMOD::Sound *sound_;
	};

	int createSound(lua_State *L, const bool stream)
	{
		rainbow::lua::Argument<char*>::is_required(L, 1);

		FMOD::System *system;
		if (getLowLevelSystem(&system) != FMOD_OK)
			return 0;

#ifdef RAINBOW_OS_ANDROID
		const char *path =
		    lua_pushfstring(L, "file:///android_asset/%s", lua_tostring(L, 1));
#else
		const Path path(lua_tostring(L, 1));
		if (!path.is_file())
		{
			R_ABORT("No such file: %s", static_cast<const char*>(path));
			return 0;
		}
#endif

		FMOD::Sound *sound;
		const FMOD_RESULT result =
		    stream ? system->createStream(path, FMOD_DEFAULT, nullptr, &sound)
		           : system->createSound(path, FMOD_DEFAULT, nullptr, &sound);
		if (result != FMOD_OK)
			return 0;

		lua_settop(L, 0);
		lua_pushlightuserdata(L, sound);
		return rainbow::lua::alloc<FMODSound>(L);
	}

	int createSound(lua_State *L)
	{
		return createSound(L, false);
	}

	int createStream(lua_State *L)
	{
		return createSound(L, true);
	}

	int playSound(lua_State *L)
	{
		rainbow::lua::Argument<FMOD::Sound>::is_required(L, 1);

		FMOD::System *system;
		if (getLowLevelSystem(&system) != FMOD_OK)
			return 0;

		FMOD::Channel *channel;
		const FMOD_RESULT result = system->playSound(
		    rainbow::lua::touserdata<FMODSound>(L, 1)->get(), nullptr, false,
		    &channel);
		if (result != FMOD_OK)
			return 0;

		lua_settop(L, 0);
		lua_pushlightuserdata(L, channel);
		return rainbow::lua::alloc<FMODChannel>(L);
	}

	template<typename T>
	int setLoopCount(lua_State *L, T *self)
	{
		rainbow::lua::Argument<lua_Number>::is_required(L, 2);

		if (!self)
			return 0;

		const int count = lua_tointeger(L, 2);
		self->get()->setMode((count == 0) ? FMOD_LOOP_OFF : FMOD_LOOP_NORMAL);
		self->get()->setLoopCount(count);
		return 0;
	}

	template<typename T>
	int getLoopCount(lua_State *L, T *self)
	{
		if (!self)
			return 0;

		int count;
		self->get()->getLoopCount(&count);
		lua_pushinteger(L, count);
		return 1;
	}

	template<typename T>
	int setLoopPoints(lua_State *L, T *self)
	{
		rainbow::lua::Argument<lua_Number>::is_required(L, 2);
		rainbow::lua::Argument<lua_Number>::is_required(L, 3);

		if (!self)
			return 0;

		self->get()->setLoopPoints(
		    static_cast<unsigned int>(lua_tointeger(L, 2)), FMOD_TIMEUNIT_MS,
		    static_cast<unsigned int>(lua_tointeger(L, 3)), FMOD_TIMEUNIT_MS);
		return 0;
	}

	template<typename T>
	int getLoopPoints(lua_State *L, T *self)
	{
		if (!self)
			return 0;

		unsigned int start, end;
		self->get()->getLoopPoints(&start, FMOD_TIMEUNIT_MS,
		                             &end, FMOD_TIMEUNIT_MS);
		lua_pushinteger(L, start);
		lua_pushinteger(L, end);
		return 2;
	}
}

namespace  // FMOD Studio API
{
	using FMOD::Studio::Bank;
	using rainbow::lua::Bind;

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

		FMOD::Studio::EventInstance *instance_;
	};

	int loadBank(lua_State *L)
	{
		// FMOD.loadBank("path/to/bank")
		rainbow::lua::Argument<char*>::is_required(L, 1);

		const Path path(lua_tostring(L, 1));
		if (!path.is_file())
		{
			R_ABORT("No such file: %s", static_cast<const char*>(path));
			return 0;
		}
		Bank *bank = nullptr;
		if ((*Mixer::Instance)->loadBankFile(
		        path, FMOD_STUDIO_LOAD_BANK_NORMAL, &bank) != FMOD_OK)
		{
			return 0;
		}
		rainbow::lua::pushpointer(L, bank, kBankType);
		return 1;
	}

	int unloadBank(lua_State *L)
	{
		// FMOD.unloadBank(<bank>)
		rainbow::lua::Argument<Bank>::is_required(L, 1);

		Bank *bank = static_cast<Bank*>(rainbow::lua::topointer(L, kBankType));
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
		// FMOD.createInstance(event)
		rainbow::lua::Argument<char*>::is_required(L, 1);

		FMOD::Studio::EventDescription *description = nullptr;
		if ((*Mixer::Instance)->getEvent(lua_tostring(L, 1), &description) !=
		    FMOD_OK)
		{
			return 0;
		}

		FMOD::Studio::EventInstance *instance = nullptr;
		if (description->createInstance(&instance) != FMOD_OK)
			return 0;

		lua_settop(L, 0);
		lua_pushlightuserdata(L, instance);
		return rainbow::lua::alloc<FMODStudioEventInstance>(L);
	}
}

NS_RAINBOW_LUA_MODULE_BEGIN(audio)
{
	void init(lua_State *L)
	{
		lua_createtable(L, 0, 7);

		// Low level API
		luaR_rawsetcfunction(L, "createSound", &createSound);
		luaR_rawsetcfunction(L, "createStream", &createStream);
		luaR_rawsetcfunction(L, "playSound", &playSound);

		// Bank control
		luaR_rawsetcfunction(L, "loadBank", &loadBank);
		luaR_rawsetcfunction(L, "unloadBank", &unloadBank);
		luaR_rawsetcfunction(L, "unloadAll", &unloadAll);

		// Playback control
		luaR_rawsetcfunction(L, "createInstance", &createInstance);

		lua_setglobal(L, "FMOD");

		reg<FMODChannel>(L);
		reg<FMODSound>(L);
		reg<FMODStudioEventInstance>(L);
	}
} NS_RAINBOW_LUA_MODULE_END(audio)

template<>
const char FMODChannel::Bind::class_name[] = "FMOD::Channel";

template<>
const bool FMODChannel::Bind::is_constructible = false;

template<>
const luaL_Reg FMODChannel::Bind::functions[] = {
	{ "stop",                 &FMODChannel::stop },
	{ "setPaused",            &FMODChannel::setPaused },
	{ "getPaused",            &FMODChannel::getPaused },
	{ "setVolume",            &FMODChannel::setVolume },
	{ "getVolume",            &FMODChannel::getVolume },
	{ "setVolumeRamp",        &FMODChannel::setVolumeRamp },
	{ "getVolumeRamp",        &FMODChannel::getVolumeRamp },
	{ "getAudibility",        &FMODChannel::getAudibility },
	{ "setPitch",             &FMODChannel::setPitch },
	{ "getPitch",             &FMODChannel::getPitch },
	{ "setMute",              &FMODChannel::setMute },
	{ "getMute",              &FMODChannel::getMute },
	{ "setReverbProperties",  &FMODChannel::setReverbProperties },
	{ "getReverbProperties",  &FMODChannel::getReverbProperties },
	{ "setLowPassGain",       &FMODChannel::setLowPassGain },
	{ "getLowPassGain",       &FMODChannel::getLowPassGain },
	{ "isPlaying",            &FMODChannel::isPlaying },
	{ "setPan",               &FMODChannel::setPan },
	{ "setLoopCount",         &FMODChannel::setLoopCount },
	{ "getLoopCount",         &FMODChannel::getLoopCount },
	{ "setLoopPoints",        &FMODChannel::setLoopPoints },
	{ "getLoopPoints",        &FMODChannel::getLoopPoints },
	{ nullptr, nullptr }
};

FMODChannel::FMODChannel(lua_State *L)
    : channel_(static_cast<FMOD::Channel*>(lua_touserdata(L, 1))) {}

FMOD::Channel* FMODChannel::get() const
{
	return channel_;
}

int FMODChannel::stop(lua_State *L)
{
	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	self->get()->stop();
	return 0;
}

int FMODChannel::setPaused(lua_State *L)
{
	rainbow::lua::Argument<bool>::is_required(L, 2);

	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	self->get()->setPaused(lua_toboolean(L, 2));
	return 0;
}

int FMODChannel::getPaused(lua_State *L)
{
	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	bool paused;
	self->get()->getPaused(&paused);
	lua_pushboolean(L, paused);
	return 1;
}

int FMODChannel::setVolume(lua_State *L)
{
	rainbow::lua::Argument<lua_Number>::is_required(L, 2);

	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	self->get()->setVolume(lua_tonumber(L, 2));
	return 0;
}

int FMODChannel::getVolume(lua_State *L)
{
	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	float volume;
	self->get()->getVolume(&volume);
	lua_pushnumber(L, volume);
	return 1;
}

int FMODChannel::setVolumeRamp(lua_State *L)
{
	rainbow::lua::Argument<bool>::is_required(L, 2);

	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	self->get()->setVolumeRamp(lua_toboolean(L, 2));
	return 0;
}

int FMODChannel::getVolumeRamp(lua_State *L)
{
	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	bool ramp;
	self->get()->getVolumeRamp(&ramp);
	lua_pushboolean(L, ramp);
	return 1;
}

int FMODChannel::getAudibility(lua_State *L)
{
	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	float audibility;
	self->get()->getAudibility(&audibility);
	lua_pushnumber(L, audibility);
	return 1;
}

int FMODChannel::setPitch(lua_State *L)
{
	rainbow::lua::Argument<lua_Number>::is_required(L, 2);

	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	self->get()->setPitch(lua_tonumber(L, 2));
	return 0;
}

int FMODChannel::getPitch(lua_State *L)
{
	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	float pitch;
	self->get()->getPitch(&pitch);
	lua_pushnumber(L, pitch);
	return 1;
}

int FMODChannel::setMute(lua_State *L)
{
	rainbow::lua::Argument<bool>::is_required(L, 2);

	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	self->get()->setMute(lua_toboolean(L, 2));
	return 0;
}

int FMODChannel::getMute(lua_State *L)
{
	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	bool mute;
	self->get()->getMute(&mute);
	lua_pushboolean(L, mute);
	return 1;
}

int FMODChannel::setReverbProperties(lua_State *L)
{
	rainbow::lua::Argument<lua_Number>::is_required(L, 2);
	rainbow::lua::Argument<lua_Number>::is_required(L, 3);

	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	self->get()->setReverbProperties(lua_tointeger(L, 2), lua_tonumber(L, 3));
	return 0;
}

int FMODChannel::getReverbProperties(lua_State *L)
{
	rainbow::lua::Argument<lua_Number>::is_required(L, 2);

	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	float wet;
	self->get()->getReverbProperties(lua_tointeger(L, 2), &wet);
	lua_pushnumber(L, wet);
	return 1;
}

int FMODChannel::setLowPassGain(lua_State *L)
{
	rainbow::lua::Argument<lua_Number>::is_required(L, 2);

	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	self->get()->setLowPassGain(lua_tonumber(L, 2));
	return 0;
}

int FMODChannel::getLowPassGain(lua_State *L)
{
	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	float gain;
	self->get()->getLowPassGain(&gain);
	lua_pushnumber(L, gain);
	return 1;
}

int FMODChannel::isPlaying(lua_State *L)
{
	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	bool playing;
	self->get()->isPlaying(&playing);
	lua_pushboolean(L, playing);
	return 1;
}

int FMODChannel::setPan(lua_State *L)
{
	rainbow::lua::Argument<lua_Number>::is_required(L, 2);

	FMODChannel *self = Bind::self(L);
	if (!self)
		return 0;

	self->get()->setPan(lua_tonumber(L, 2));
	return 0;
}

int FMODChannel::setLoopCount(lua_State *L)
{
	return ::setLoopCount(L, Bind::self(L));
}

int FMODChannel::getLoopCount(lua_State *L)
{
	return ::getLoopCount(L, Bind::self(L));
}

int FMODChannel::setLoopPoints(lua_State *L)
{
	return ::setLoopPoints(L, Bind::self(L));
}

int FMODChannel::getLoopPoints(lua_State *L)
{
	return ::getLoopPoints(L, Bind::self(L));
}

template<>
const char FMODSound::Bind::class_name[] = "FMOD::Sound";

template<>
const bool FMODSound::Bind::is_constructible = false;

template<>
const luaL_Reg FMODSound::Bind::functions[] = {
	{ "setLoopCount",   &FMODSound::setLoopCount },
	{ "getLoopCount",   &FMODSound::getLoopCount },
	{ "setLoopPoints",  &FMODSound::setLoopPoints },
	{ "getLoopPoints",  &FMODSound::getLoopPoints },
	{ nullptr, nullptr }
};

FMODSound::FMODSound(lua_State *L)
    : sound_(static_cast<FMOD::Sound*>(lua_touserdata(L, 1))) {}

FMODSound::~FMODSound()
{
	get()->release();
}

FMOD::Sound* FMODSound::get() const
{
	return sound_;
}

int FMODSound::setLoopCount(lua_State *L)
{
	return ::setLoopCount(L, Bind::self(L));
}

int FMODSound::getLoopCount(lua_State *L)
{
	return ::getLoopCount(L, Bind::self(L));
}

int FMODSound::setLoopPoints(lua_State *L)
{
	return ::setLoopPoints(L, Bind::self(L));
}

int FMODSound::getLoopPoints(lua_State *L)
{
	return ::getLoopPoints(L, Bind::self(L));
}

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
	{ nullptr, nullptr }
};

FMODStudioEventInstance::FMODStudioEventInstance(lua_State *L)
    : instance_(static_cast<FMOD::Studio::EventInstance*>(lua_touserdata(L, 1)))
{}

FMODStudioEventInstance::~FMODStudioEventInstance()
{
	instance_->release();
}

int FMODStudioEventInstance::getVolume(lua_State *L)
{
	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	float volume = 0.0f;
	self->instance_->getVolume(&volume);
	lua_pushnumber(L, volume);
	return 1;
}

int FMODStudioEventInstance::setVolume(lua_State *L)
{
	// <eventInstance>:setVolume(volume)
	rainbow::lua::Argument<lua_Number>::is_required(L, 2);

	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	self->instance_->setVolume(lua_tonumber(L, 2));
	return 0;
}

int FMODStudioEventInstance::getPitch(lua_State *L)
{
	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	float pitch = 0.0f;
	self->instance_->getPitch(&pitch);
	lua_pushnumber(L, pitch);
	return 1;
}

int FMODStudioEventInstance::setPitch(lua_State *L)
{
	// <eventInstance>:setPitch(volume)
	rainbow::lua::Argument<lua_Number>::is_required(L, 2);

	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	self->instance_->setPitch(lua_tonumber(L, 2));
	return 0;
}

int FMODStudioEventInstance::getPaused(lua_State *L)
{
	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	bool paused = false;
	self->instance_->getPaused(&paused);
	lua_pushboolean(L, paused);
	return 1;
}

int FMODStudioEventInstance::setPaused(lua_State *L)
{
	// <eventInstance>:setPaused(paused)
	rainbow::lua::Argument<bool>::is_required(L, 2);

	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	self->instance_->setPaused(lua_toboolean(L, 2));
	return 0;
}

int FMODStudioEventInstance::start(lua_State *L)
{
	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	self->instance_->start();
	return 0;
}

int FMODStudioEventInstance::stop(lua_State *L)
{
	// <eventInstance>:stop(fadeout = false)
	rainbow::lua::Argument<bool>::is_optional(L, 2);

	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	self->instance_->stop(lua_toboolean(L, 2) ? FMOD_STUDIO_STOP_ALLOWFADEOUT
	                                          : FMOD_STUDIO_STOP_IMMEDIATE);
	return 0;
}

int FMODStudioEventInstance::getTimelinePosition(lua_State *L)
{
	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	int position = 0;
	self->instance_->getTimelinePosition(&position);
	lua_pushinteger(L, position);
	return 1;
}

int FMODStudioEventInstance::setTimelinePosition(lua_State *L)
{
	// <eventInstance>:setTimelinePosition(position)
	rainbow::lua::Argument<lua_Number>::is_required(L, 2);

	FMODStudioEventInstance *self = Bind::self(L);
	if (!self)
		return 0;

	self->instance_->setTimelinePosition(lua_tointeger(L, 2));
	return 0;
}

#endif  // USE_FMOD_STUDIO
