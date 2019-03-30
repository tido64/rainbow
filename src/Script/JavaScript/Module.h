// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_JAVASCRIPT_MODULE_H_
#define SCRIPT_JAVASCRIPT_MODULE_H_

#include <duktape.h>

namespace rainbow::duk::module
{
    auto load(duk_context* ctx) -> duk_ret_t;
    auto resolve(duk_context* ctx) -> duk_ret_t;
    void initialize(duk_context* ctx);
}  // namespace rainbow::duk::module

#endif
