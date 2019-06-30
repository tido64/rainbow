// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREALLOCATOR_VK_H_
#define GRAPHICS_TEXTUREALLOCATOR_VK_H_

#include <vulkan/vulkan_core.h>

#include "Graphics/Texture.h"

namespace rainbow::vk
{
    class Context;
}  // namespace rainbow::vk

namespace rainbow::graphics::vk
{
    class TextureAllocator final : public ITextureAllocator
    {
    public:
        TextureAllocator(rainbow::vk::Context&);
        ~TextureAllocator();

        void construct(TextureHandle&,
                       const Image&,
                       Filter mag_filter,
                       Filter min_filter) override;

        void destroy(TextureHandle&) override;

        [[maybe_unused, nodiscard]]
        auto max_size() const noexcept -> size_t override;

        void update(const TextureHandle&,
                    const Image&,
                    Filter mag_filter,
                    Filter min_filter) override;

    private:
        rainbow::vk::Context& context_;
        VkSampler default_sampler_ = VK_NULL_HANDLE;
    };
}  // namespace rainbow::graphics::vk

#endif
