// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTURE_H_
#define GRAPHICS_TEXTURE_H_

#include <string>
#include <utility>

#include "Math/Vec2.h"

namespace rainbow
{
    struct ISolemnlySwearThatIAmOnlyTesting;
}

namespace rainbow::graphics
{
    namespace detail
    {
        struct Texture
        {
            std::string id;
            uint32_t name;
            uint32_t width;
            uint32_t height;
            uint32_t size;
            uint32_t use_count;

            Texture(std::string id_, uint32_t name_)
                : id(std::move(id_)), name(name_), width(0), height(0), size(0),
                  use_count(0)
            {
            }

            auto operator==(uint32_t name_) { return name == name_; }
            auto operator==(const std::string& id_) { return id == id_; }
        };
    }  // namespace detail

    class Texture
    {
    public:
        Texture() : name_(0) {}

        Texture(detail::Texture& texture)
            : name_(texture.name), size_(texture.width, texture.height)
        {
            ++texture.use_count;
        }

        explicit Texture(const ISolemnlySwearThatIAmOnlyTesting&)
            : name_(0), size_(64, 64)
        {
        }

        Texture(const Texture& texture);
        ~Texture();

        auto width() const { return size_.x; }
        auto height() const { return size_.y; }

        void bind() const;
        void bind(uint32_t unit) const;

        explicit operator bool() const { return name_ != 0; }
        operator uint32_t() const { return name_; }

        auto operator=(const Texture& texture) -> Texture& = delete;
        auto operator=(Texture&& texture) noexcept -> Texture&;

    private:
        uint32_t name_;
        Vec2u size_;
    };

    /// <summary>Stores texture id and UV coordinates.</summary>
    /// <remarks>
    ///   <code>
    ///     3 ┌─────┐ 2
    ///       │     │
    ///       │     │
    ///     0 └─────┘ 1
    ///   </code>
    ///   Textures are read into memory upside-down. Therefore, the order of the
    ///   UV coordinates are flipped vertically, giving us 3,2,1 and 1,0,3.
    /// </remarks>
    struct TextureRegion
    {
        Vec2f vx[4];
        uint32_t atlas;

        TextureRegion() : atlas(0) {}

        TextureRegion(const Vec2f& v0, const Vec2f& v1) : atlas(0)
        {
            vx[0].x = v0.x;
            vx[0].y = v1.y;
            vx[1].x = v1.x;
            vx[1].y = v1.y;
            vx[2].x = v1.x;
            vx[2].y = v0.y;
            vx[3].x = v0.x;
            vx[3].y = v0.y;
        }

        operator uint32_t() const { return atlas; }
    };
}  // namespace rainbow::graphics

#endif
