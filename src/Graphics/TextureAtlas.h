// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREATLAS_H_
#define GRAPHICS_TEXTUREATLAS_H_

#include <array>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

#include "Common/Data.h"
#include "Common/String.h"
#include "Graphics/Texture.h"
#include "Memory/Array.h"
#include "Memory/SharedPtr.h"

namespace rainbow
{
    namespace graphics
    {
        class TextureManager;
    }

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
    class TextureAtlas : public RefCounted
    {
    public:
        explicit TextureAtlas(czstring path, float scale = 1.0f);

        template <typename... Args>
        TextureAtlas(czstring path, float scale, Args&&... regions)
            : TextureAtlas(path, scale)
        {
            regions_.reserve(sizeof...(regions));
            add_regions(std::forward<Args>(regions)...);
        }

        TextureAtlas(czstring id, const Data& data, float scale = 1.0f);

        template <typename... Args>
        TextureAtlas(czstring id,
                     const Data& data,
                     float scale,
                     Args&&... regions)
            : TextureAtlas(id, data, scale)
        {
            regions_.reserve(sizeof...(regions));
            add_regions(std::forward<Args>(regions)...);
        }

        auto height() const { return texture_.height(); }
        auto is_valid() const { return texture_; }
        auto size() const { return regions_.size(); }
        auto width() const { return texture_.width(); }

        /// <summary>Binds this texture.</summary>
        void bind() const { texture_.bind(); }

        /// <summary>Binds this texture to specified texture unit.</summary>
        void bind(uint32_t unit) const { texture_.bind(unit); }

        /// <summary>Adds a texture region.</summary>
        /// <param name="x">Starting point of the region (x-coordinate).</param>
        /// <param name="y">Starting point of the region (y-coordinate).</param>
        /// <param name="width">Width of the region.</param>
        /// <param name="height">Height of the region.</param>
        /// <returns>The id of the region.</returns>
        auto add_region(int x, int y, int width, int height) -> uint32_t;

        /// <summary>
        ///   Replaces the current set of texture regions with the set in the
        ///   specified array.
        /// </summary>
        void set_regions(const ArrayView<int>& rectangles);

        /// <summary>Trims the internal texture region storage.</summary>
        void trim() { regions_.shrink_to_fit(); }

        auto operator[](uint32_t i) const -> const graphics::TextureRegion&
        {
            return regions_[i];
        }

        explicit TextureAtlas(const ISolemnlySwearThatIAmOnlyTesting& test)
            : texture_(test)
        {
        }

    private:
        template <std::size_t I, typename T>
        static constexpr bool is_integral_v =
            std::is_integral_v<typename std::tuple_element<I, T>::type>;

        graphics::Texture texture_;                     ///< Texture atlas' id.
        std::vector<graphics::TextureRegion> regions_;  ///< Defined texture regions.

        void add_regions() {}

        template <typename T, typename... Args>
        void add_regions(const T& region, Args&&... regions)
        {
            static_assert(is_integral_v<0, T> &&      //
                              is_integral_v<1, T> &&  //
                              is_integral_v<2, T> &&  //
                              is_integral_v<3, T>,
                          "Tuple elements must be of integral type");

            add_region(std::get<0>(region),
                       std::get<1>(region),
                       std::get<2>(region),
                       std::get<3>(region));
            add_regions(std::forward<Args>(regions)...);
        }

        void load(graphics::TextureManager& texture_manager,
                  const graphics::Texture& texture,
                  const Data& data,
                  float scale);
    };
}  // namespace rainbow

namespace rainbow::v2
{
    class TextureAtlas;
}

namespace rainbow::graphics::v2
{
    class TextureMap;
}

namespace rainbow::vk
{
    class CommandBuffer;

    void update_descriptor(const CommandBuffer&,
                           const v2::TextureAtlas&,
                           uint32_t binding = 1);
}  // namespace rainbow::vk

namespace rainbow::v2
{
    struct Rect
    {
        float x;
        float y;
        float width;
        float height;
    };

    class TextureAtlas : public RefCounted
    {
    public:
        TextureAtlas(const graphics::v2::TextureMap& texture_manager,
                     std::string_view path)
            : path_(path), texture_manager_(texture_manager)
        {
        }

        template <typename... Args>
        TextureAtlas(graphics::v2::TextureMap& texture_manager,
                     std::string_view path,
                     Args&&... args)
            : TextureAtlas(texture_manager, path)
        {
            add(std::forward<Args>(args)...);
        }

        auto path() const { return path_; }

        auto texture_manager() const -> const graphics::v2::TextureMap&
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
        auto operator[](uint32_t i) const -> std::array<Vec2f, 6>
        {
            const auto& rect = regions_[i];
            return {{
                {rect.x, rect.y + rect.height},
                {rect.x + rect.width, rect.y + rect.height},
                {rect.x + rect.width, rect.y},
                {rect.x + rect.width, rect.y},
                {rect.x, rect.y},
                {rect.x, rect.y + rect.height},
            }};
        }

    private:
        std::vector<Rect> regions_;
        std::string_view path_;
        const graphics::v2::TextureMap& texture_manager_;

#ifdef RAINBOW_TEST
    public:
        explicit TextureAtlas(const ISolemnlySwearThatIAmOnlyTesting&)
            : path_("test")
        {
        }
#endif
    };
}  // namespace rainbow::v2

#endif
