// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREMANAGER_H_
#define GRAPHICS_TEXTUREMANAGER_H_

#include <vector>

#include "Common/Global.h"
#include "Graphics/Texture.h"

#define RAINBOW_RECORD_VMEM_USAGE !defined(NDEBUG) || defined(USE_HEIMDALL)

namespace rainbow { namespace graphics
{
    struct State;
}}

/// <summary>Manages texture resources.</summary>
class TextureManager : public Global<TextureManager>
{
public:
    auto mag_filter() const { return mag_filter_; }
    auto min_filter() const { return min_filter_; }

    /// <summary>Sets texture filtering function.</summary>
    /// <remarks>
    ///   Existing textures are not affected by this setting. Valid values are
    ///   <see cref="GL_NEAREST"/> and <see cref="GL_LINEAR"/>.
    /// </remarks>
    void set_filter(int filter);

    /// <summary>Makes texture active on current rendering target.</summary>
    /// <param name="name">
    ///   Name of texture. If omitted, binds the default texture.
    /// </param>
    void bind(unsigned int name = 0);

    /// <summary>Makes texture active on specified unit.</summary>
    /// <param name="name">Name of texture.</param>
    /// <param name="unit">Texture unit to bind to.</param>
    void bind(unsigned int name, unsigned int unit);

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
    auto create(const char* id, F&& loader) -> rainbow::Texture
    {
        auto t = std::find(textures_.begin(), textures_.end(), id);
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
    /// <param name="internal_format">Internal format of the texture.</param>
    /// <param name="width">Width of the texture.</param>
    /// <param name="height">Height of the texture.</param>
    /// <param name="format">Format of the image data.</param>
    /// <param name="data">Image data.</param>
    void upload(const rainbow::Texture& texture,
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
    void upload_compressed(const rainbow::Texture& texture,
                           unsigned int format,
                           unsigned int width,
                           unsigned int height,
                           unsigned int size,
                           const void* data);

#if RAINBOW_RECORD_VMEM_USAGE
    struct MemoryUsage
    {
        double used;
        double peak;
    };

    /// <summary>Returns total video memory used by textures.</summary>
    auto memory_usage() const -> MemoryUsage;
#endif

private:
    static const size_t kNumTextureUnits = 2;

    unsigned int active_[kNumTextureUnits];
    std::vector<rainbow::detail::Texture> textures_;
    int mag_filter_;
    int min_filter_;

#if RAINBOW_RECORD_VMEM_USAGE
    double mem_peak_;
    double mem_used_;
#endif

    TextureManager();
    ~TextureManager();

    auto create_texture(const char* id) -> rainbow::Texture;

    void release(const rainbow::Texture& t);
    void retain(const rainbow::Texture& t);

#if RAINBOW_RECORD_VMEM_USAGE
    /// <summary>Updates and prints total texture memory used.</summary>
    void update_usage();
#endif

    friend rainbow::Texture;
    friend rainbow::graphics::State;
};

#endif
