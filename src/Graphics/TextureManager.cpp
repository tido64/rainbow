// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureManager.h"

#include "Graphics/Renderer.h"

using rainbow::Texture;
using rainbow::graphics::TextureFilter;

namespace
{
#ifndef NDEBUG
    void assert_texture_size(unsigned int width, unsigned int height)
    {
        const unsigned int max_texture_size =
            rainbow::graphics::max_texture_size();
        R_ASSERT(width <= max_texture_size && height <= max_texture_size,
                 "Texture dimension exceeds max texture size supported by "
                 "hardware");
    }
#else
#   define assert_texture_size(...) static_cast<void>(0)
#endif

    template <typename Container, typename T, typename F>
    void perform_if(Container& container, const T& value, F&& action)
    {
        for (auto&& element : container)
        {
            if (element == value)
            {
                action(element);
                break;
            }
        }
    }

    int texture_filter(TextureFilter filter)
    {
        switch (filter)
        {
            case TextureFilter::Nearest:
                return GL_NEAREST;
            default:
                return GL_LINEAR;
        }
    }
}

Texture::Texture(const Texture& texture)
    : name_(texture.name_), size_(texture.size_)
{
    TextureManager::Get()->retain(*this);
}

Texture::~Texture()
{
    if (name_ == 0)
        return;

    TextureManager::Get()->release(*this);
}

void Texture::bind() const
{
    TextureManager::Get()->bind(name_);
}

void Texture::bind(unsigned int unit) const
{
    TextureManager::Get()->bind(name_, unit);
}

auto Texture::operator=(Texture&& texture) -> Texture&
{
    if (name_ > 0)
        TextureManager::Get()->release(*this);
    name_ = texture.name_;
    size_ = texture.size_;
    texture.name_ = 0;
    texture.size_ = Vec2u::Zero;
    return *this;
}

void TextureManager::set_filter(TextureFilter filter)
{
    mag_filter_ = filter;
    min_filter_ = filter;
}

void TextureManager::bind(unsigned int name)
{
    if (name == active_[0])
        return;

    glBindTexture(GL_TEXTURE_2D, name);
    active_[0] = name;
}

void TextureManager::bind(unsigned int name, unsigned int unit)
{
    R_ASSERT(unit < kNumTextureUnits, "Invalid texture unit");

    if (unit >= kNumTextureUnits || name == active_[unit])
        return;

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, name);
    glActiveTexture(GL_TEXTURE0);
    active_[unit] = name;
}

void TextureManager::trim()
{
    auto first = std::remove_if(
        textures_.begin(),
        textures_.end(),
        [](const rainbow::detail::Texture& texture) {
            return texture.use_count == 0;
        });
    auto end = textures_.end();
    if (first == end)
        return;

    for (auto i = first; i != end; ++i)
    {
        glDeleteTextures(1, &i->name);
#if RAINBOW_RECORD_VMEM_USAGE
        mem_used_ -= i->size;
#endif
    }

    textures_.erase(first, end);

#if RAINBOW_RECORD_VMEM_USAGE
    update_usage();
#endif
}

TextureManager::TextureManager()
    : mag_filter_(TextureFilter::Linear), min_filter_(TextureFilter::Linear)
#if RAINBOW_RECORD_VMEM_USAGE
    , mem_peak_(0.0), mem_used_(0.0)
#endif
{
    std::fill_n(active_, kNumTextureUnits, 0);
    make_global();
}

TextureManager::~TextureManager()
{
    for (const rainbow::detail::Texture& texture : textures_)
        glDeleteTextures(1, &texture.name);
}

auto TextureManager::create_texture(const char* id) -> Texture
{
    GLuint name;
    glGenTextures(1, &name);
    textures_.emplace_back(id, name);

    bind(name);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_filter(min_filter_));
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_filter(mag_filter_));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return textures_.back();
}

void TextureManager::upload(const Texture& texture,
                            unsigned int internal_format,
                            unsigned int width,
                            unsigned int height,
                            unsigned int format,
                            const void* data)
{
    assert_texture_size(width, height);

    bind(texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);

    R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to upload texture");

    perform_if(textures_,
               texture,
               [this, width, height](rainbow::detail::Texture& texture) {
                   texture.width = width;
                   texture.height = height;
                   texture.size = width * height * 4;
#if RAINBOW_RECORD_VMEM_USAGE
                   mem_used_ += texture.size;
                   update_usage();
#endif
               });
}

void TextureManager::upload_compressed(const Texture& texture,
                                       unsigned int format,
                                       unsigned int width,
                                       unsigned int height,
                                       unsigned int size,
                                       const void* data)
{
    assert_texture_size(width, height);

    bind(texture);
    glCompressedTexImage2D(
        GL_TEXTURE_2D, 0, format, width, height, 0, size, data);

    R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to upload texture");

    perform_if(textures_,
               texture,
               [this, width, height, size](rainbow::detail::Texture& texture) {
                   texture.width = width;
                   texture.height = height;
                   texture.size = size;
#if RAINBOW_RECORD_VMEM_USAGE
                   mem_used_ += texture.size;
                   update_usage();
#endif
               });
}

void TextureManager::release(const Texture& t)
{
    perform_if(textures_, t, [](rainbow::detail::Texture& texture) {
        --texture.use_count;
    });
}

void TextureManager::retain(const Texture& t)
{
    perform_if(textures_, t, [](rainbow::detail::Texture& texture) {
        ++texture.use_count;
    });
}

#if RAINBOW_RECORD_VMEM_USAGE
auto TextureManager::memory_usage() const -> TextureManager::MemoryUsage
{
    const double M = 1e-6;
    return {mem_used_ * M, mem_peak_ * M};
}

void TextureManager::update_usage()
{
    if (mem_used_ > mem_peak_)
        mem_peak_ = mem_used_;

    LOGD("Video: %.2f MBs of textures", memory_usage().used);
}
#endif
