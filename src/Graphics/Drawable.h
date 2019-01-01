// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_DRAWABLE_H_
#define GRAPHICS_DRAWABLE_H_

#include "Graphics/LifetimeTracked.h"

namespace rainbow
{
    /// <summary>A drawable interface.</summary>
    class IDrawable : public LifetimeTracked<IDrawable>
    {
    public:
        virtual ~IDrawable() = default;

        auto get() { return this; }

        void draw() { draw_impl(); }
        void update(uint64_t dt) { update_impl(dt); }

    private:
        virtual void draw_impl() = 0;
        virtual void update_impl(uint64_t dt) = 0;
    };
}  // namespace rainbow

#endif
