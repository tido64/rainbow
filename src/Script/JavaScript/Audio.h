// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_JAVASCRIPT_AUDIO_H_
#define SCRIPT_JAVASCRIPT_AUDIO_H_

#include "Audio/Mixer.h"
#include "Script/JavaScript/Helper.h"

namespace rainbow::duk
{
    void initialize_audio(duk_context* ctx)
    {
        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                auto args = duk::get_args<czstring>(ctx);
                auto sound = audio::load_sound(std::get<0>(args));
                if (sound == nullptr)
                    return 0;

                duk::push(ctx, sound);
                return 1;
            },
            1);
        duk::put_prop_literal(ctx, -2, "loadSound");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                auto args = duk::get_args<czstring>(ctx);
                auto sound = audio::load_stream(std::get<0>(args));
                if (sound == nullptr)
                    return 0;

                duk::push(ctx, sound);
                return 1;
            },
            1);
        duk::put_prop_literal(ctx, -2, "loadStream");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                auto args = duk::get_args<audio::Sound*>(ctx);
                audio::release(std::get<0>(args));
                return 0;
            },
            1);
        duk::put_prop_literal(ctx, -2, "release");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                auto args = duk::get_args<audio::Channel*>(ctx);
                const auto is_paused = audio::is_paused(std::get<0>(args));
                duk::push(ctx, is_paused);
                return 1;
            },
            1);
        duk::put_prop_literal(ctx, -2, "isPaused");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                auto args = duk::get_args<audio::Channel*>(ctx);
                const auto is_playing = audio::is_playing(std::get<0>(args));
                duk::push(ctx, is_playing);
                return 1;
            },
            1);
        duk::put_prop_literal(ctx, -2, "isPlaying");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                auto args = duk::get_args<audio::Channel*, int>(ctx);
                audio::set_loop_count(std::get<0>(args), std::get<1>(args));
                return 0;
            },
            2);
        duk::put_prop_literal(ctx, -2, "setLoopCount");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                auto args = duk::get_args<audio::Channel*, float>(ctx);
                audio::set_volume(std::get<0>(args), std::get<1>(args));
                return 0;
            },
            2);
        duk::put_prop_literal(ctx, -2, "setVolume");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                auto args = duk::get_args<audio::Channel*, Vec2f>(ctx);
                audio::set_world_position(std::get<0>(args), std::get<1>(args));
                return 0;
            },
            2);
        duk::put_prop_literal(ctx, -2, "setWorldPosition");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                auto args = duk::get_args<audio::Channel*>(ctx);
                audio::pause(std::get<0>(args));
                return 0;
            },
            1);
        duk::put_prop_literal(ctx, -2, "pause");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                duk_require_object(ctx, 0);

                duk::get_prop_literal(ctx, 0, DUKR_HIDDEN_SYMBOL_TYPE);
                const auto type = duk_require_pointer(ctx, -1);

                if (type == type_id<audio::Channel>().value())
                {
                    auto channel =
                        duk::get_pointer<audio::Channel, false>(ctx, 0);
                    auto ch = audio::play(channel);
                    duk::push(ctx, ch);
                    return 1;
                }

                if (type == type_id<audio::Sound>().value())
                {
                    auto sound = duk::get_pointer<audio::Sound, false>(ctx, 0);
                    auto channel = audio::play(sound);
                    duk::push(ctx, channel);
                    return 1;
                }

                dukr_type_error(  //
                    ctx,
                    "Expected 'Rainbow.Audio.Channel' or "
                    "'Rainbow.Audio.Sound'");
#ifndef _MSC_VER
                // GCC throws 'error: control reaches end of non-void function'
                // if the return statement is missing. But this causes 'Warning
                // C4702: unreachable code' with MSC.
                return DUK_RET_TYPE_ERROR;
#endif
            },
            1);
        duk::put_prop_literal(ctx, -2, "play");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                auto args = duk::get_args<audio::Channel*>(ctx);
                audio::stop(std::get<0>(args));
                return 0;
            },
            1);
        duk::put_prop_literal(ctx, -2, "stop");
    }
}  // namespace rainbow::duk

#endif
