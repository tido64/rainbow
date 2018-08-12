// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_JAVASCRIPT_MODULE_H_
#define SCRIPT_JAVASCRIPT_MODULE_H_

#include <string>

#include <duktape.h>
#include <duk_module_node.h>

#include "FileSystem/File.h"

using rainbow::ends_with;
using rainbow::File;
using rainbow::FileType;

namespace
{
    template <typename... Args>
    auto throw_if(duk_context* ctx,
                  bool condition,
                  duk_errcode_t err_code,
                  Args&&... args)
    {
        if (condition)
        {
#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
            duk_push_error_object(ctx, err_code, std::forward<Args>(args)...);
            NOT_USED(duk_throw(ctx));
            UNREACHABLE();
#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif
        }

        return 0;
    }

    auto module_path(duk_context* ctx)
    {
        constexpr char kModuleExtension[] = ".js";

        // Entry stack: [ resolved_id exports module ]
        duk_size_t id_length;
        auto resolved_id = duk_get_lstring(ctx, 0, &id_length);
        throw_if(
            ctx, id_length == 0, DUK_ERR_TYPE_ERROR, "invalid module name");

        if (!ends_with({resolved_id, id_length}, kModuleExtension))
        {
            thread_local std::string module;
            module = resolved_id;
            module += kModuleExtension;
            return module.c_str();
        }

        return resolved_id;
    }
}  // namespace

namespace rainbow::duk::module
{
    auto load(duk_context* ctx) -> duk_ret_t
    {
        auto path = module_path(ctx);
        const auto data = File::read(path, FileType::Asset);

        throw_if(ctx, !data, DUK_RET_ERROR, "error loading module: %s", path);

        duk_push_lstring(ctx, data.as<const char*>(), data.size());
        return 1;
    }

    auto resolve(duk_context* ctx) -> duk_ret_t
    {
        // Entry stack: [ requested_id parent_id ]
        auto requested_id = duk_get_string(ctx, 0);
        throw_if(ctx,
                 is_empty(requested_id),
                 DUK_RET_TYPE_ERROR,
                 "invalid module name");

        thread_local std::string resolved_id;

        auto parent_id = duk_get_string(ctx, 1);
        if (is_empty(parent_id))
            resolved_id.clear();
        else
            resolved_id = parent_id;

        const auto i = resolved_id.find_last_of('/');
        if (i != std::string::npos)
            resolved_id.erase(i + 1);
        resolved_id += requested_id;
        duk_push_string(ctx, resolved_id.c_str());
        return 1;
    }

    void initialize(duk_context* ctx)
    {
        duk_push_object(ctx);
        duk_push_c_function(ctx, module::load, -1);
        duk_put_prop_literal(ctx, -2, "load");
        duk_push_c_function(ctx, module::resolve, -1);
        duk_put_prop_literal(ctx, -2, "resolve");
        duk_module_node_init(ctx);
    }
}  // namespace rainbow::duk::module

#endif
