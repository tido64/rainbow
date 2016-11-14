// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Audio.h"

#include "Audio/Mixer.h"
#include "Lua/LuaHelper.h"
#include "Lua/LuaSyntax.h"

using rainbow::audio::Channel;
using rainbow::audio::Sound;

namespace
{
    constexpr const char kChannelType[] = "rainbow::audio::Channel";
    constexpr const char kSoundType[] = "rainbow::audio::Sound";

    Channel* tochannel(lua_State* L)
    {
        return static_cast<Channel*>(rainbow::lua::topointer(L, kChannelType));
    }

    Sound* tosound(lua_State* L)
    {
        return static_cast<Sound*>(rainbow::lua::topointer(L, kSoundType));
    }

    int load_sound(lua_State* L)
    {
        // rainbow.audio.load_sound(file)
        rainbow::lua::checkargs<char*>(L);

        Sound* sound = rainbow::audio::load_sound(lua_tostring(L, 1));
        if (sound == nullptr)
            return 0;

        rainbow::lua::pushpointer(L, sound, kSoundType);
        return 1;
    }

    int load_stream(lua_State* L)
    {
        // rainbow.audio.load_stream(file)
        rainbow::lua::checkargs<char*>(L);

        Sound* sound = rainbow::audio::load_stream(lua_tostring(L, 1));
        if (sound == nullptr)
            return 0;

        rainbow::lua::pushpointer(L, sound, kSoundType);
        return 1;
    }

    int release(lua_State* L)
    {
        // rainbow.audio.release(<sound>)
        rainbow::lua::checkargs<Sound>(L);

        auto sound = tosound(L);
        if (sound != nullptr)
            rainbow::audio::release(sound);
        return 0;
    }

    int is_paused(lua_State* L)
    {
        // rainbow.audio.is_paused(<channel>)
        rainbow::lua::checkargs<Channel>(L);

        Channel* channel = tochannel(L);
        rainbow::lua::push(
            L, channel != nullptr && rainbow::audio::is_paused(channel));
        return 1;
    }

    int is_playing(lua_State* L)
    {
        // rainbow.audio.is_playing(<channel>)
        rainbow::lua::checkargs<Channel>(L);

        Channel* channel = tochannel(L);
        rainbow::lua::push(
            L, channel != nullptr && rainbow::audio::is_playing(channel));
        return 1;
    }

    int set_loop_count(lua_State* L)
    {
        // rainbow.audio.set_loop_count(<channel>, loop_count)
        rainbow::lua::checkargs<Channel, lua_Number>(L);

        const int loop_count = lua_tointeger(L, 2);
        lua_settop(L, 1);

        Channel* channel = tochannel(L);
        if (channel != nullptr)
            rainbow::audio::set_loop_count(channel, loop_count);
        return 0;
    }

    int set_volume(lua_State* L)
    {
        // rainbow.audio.set_volume(<channel>, volume)
        rainbow::lua::checkargs<Channel, lua_Number>(L);

        const float volume = lua_tonumber(L, 2);
        lua_settop(L, 1);

        Channel* channel = tochannel(L);
        if (channel != nullptr)
            rainbow::audio::set_volume(channel, volume);
        return 0;
    }

    int set_world_position(lua_State* L)
    {
        // rainbow.audio.set_world_position(<channel>, x, y)
        rainbow::lua::checkargs<Channel, lua_Number, lua_Number>(L);

        const Vec2f position(lua_tonumber(L, 2), lua_tonumber(L, 3));
        lua_settop(L, 1);

        Channel* channel = tochannel(L);
        if (channel != nullptr)
            rainbow::audio::set_world_position(channel, position);
        return 0;
    }

    int pause(lua_State* L)
    {
        // rainbow.audio.pause(<channel>)
        rainbow::lua::checkargs<Channel>(L);

        Channel* channel = tochannel(L);
        if (channel != nullptr)
            rainbow::audio::pause(channel);
        return 0;
    }

    int play(lua_State* L)
    {
        // rainbow.audio.play(<channel>|<sound>[, x, y])
        rainbow::lua::checkargs<void*,
                                rainbow::lua::nil_or<lua_Number>,
                                rainbow::lua::nil_or<lua_Number>>(L);

        Channel* channel;

        lua_pushliteral(L, "__type");
        lua_rawget(L, 1);
        const char* type = lua_tostring(L, -1);

        if (strcmp(type, kSoundType) == 0)
        {
            Vec2f position;
            if (lua_gettop(L) > 3)
            {
                position.x = lua_tonumber(L, 2);
                position.y = lua_tonumber(L, 3);
            }
            lua_settop(L, 1);

            auto sound = tosound(L);
            if (sound == nullptr)
                return 0;

            channel = rainbow::audio::play(sound, position);
        }
        else if (strcmp(type, kChannelType) == 0)
        {
            lua_settop(L, 1);
            channel = tochannel(L);
            if (channel == nullptr)
                return 0;

            channel = rainbow::audio::play(channel);
        }
        else
        {
            return 0;
        }

        rainbow::lua::pushpointer(L, channel, kChannelType);
        return 1;
    }

    int stop(lua_State* L)
    {
        // rainbow.audio.stop(<sound>)
        rainbow::lua::checkargs<Sound>(L);

        Channel* channel = tochannel(L);
        if (channel != nullptr)
            rainbow::audio::stop(channel);
        return 0;
    }
}

NS_RAINBOW_LUA_MODULE_BEGIN(audio)
{
    void init(lua_State* L)
    {
        lua_pushliteral(L, "audio");
        lua_createtable(L, 0, 16);

        luaR_rawsetcfunction(L, "load_sound", &load_sound);
        luaR_rawsetcfunction(L, "load_stream", &load_stream);
        luaR_rawsetcfunction(L, "release", &release);

        luaR_rawsetcfunction(L, "is_paused", &is_paused);
        luaR_rawsetcfunction(L, "is_playing", &is_playing);

        luaR_rawsetcfunction(L, "set_loop_count", &set_loop_count);
        luaR_rawsetcfunction(L, "set_volume", &set_volume);
        luaR_rawsetcfunction(L, "set_world_position", &set_world_position);

        luaR_rawsetcfunction(L, "pause", &pause);
        luaR_rawsetcfunction(L, "play", &play);
        luaR_rawsetcfunction(L, "stop", &stop);

        lua_rawset(L, -3);
    }
} NS_RAINBOW_LUA_MODULE_END(audio)
