// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SHADERS_H_
#define GRAPHICS_SHADERS_H_

namespace rainbow::shaders
{
    namespace integrated
    {
        extern const char kDiffuseLight2Df[];
        extern const char kDiffuseLightNormalf[];
        extern const char kFixed2Df[];
        extern const char kFixed2Dv[];
        extern const char kNormalMappedv[];
        extern const char kSimple2Dv[];
        extern const char kSimplef[];
    }

    constexpr char kDiffuseLight2Df[]      = "Shaders/DiffuseLight2D.fsh";
    constexpr char kDiffuseLightNormalf[]  = "Shaders/DiffuseLightNormal.fsh";
    constexpr char kFixed2Df[]             = "Shaders/Fixed2D.fsh";
    constexpr char kFixed2Dv[]             = "Shaders/Fixed2D.vsh";
    constexpr char kNormalMappedv[]        = "Shaders/NormalMapped.vsh";
    constexpr char kSimple2Dv[]            = "Shaders/Simple2D.vsh";
    constexpr char kSimplef[]              = "Shaders/Simple.fsh";
}  // namespace rainbow::shaders

#endif
