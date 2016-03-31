// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SHADERS_DIFFUSE_H_
#define GRAPHICS_SHADERS_DIFFUSE_H_

#include "Math/Vec3.h"

namespace rainbow { namespace shaders
{
    class Diffuse
    {
    public:
        Diffuse(bool normal = false);

        unsigned int id() const { return program_; }

        void set_cutoff(float cutoff) const;
        void set_radius(float radius) const;
        void set_position(float x, float y, float z = 100.0f) const;

    private:
        int cutoff_;    ///< Maximum distance of the light's influence.
        int radius_;    ///< Light source's radius.
        int position_;  ///< Position of the light source.
        unsigned int program_;
    };
}}  // namespace rainbow::shaders

#endif
