// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREATLAS_H_
#define GRAPHICS_TEXTUREATLAS_H_

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

#endif
