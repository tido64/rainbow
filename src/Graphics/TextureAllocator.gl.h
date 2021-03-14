// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREALLOCATOR_GL_H_
#define GRAPHICS_TEXTUREALLOCATOR_GL_H_

#include "Graphics/Texture.h"

namespace rainbow::graphics::gl
{
    struct TextureAllocator final : public ITextureAllocator {
        void construct(TextureHandle&,
                       const Image&,
                       Filter mag_filter,
                       Filter min_filter) override;

        void destroy(TextureHandle&) override;

        [[maybe_unused, nodiscard]] auto max_size() const noexcept
            -> size_t override;

        void update(const TextureHandle&,
                    const Image&,
                    Filter mag_filter,
                    Filter min_filter) override;
    };
}  // namespace rainbow::graphics::gl

#endif
