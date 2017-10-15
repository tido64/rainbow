// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_JAVASCRIPT_MODULE_H_
#define SCRIPT_JAVASCRIPT_MODULE_H_

#include <duktape.h>

extern "C" {  // TODO: Remove workaround with Duktape v2.3.0
#include <duk_module_node.h>
}

#include "Common/Logging.h"

namespace rainbow::duk::module
{
    auto load(duk_context* ctx) -> duk_ret_t
    {
        // Entry stack: [ resolved_id exports module ]
        duk_push_string(ctx, "<source code>");
        return 1;
    }

    auto resolve(duk_context* ctx) -> duk_ret_t
    {
        // Entry stack: [ requested_id parent_id ]

        auto requested_id = duk_get_string(ctx, 0);
        auto parent_id = duk_get_string(ctx, 1);

        std::string resolved_id = parent_id;
        resolved_id += '.';
        resolved_id += requested_id;
        duk_push_string(ctx, resolved_id.c_str());
        return 1;
    }

    void initialize(duk_context* ctx)
    {
        duk_push_object(ctx);
        duk_push_c_function(ctx, module::load, -1);
        duk_put_prop_string(ctx, -2, "load");
        duk_push_c_function(ctx, module::resolve, -1);
        duk_put_prop_string(ctx, -2, "resolve");
        duk_module_node_init(ctx);
    }
}  // namespace rainbow::duk::module

#endif
