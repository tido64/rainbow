// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Error.h"

#include <string>

#include "Common/TypeCast.h"

namespace
{
    class RainbowErrorCategory final : public std::error_category
    {
    public:
        [[nodiscard]] auto name() const noexcept -> const char* override
        {
            return "Rainbow";
        }

        [[nodiscard]] auto message(int condition) const -> std::string override
        {
            using rainbow::ErrorCode;

            switch (static_cast<ErrorCode>(condition))
            {
                case ErrorCode::Success:
                    return {};
                case ErrorCode::AudioInitializationFailed:
                    return "Failed to initialise audio engine";
                case ErrorCode::GLInitializationFailed:
                    return "Failed to initialise OpenGL";
                case ErrorCode::ShaderManagerInitializationFailed:
                    return "Failed to initialise shaders";
                case ErrorCode::RenderInitializationFailed:
                    return "Failed to initialise renderer";
                case ErrorCode::ScriptCompilationFailed:
                    return "Failed to compile JS";
                case ErrorCode::ScriptNotFound:
                    return "No script was found";
                case ErrorCode::ErrorExecutingScript:
                    return "An error occurred while executing JavaScript";
                default:
                    return "Unknown error";
            }
        }
    };

    RainbowErrorCategory sRainbowErrorCategory;
}  // namespace

auto rainbow::make_error_code(rainbow::ErrorCode c) noexcept -> std::error_code
{
    return {to_underlying_type(c), sRainbowErrorCategory};
}
