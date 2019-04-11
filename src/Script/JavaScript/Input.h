// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_JAVASCRIPT_INPUT_H_
#define SCRIPT_JAVASCRIPT_INPUT_H_

#include "Input/Input.h"
#include "Input/Pointer.h"
#include "Script/JavaScript/Helper.h"

#define DUKR_PUT_PROP(obj, prop)                                               \
    do                                                                         \
    {                                                                          \
        duk::push(ctx, obj.prop);                                              \
        duk::put_prop_literal(ctx, -2, #prop);                                 \
    } while (false)

namespace rainbow::duk
{
    void initialize_input(duk_context* ctx, Input& input)
    {
        // Map acceleration data to a typed array
        auto&& acceleration = input.acceleration();
        duk::push_external_buffer_object<double>(
            ctx, &acceleration, sizeof(acceleration));
        duk::put_prop_literal(ctx, -2, "acceleration");

        // Controllers
        duk_push_array(ctx);
        const auto& controller_states = input.controller_states();
        duk_uarridx_t i = 0;
        for (auto&& state : controller_states)
        {
            duk_push_bare_object(ctx);

            duk::push(ctx, state.id());
            duk::put_prop_literal(ctx, -2, "id");

            auto&& buttons = state.buttons();
            duk::push_external_buffer_object<bool>(
                ctx, buttons.data(), sizeof(buttons));
            duk::put_prop_literal(ctx, -2, "buttons");

            auto&& axes = state.axes();
            duk::push_external_buffer_object<int>(
                ctx, axes.data(), sizeof(axes));
            duk::put_prop_literal(ctx, -2, "axes");

            duk_put_prop_index(ctx, -2, i++);
        }
        duk::put_prop_literal(ctx, -2, "controllers");

        // Map keyboard state to a typed array
        auto&& keys = input.keyboard_state();
        duk::push_external_buffer_object<bool>(ctx, keys.data(), sizeof(keys));
        duk::put_prop_literal(ctx, -2, "keysDown");

        // Pointer events
        duk_push_array(ctx);
        duk::put_prop_literal(ctx, -2, "pointersDown");
        duk_push_array(ctx);
        duk::put_prop_literal(ctx, -2, "pointersMoved");
        duk_push_array(ctx);
        duk::put_prop_literal(ctx, -2, "pointersUp");

        // Stash the module for quicker access
        duk_push_global_stash(ctx);
        duk_dup(ctx, -2);
        duk_put_prop_index(ctx, -2, DUKR_IDX_INPUT);
        duk_pop(ctx);
    }

    void clear_pointer_events(duk_context* ctx)
    {
        duk_push_global_stash(ctx);
        duk_get_prop_index(ctx, -1, DUKR_IDX_INPUT);

        duk::get_prop_literal(ctx, -1, "pointersDown");
        duk_set_length(ctx, -1, 0);
        duk_pop(ctx);

        duk::get_prop_literal(ctx, -1, "pointersMoved");
        duk_set_length(ctx, -1, 0);
        duk_pop(ctx);

        duk::get_prop_literal(ctx, -1, "pointersUp");
        duk_set_length(ctx, -1, 0);

        duk_pop_3(ctx);
    }

    void update_controller_id(duk_context* ctx, uint32_t port, ControllerID id)
    {
        duk_push_global_stash(ctx);
        duk_get_prop_index(ctx, -1, DUKR_IDX_INPUT);
        duk::get_prop_literal(ctx, -1, "controllers");
        duk_get_prop_index(ctx, -1, port);
        duk::push(ctx, id);
        duk::put_prop_literal(ctx, -2, "id");

        duk_pop_n(ctx, 4);
    }

    template <size_t N>
    void update_pointer_event(duk_context* ctx,
                              const char (&event_name)[N],
                              const ArrayView<Pointer>& pointers)
    {
        duk_push_global_stash(ctx);
        duk_get_prop_index(ctx, -1, DUKR_IDX_INPUT);
        duk::get_prop_literal(ctx, -1, event_name);

        duk_uarridx_t i = 0;
        for (auto&& p : pointers)
        {
            duk_push_bare_object(ctx);
            DUKR_PUT_PROP(p, hash);
            DUKR_PUT_PROP(p, x);
            DUKR_PUT_PROP(p, y);
            DUKR_PUT_PROP(p, timestamp);
            duk_put_prop_index(ctx, -2, i++);
        }

        duk_pop_3(ctx);
    }
}  // namespace rainbow::duk

#undef DUKR_PUT_PROP

#endif
