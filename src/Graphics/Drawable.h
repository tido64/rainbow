// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_DRAWABLE_H_
#define GRAPHICS_DRAWABLE_H_

#include <cstdint>

namespace rainbow::graphics
{
    struct Context;
}  // namespace rainbow::graphics

namespace rainbow
{
    class GameBase;

    /// <summary>Interface for a drawable object.</summary>
    class IDrawable
    {
    public:
        void draw(graphics::Context& ctx) const { draw_impl(ctx); }
        void update(GameBase& ctx, uint64_t dt) { update_impl(ctx, dt); }

    protected:
        virtual ~IDrawable() = default;

    private:
        virtual void draw_impl(graphics::Context&) const = 0;
        virtual void update_impl(GameBase&, uint64_t dt) = 0;
    };
}  // namespace rainbow

#endif
