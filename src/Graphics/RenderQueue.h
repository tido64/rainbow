// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_RENDERQUEUE_H_
#define GRAPHICS_RENDERQUEUE_H_

#include <memory>
#include <string>
#include <vector>

#include "Common/String.h"
#include "Common/Variant.h"

namespace rainbow
{
    class Animation;
    class IDrawable;
    class Label;
    class SpriteBatch;
}  // namespace rainbow

namespace rainbow::graphics
{
    struct Context;

    class RenderUnit
    {
    public:
        using variant_type = variant<  //
            Animation*,
            IDrawable*,
            Label*,
            SpriteBatch*>;

        template <typename T>
        RenderUnit(T& variant, std::string tag = {})
            : variant_(&variant), tag_(std::move(tag))
        {
        }

        template <typename T>
        RenderUnit(std::shared_ptr<T>& variant, std::string tag = {})
            : RenderUnit(*variant, std::move(tag))
        {
        }

        template <typename T>
        RenderUnit(std::unique_ptr<T>& variant, std::string tag = {})
            : RenderUnit(*variant, std::move(tag))
        {
        }

        [[nodiscard]] auto is_enabled() const { return enabled_; }

        [[nodiscard]] auto object() const -> const variant_type&
        {
            return variant_;
        }

        [[nodiscard]] auto tag() const -> std::string_view { return tag_; }

        void set_tag(std::string_view tag) { tag_ = tag; }

        void disable() { enabled_ = false; }
        void enable() { enabled_ = true; }

        friend auto operator==(const RenderUnit& lhs, const RenderUnit& rhs)
        {
            return lhs.variant_ == rhs.variant_;
        }

        friend auto operator!=(const RenderUnit& lhs, const RenderUnit& rhs)
        {
            return !(lhs == rhs);
        }

        friend auto operator==(czstring tag, const RenderUnit& unit)
        {
            return tag == unit.tag();
        }

        friend auto operator==(const RenderUnit& unit, czstring tag)
        {
            return unit.tag() == tag;
        }

    private:
        bool enabled_ = true;
        variant_type variant_;
        std::string tag_;
    };

    using RenderQueue = std::vector<RenderUnit>;

    void draw(graphics::Context&, RenderQueue&);

    void update(graphics::Context&, RenderQueue&, uint64_t dt);

    template <typename F>
    void visit_all(F&& f, RenderQueue& queue)
    {
        for (auto&& unit : queue)
        {
            if (!unit.is_enabled())
                continue;

            visit(f, unit.object());
        }
    }
}  // namespace rainbow::graphics

#endif
