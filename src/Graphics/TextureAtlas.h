// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREATLAS_H_
#define GRAPHICS_TEXTUREATLAS_H_

#include <array>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "Math/Vec2.h"
#include "Memory/Array.h"

namespace rainbow
{
    namespace graphics
    {
        class TextureManager;
    }

    struct Rect
    {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
    };

    /// <summary>Texture atlas loaded from an image file.</summary>
    /// <remarks>
    ///   <list type="bullet">
    ///     <item>
    ///       Textures are loaded "upside-down", so the coordinates must be
    ///       flipped.
    ///     </item>
    ///     <item>
    ///       iOS: Textures' dimension must be 2<sup>n</sup> by 2<sup>m</sup>
    ///       for some arbitrary n and m, where n, m > 6.
    ///     </item>
    ///   </list>
    ///   References
    ///   <list type="bullet">
    ///     <item>http://iphonedevelopment.blogspot.com/2009/05/opengl-es-from-ground-up-part-6_25.html</item>
    ///     <item>http://developer.android.com/guide/topics/resources/providing-resources.html</item>
    ///     <item>http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures</item>
    ///   </list>
    /// </remarks>
    class TextureAtlas : public std::enable_shared_from_this<TextureAtlas>
    {
    public:
        TextureAtlas(const graphics::TextureManager& texture_manager,
                     std::string_view path);

        template <typename... Args>
        TextureAtlas(const graphics::TextureManager& texture_manager,
                     std::string_view path,
                     Rect region,
                     Args&&... args)
            : TextureAtlas(texture_manager, path)
        {
            add(region, std::forward<Args>(args)...);
        }

        auto hash() const { return hash_; }
        auto path() const { return std::string_view{path_}; }

        auto texture_manager() const -> const graphics::TextureManager&
        {
            return texture_manager_;
        }

        /// <summary>Adds a texture region.</summary>
        /// <param name="region">Region to add.</param>
        /// <returns>The id of the region.</returns>
        auto add(Rect region) -> uint32_t;

        /// <summary>Adds multiple texture regions.</summary>
        /// <param name="regions">Regions to add.</param>
        template <typename... Args>
        void add(Args&&... regions)
        {
            static_assert((std::is_convertible_v<Args, Rect> && ...));

            regions_.reserve(regions_.size() + sizeof...(Args));
            (regions_.push_back(std::forward<Args>(regions)), ...);
        }

        /// <summary>
        ///   Replaces the current set of texture regions with the set in the
        ///   specified array.
        /// </summary>
        void set(ArrayView<Rect>);

        /// <summary>Trims the internal texture region storage.</summary>
        void trim() { regions_.shrink_to_fit(); }

        /// <summary>
        ///   Returns vertices for the texture region with specified id.
        /// </summary>
        /// <remarks>
        ///   <code>
        ///     3 ┌─────┐ 2
        ///       │     │
        ///       │     │
        ///     0 └─────┘ 1
        ///   </code>
        ///   Textures are read into memory upside-down. Therefore, the order of
        ///   the UV coordinates are flipped vertically, giving us (3,2,1) and
        ///   (1,0,3).
        /// </remarks>
        auto operator[](uint32_t i) const
        {
            const auto& rect = regions_[i];
            return std::array<Vec2f, 4>{{
                {rect.x, rect.y + rect.height},
                {rect.x + rect.width, rect.y + rect.height},
                {rect.x + rect.width, rect.y},
                {rect.x, rect.y},
            }};
        }

    private:
        std::vector<Rect> regions_;
        std::string path_;
        size_t hash_;
        const graphics::TextureManager& texture_manager_;

#ifdef RAINBOW_TEST
    public:
        explicit TextureAtlas(const ISolemnlySwearThatIAmOnlyTesting&)
            : path_("test")
        {
        }
#endif
    };

    namespace vk
    {
        class CommandBuffer;

        void update_descriptor(const CommandBuffer&,
                               const TextureAtlas&,
                               uint32_t binding = 1);
    }  // namespace vk
}  // namespace rainbow

#endif
