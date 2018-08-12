// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_JAVASCRIPT_CONSOLE_H_
#define SCRIPT_JAVASCRIPT_CONSOLE_H_

#include <duktape.h>

#include "Common/Logging.h"

namespace rainbow::duk::console
{
    auto join_strings(duk_context* ctx)
    {
        const auto nargs = duk_get_top(ctx);
        duk_push_literal(ctx, " ");
        duk_insert(ctx, 0);
        duk_join(ctx, nargs);
        return duk_require_string(ctx, -1);
    }

    auto debug([[maybe_unused]] duk_context* ctx)
    {
        LOGD("JavaScript: %s", join_strings(ctx));
        return 0;
    }

    auto error(duk_context* ctx)
    {
        LOGE("JavaScript: %s", join_strings(ctx));
        return 0;
    }

    auto info([[maybe_unused]] duk_context* ctx)
    {
        LOGI("JavaScript: %s", join_strings(ctx));
        return 0;
    }

    auto warn([[maybe_unused]] duk_context* ctx)
    {
        LOGW("JavaScript: %s", join_strings(ctx));
        return 0;
    }

    auto initialize(duk_context* ctx)
    {
        const duk_function_list_entry funcs[]{
            {"debug", &console::debug, -1},
            {"error", &console::error, -1},
            {"info", &console::info, -1},
            {"log", &console::debug, -1},
            {"warn", &console::warn, -1},
            {nullptr, nullptr, 0},
        };

        duk_push_bare_object(ctx);
        duk_put_function_list(ctx, -1, funcs);
        duk_freeze(ctx, -1);
        duk_put_global_literal(ctx, "console");
    }
}  // namespace rainbow::duk::console

#endif
