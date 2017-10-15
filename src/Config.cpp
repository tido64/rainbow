// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Config.h"

#include <memory>

#include "Common/Data.h"
#include "Common/Logging.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"
#include "Script/JavaScript/Helper.h"
#include "Script/JavaScript/JavaScript.h"

namespace
{
#ifdef RAINBOW_SDL
    constexpr unsigned int kMaxMSAA = 16;
#endif
}

rainbow::Config::Config()
    : accelerometer_(true), high_dpi_(false), suspend_(true), width_(0),
      height_(0), msaa_(0)
{
    constexpr char kConfigModule[] = "config.json";

    const auto path = filesystem::relative(kConfigModule);
    std::error_code error;
    if (!filesystem::is_regular_file(path, error))
    {
        LOGI("No config file was found");
        return;
    }

    const Data& config = File::read(kConfigModule, FileType::Asset);
    if (!config)
        return;

    duk::Context ctx;
    duk_push_lstring(ctx, config.as<const char*>(), config.size());
    if (duk_safe_call(  //
            ctx,
            [](duk_context* ctx, void*) -> duk_ret_t {
                duk_json_decode(ctx, -1);
                return 1;
            },
            nullptr,
            1,
            1) != DUK_EXEC_SUCCESS)
    {
        return;
    }

    if (duk::get_prop_literal(ctx, -1, "accelerometer") &&
        duk_is_boolean(ctx, -1))
    {
        accelerometer_ = duk::get<bool>(ctx, -1);
    }
    duk_pop(ctx);

#ifdef RAINBOW_SDL
    if (duk::get_prop_literal(ctx, -1, "allowHighDPI") &&
        duk_is_boolean(ctx, -1))
    {
        high_dpi_ = duk::get<bool>(ctx, -1);
    }
    duk_pop(ctx);

    if (duk::get_prop_literal(ctx, -1, "msaa") && duk_is_number(ctx, -1))
    {
        auto value = floor_pow2(duk_get_uint_default(ctx, -1, 0));
        msaa_ = std::min(value, kMaxMSAA);
    }
    duk_pop(ctx);
#endif

    if (duk::get_prop_literal(ctx, -1, "resolution") && duk_is_object(ctx, -1))
    {
        duk::get_prop_literal(ctx, -1, "width");
        width_ = duk_get_int_default(ctx, -1, 0);

        duk::get_prop_literal(ctx, -2, "height");
        height_ = duk_get_int_default(ctx, -1, 0);

        duk_pop_2(ctx);
    }
    duk_pop(ctx);

#ifdef RAINBOW_SDL
    if (duk::get_prop_literal(ctx, -1, "suspendOnFocusLost") &&
        duk_is_boolean(ctx, -1))
    {
        suspend_ = duk::get<bool>(ctx, -1);
    }
    duk_pop(ctx);
#endif
}
