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
#include "Graphics/RenderUnit.h"

namespace rainbow::vk
{
    class CommandBuffer;
    class IndexBuffer;
}  // namespace rainbow::vk

namespace rainbow::graphics
{
    class RenderNode
    {
    public:
        using variant_type = RenderUnit;

        template <typename T>
        RenderNode(T& unit, std::string tag = {})
            : enabled_(true), unit_(&unit), tag_(std::move(tag))
        {
        }

        template <typename T>
        RenderNode(std::shared_ptr<T>& variant, std::string tag = {})
            : RenderNode(*variant, std::move(tag))
        {
        }

        template <typename T>
        RenderNode(std::unique_ptr<T>& variant, std::string tag = {})
            : RenderNode(*variant, std::move(tag))
        {
        }

        auto is_enabled() const { return enabled_; }
        auto tag() const -> std::string_view { return tag_; }
        auto unit() const -> const variant_type& { return unit_; }

        void set_tag(std::string tag) { tag_ = std::move(tag); }

        void disable() { enabled_ = false; }
        void enable() { enabled_ = true; }

        friend bool operator==(const RenderNode& lhs, const RenderNode& rhs)
        {
            return lhs.unit_ == rhs.unit_;
        }

        friend bool operator!=(const RenderNode& lhs, const RenderNode& rhs)
        {
            return !(lhs == rhs);
        }

        friend bool operator==(czstring tag, const RenderNode& unit)
        {
            return tag == unit.tag();
        }

        friend bool operator==(const RenderNode& unit, czstring tag)
        {
            return unit.tag() == tag;
        }

    private:
        bool enabled_;
        variant_type unit_;
        std::string tag_;
    };

    using RenderQueue = std::vector<RenderNode>;

    void draw(const vk::CommandBuffer&,
              const RenderQueue&,
              const vk::IndexBuffer&);

    void update(RenderQueue& queue, uint64_t dt);
}  // namespace rainbow::graphics

#endif
