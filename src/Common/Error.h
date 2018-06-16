// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_ERROR_H_
#define COMMON_ERROR_H_

#include <system_error>

namespace rainbow
{
    enum class ErrorCode
    {
        Success,
        AudioInitializationFailed,
        GLEWInitializationFailed,
        ShaderManagerInitializationFailed,
        RenderInitializationFailed,
        ScriptCompilationFailed,
        ErrorExecutingScript
    };
}

namespace std
{
    template <>
    struct is_error_code_enum<rainbow::ErrorCode> : true_type
    {
    };
}  // namespace std

namespace rainbow
{
    auto make_error_code(ErrorCode) noexcept -> std::error_code;
}

#endif
