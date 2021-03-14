// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/JavaScript/Module.h"

#include <string>

#include <duk_module_node.h>

#include "Common/Algorithm.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"

using rainbow::ends_with;
using rainbow::File;
using rainbow::FileType;
using rainbow::filesystem::Path;

namespace
{
    template <typename... Args>
    auto throw_if(duk_context* ctx,
                  bool condition,
                  duk_errcode_t err_code,
                  Args&&... args)
    {
        if (condition) {
#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
            duk_push_error_object(ctx, err_code, std::forward<Args>(args)...);
            NOT_USED(duk_throw(ctx));
            UNREACHABLE();
#ifdef __GNUC__
#    pragma GCC diagnostic pop
#endif
        }

        return 0;
    }

    bool is_relative(const Path& path)
    {
        auto str = path.c_str();
        return str[0] == '.' &&
               (str[1] == '/' || (str[1] == '.' && str[2] == '/'));
    }

    auto push(duk_context* ctx, Path&& path)
    {
        auto str = path.lexically_normal().native();
        duk_push_lstring(ctx, str.c_str(), str.length());
        return 1;
    }
}  // namespace

auto rainbow::duk::module::load(duk_context* ctx) -> duk_ret_t
{
    // Entry stack: [ resolved_id exports module ]
    duk_size_t id_length;
    auto resolved_id = duk_get_lstring(ctx, 0, &id_length);
    throw_if(ctx, id_length == 0, DUK_ERR_TYPE_ERROR, "invalid module name");

    const auto data = File::read(resolved_id, FileType::Asset);
    throw_if(
        ctx, !data, DUK_RET_ERROR, "error loading module: %s", resolved_id);
    duk_push_lstring(ctx, data.as<const char*>(), data.size());
    return 1;
}

auto rainbow::duk::module::resolve(duk_context* ctx) -> duk_ret_t
{
    // Entry stack: [ requested_id parent_id ]
    duk_size_t requested_id_length;
    auto requested_id = duk_get_lstring(ctx, 0, &requested_id_length);
    throw_if(
        ctx, is_empty(requested_id), DUK_RET_TYPE_ERROR, "invalid module name");

    Path requested(requested_id);
    if (requested.extension() != ".js") {
        requested += ".js";
    }

    if (!is_relative(requested)) {
        return push(ctx, std::move(requested));
    }

    duk_size_t parent_id_length;
    auto parent_id = duk_get_lstring(ctx, 1, &parent_id_length);
    if (parent_id_length == 0) {
        return push(ctx, std::move(requested));
    }

    Path parent(parent_id);
    return push(ctx, parent.parent_path() / requested);
}

void rainbow::duk::module::initialize(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_push_c_function(ctx, module::load, -1);
    duk_put_prop_literal(ctx, -2, "load");
    duk_push_c_function(ctx, module::resolve, -1);
    duk_put_prop_literal(ctx, -2, "resolve");
    duk_module_node_init(ctx);
}
