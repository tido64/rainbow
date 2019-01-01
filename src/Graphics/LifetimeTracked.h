// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_LIFETIMETRACKED_H_
#define GRAPHICS_LIFETIMETRACKED_H_

#ifdef NDEBUG
namespace rainbow
{
    template <typename T>
    class LifetimeTracked
    {
    protected:
        LifetimeTracked = default;
    };
}  // namespace rainbow
#else
#    include <vector>

#    include "Common/Algorithm.h"
#    include "Graphics/RenderUnit.h"

namespace rainbow
{
    namespace graphics
    {
        extern std::vector<RenderUnit> g_deleted_units;
    }

    template <typename T>
    class LifetimeTracked
    {
    protected:
        LifetimeTracked()
        {
            auto iter = find(graphics::g_deleted_units, static_cast<T*>(this));
            if (iter != std::end(graphics::g_deleted_units))
                quick_erase(graphics::g_deleted_units, iter);
        }

        ~LifetimeTracked()
        {
            graphics::g_deleted_units.emplace_back(static_cast<T*>(this));
        }
    };
}  // namespace rainbow
#endif

#endif
