// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_JAVASCRIPT_RENDERQUEUE_H_
#define SCRIPT_JAVASCRIPT_RENDERQUEUE_H_

#include "Graphics/RenderQueue.h"
#include "Script/JavaScript/Helper.h"

namespace rainbow::duk
{
    namespace
    {
        template <typename T>
        using ApplyFunction = void (*)(duk_context*,
                                       graphics::RenderQueue&,
                                       T&);

        template <typename T>
        void render_queue_add(duk_context*, graphics::RenderQueue& q, T& t)
        {
            q.emplace_back(t);
        }

        template <typename F>
        auto render_queue_apply(duk_context* ctx, duk_idx_t obj_idx, F&& func)
            -> duk_ret_t
        {
            duk_require_type_mask(ctx,
                                  obj_idx,
                                  DUK_TYPE_MASK_NUMBER | DUK_TYPE_MASK_STRING |
                                      DUK_TYPE_MASK_OBJECT);

            auto queue = duk::push_this<graphics::RenderQueue>(ctx);
            const auto result = ([](duk_context* ctx,
                                    duk_idx_t obj_idx,
                                    graphics::RenderQueue* queue) {
                if (duk_is_number(ctx, obj_idx))
                {
                    auto index = duk_require_int(ctx, obj_idx);
                    return queue->begin() + index;
                }
                else if (duk_is_string(ctx, obj_idx))
                {
                    auto tag = duk_require_string(ctx, obj_idx);
                    return std::find_if(  //
                        queue->begin(),
                        queue->end(),
                        [tag](graphics::RenderUnit& unit) {
                            return unit == tag;
                        });
                }
                else
                {
                    auto target = duk::push_instance(ctx, obj_idx);
                    return std::find_if(
                        queue->begin(),
                        queue->end(),
                        [target](graphics::RenderUnit& unit) {
                            return rainbow::visit(
                                [target](auto&& ptr) { return ptr == target; },
                                unit.object());
                        });
                }
            })(ctx, obj_idx, queue);

            if (result == queue->end())
                return 0;

            func(ctx, *queue, result);
            return 0;
        }

        auto render_queue_apply(duk_context* ctx,
                                duk_idx_t obj_idx,
                                ApplyFunction<Animation> fa,
                                ApplyFunction<Label> fl,
                                ApplyFunction<SpriteBatch> fs) -> duk_ret_t
        {
            constexpr char kIncompatibleTypeForRenderUnit[] =
                "Expected Animation, Label, SpriteBatch, or a drawable";

            duk_require_type_mask(ctx, obj_idx, DUK_TYPE_MASK_OBJECT);

            auto& q = *duk::push_this<graphics::RenderQueue>(ctx);

            if (!duk::get_prop_literal(ctx, obj_idx, DUKR_HIDDEN_SYMBOL_TYPE))
                dukr_type_error(ctx, kIncompatibleTypeForRenderUnit);

            auto type = duk_get_pointer(ctx, -1);
            auto ptr = duk::push_instance(ctx, obj_idx);
            if (type == type_id<Animation>().value())
                fa(ctx, q, *static_cast<Animation*>(ptr));
            else if (type == type_id<Label>().value())
                fl(ctx, q, *static_cast<Label*>(ptr));
            else if (type == type_id<SpriteBatch>().value())
                fs(ctx, q, *static_cast<SpriteBatch*>(ptr));
            else
                dukr_type_error(ctx, kIncompatibleTypeForRenderUnit);

            return 0;
        }

        template <typename T>
        void render_queue_insert(duk_context* ctx,
                                 graphics::RenderQueue& q,
                                 T& t)
        {
            q.emplace(q.cbegin() + duk_require_int(ctx, 0), t);
        }
    }

    void initialize_renderqueue(duk_context* ctx, graphics::RenderQueue& q)
    {
        using graphics::RenderQueue;

        duk::put_instance(ctx, duk_get_top(ctx) - 1, &q);
        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                return render_queue_apply(  //
                    ctx,
                    0,
                    &render_queue_add<Animation>,
                    &render_queue_add<Label>,
                    &render_queue_add<SpriteBatch>);
            },
            1);
        duk::put_prop_literal(ctx, -2, "add");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                return render_queue_apply(
                    ctx,
                    0,
                    [](duk_context*, RenderQueue&, RenderQueue::iterator i) {
                        i->disable();
                    });
            },
            1);
        duk::put_prop_literal(ctx, -2, "disable");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                return render_queue_apply(
                    ctx,
                    0,
                    [](duk_context*, RenderQueue&, RenderQueue::iterator i) {
                        i->enable();
                    });
            },
            1);
        duk::put_prop_literal(ctx, -2, "enable");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                return render_queue_apply(
                    ctx,
                    0,
                    [](duk_context*, RenderQueue& q, RenderQueue::iterator i) {
                        q.erase(i);
                    });
            },
            1);
        duk::put_prop_literal(ctx, -2, "erase");

        duk_push_c_function(
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                if (!duk_check_type(ctx, 0, DUK_TYPE_NUMBER))
                {
                    dukr_type_error(ctx, "Expected 'position' to be a number");
                }

                return render_queue_apply(ctx,
                                          1,
                                          &render_queue_insert<Animation>,
                                          &render_queue_insert<Label>,
                                          &render_queue_insert<SpriteBatch>);
            },
            2);
        duk::put_prop_literal(ctx, -2, "insert");

        duk_push_c_function(  //
            ctx,
            [](duk_context* ctx) -> duk_ret_t {
                if (!duk_check_type(ctx, 1, DUK_TYPE_STRING))
                    dukr_type_error(ctx, "Expected 'tag' to be a string");

                return render_queue_apply(
                    ctx,
                    0,
                    [](duk_context* ctx,
                       RenderQueue&,
                       RenderQueue::iterator i) {
                        i->set_tag(duk_require_string(ctx, 1));
                    });
            },
            2);
        duk::put_prop_literal(ctx, -2, "setTag");
    }
}  // namespace rainbow::duk

#endif
