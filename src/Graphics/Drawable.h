// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_DRAWABLE_H_
#define GRAPHICS_DRAWABLE_H_

#include <cstdint>

namespace rainbow
{
    /// <summary>Interface for a drawable object.</summary>
    class IDrawable
    {
    public:
        void draw() const { draw_impl(); }
        void update(uint64_t dt) { update_impl(dt); }

    protected:
        virtual ~IDrawable() = default;

    private:
        virtual void draw_impl() const = 0;
        virtual void update_impl(uint64_t dt) = 0;
    };
}  // namespace rainbow

#endif
