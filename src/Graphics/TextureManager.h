// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREMANAGER_H_
#define GRAPHICS_TEXTUREMANAGER_H_

#include <vector>

#include "Common/Global.h"
#include "Common/Passkey.h"
#include "Graphics/Texture.h"

namespace rainbow { namespace graphics
{
    struct State;

    enum class TextureFilter
    {
        Linear,
        Nearest,
        TextureFilterCount
    };

    /// <summary>Manages texture resources.</summary>
    class TextureManager : public Global<TextureManager>
    {
    public:
        TextureManager(const Passkey<State>&);
        ~TextureManager();

        auto mag_filter() const { return mag_filter_; }
        auto min_filter() const { return min_filter_; }

        /// <summary>Sets texture filtering function.</summary>
        /// <remarks>
        ///   Existing textures are not affected by this setting.
        /// </remarks>
        void set_filter(TextureFilter filter);

        /// <summary>Makes texture active on current rendering target.</summary>
        /// <param name="name">
        ///   Name of texture. If omitted, binds the default texture.
        /// </param>
        void bind(uint32_t name = 0);

        /// <summary>Makes texture active on specified unit.</summary>
        /// <param name="name">Name of texture.</param>
        /// <param name="unit">Texture unit to bind to.</param>
        void bind(uint32_t name, uint32_t unit);

        /// <summary>
        ///   Loads texture for unique identifier, using the specified loader.
        /// </summary>
        /// <remarks>
        ///   If a texture with the same identifier already exists, it will
        ///   immediately be returned, saving a call to the loader.
        /// </remarks>
        /// <param name="id">A unique identifier.</param>
        /// <param name="loader">
        ///   Function for loading and <see cref="upload"/> data.
        /// </param>
        /// <returns>Texture name.</returns>
        template <typename F>
        auto create(const std::string& id, F&& loader) -> Texture
        {
            auto t = find(textures_, id);
            if (t == textures_.end())
            {
                loader(*this, create_texture(id));
                return textures_.back();
            }
            return *t;
        }

        /// <summary>Deletes unused textures.</summary>
        void trim();

        /// <summary>Uploads image data to specified texture.</summary>
        /// <param name="name">Target texture.</param>
        /// <param name="internal_format">
        ///   Internal format of the texture.
        /// </param>
        /// <param name="width">Width of the texture.</param>
        /// <param name="height">Height of the texture.</param>
        /// <param name="format">Format of the image data.</param>
        /// <param name="data">Image data.</param>
        void upload(const Texture& texture,
                    unsigned int internal_format,
                    unsigned int width,
                    unsigned int height,
                    unsigned int format,
                    const void* data);

        /// <summary>
        ///   Uploads compressed image data to specified texture.
        /// </summary>
        /// <param name="name">Target texture.</param>
        /// <param name="format">Compression format.</param>
        /// <param name="width">Width of the texture.</param>
        /// <param name="height">Height of the texture.</param>
        /// <param name="size">Data size.</param>
        /// <param name="data">Image data.</param>
        void upload_compressed(const Texture& texture,
                               unsigned int format,
                               unsigned int width,
                               unsigned int height,
                               unsigned int size,
                               const void* data);

        // Internal API

        /// <summary>
        ///   [Internal] Used by <see cref="Texture"/> to release itself.
        /// </summary>
        void release(const Texture& t, const Passkey<Texture>&);

        /// <summary>
        ///   [Internal] Used by <see cref="Texture"/> to retain itself.
        /// </summary>
        void retain(const Texture& t, const Passkey<Texture>&);

#if RAINBOW_DEVMODE
        struct MemoryUsage
        {
            double used;
            double peak;
        };

        /// <summary>Returns total video memory used by textures.</summary>
        auto memory_usage() const -> MemoryUsage;
#endif

    private:
        static constexpr size_t kNumTextureUnits = 2;

        uint32_t active_[kNumTextureUnits];
        std::vector<detail::Texture> textures_;
        TextureFilter mag_filter_;
        TextureFilter min_filter_;

#if RAINBOW_DEVMODE
        double mem_peak_;
        double mem_used_;
#endif

        auto create_texture(std::string id) -> Texture;

#if RAINBOW_DEVMODE
        /// <summary>Updates and prints total texture memory used.</summary>
        void update_usage();
#endif
    };
}}  // namespace rainbow::graphics

#endif
