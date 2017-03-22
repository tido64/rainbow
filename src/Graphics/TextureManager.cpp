// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureManager.h"

#include "Graphics/Renderer.h"

using rainbow::Passkey;
using rainbow::graphics::Texture;
using rainbow::graphics::TextureFilter;
using rainbow::graphics::TextureManager;

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
#endif

    auto texture_filter(TextureFilter filter) -> int
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
    TextureManager::Get()->retain(*this, {});
}

Texture::~Texture()
{
    if (name_ == 0)
        return;

    TextureManager::Get()->release(*this, {});
}

void Texture::bind() const
{
    TextureManager::Get()->bind(name_);
}

void Texture::bind(uint32_t unit) const
{
    TextureManager::Get()->bind(name_, unit);
}

auto Texture::operator=(Texture&& texture) noexcept -> Texture&
{
    if (name_ > 0)
        TextureManager::Get()->release(*this, {});
    name_ = texture.name_;
    size_ = texture.size_;
    texture.name_ = 0;
    texture.size_ = Vec2u::Zero;
    return *this;
}

TextureManager::TextureManager(const Passkey<rainbow::graphics::State>&)
    : mag_filter_(TextureFilter::Linear), min_filter_(TextureFilter::Linear)
#if RAINBOW_DEVMODE
    , mem_peak_(0.0), mem_used_(0.0)
#endif
{
    std::fill_n(active_, kNumTextureUnits, 0);
    make_global();
}

TextureManager::~TextureManager()
{
    for (const detail::Texture& texture : textures_)
        glDeleteTextures(1, &texture.name);
}

void TextureManager::set_filter(TextureFilter filter)
{
    mag_filter_ = filter;
    min_filter_ = filter;
}

void TextureManager::bind(uint32_t name)
{
    if (name == active_[0])
        return;

    glBindTexture(GL_TEXTURE_2D, name);
    active_[0] = name;
}

void TextureManager::bind(uint32_t name, uint32_t unit)
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
        [](const detail::Texture& texture) { return texture.use_count == 0; });
    auto end = textures_.end();
    if (first == end)
        return;

    std::for_each(  //
        first,
        end,
#if RAINBOW_DEVMODE
        [this](const detail::Texture& texture) {
            mem_used_ -= texture.size;
#else
        [](const detail::Texture& texture) {
#endif
            glDeleteTextures(1, &texture.name);
        });

    textures_.erase(first, end);

    IF_DEVMODE(update_usage());
}

void TextureManager::upload(const Texture& texture,
                            unsigned int internal_format,
                            unsigned int width,
                            unsigned int height,
                            unsigned int format,
                            const void* data)
{
    IF_DEBUG(assert_texture_size(width, height));

    bind(texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);

    R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to upload texture");

    find_invoke(
        textures_, texture, [this, width, height](detail::Texture& texture) {
            texture.width = width;
            texture.height = height;
            texture.size = width * height * 4;
#if RAINBOW_DEVMODE
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
    IF_DEBUG(assert_texture_size(width, height));

    bind(texture);
    glCompressedTexImage2D(
        GL_TEXTURE_2D, 0, format, width, height, 0, size, data);

    R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to upload texture");

    find_invoke(textures_,
                texture,
                [this, width, height, size](detail::Texture& texture) {
                    texture.width = width;
                    texture.height = height;
                    texture.size = size;
#if RAINBOW_DEVMODE
                    mem_used_ += texture.size;
                    update_usage();
#endif
                });
}

void TextureManager::release(const Texture& t, const Passkey<Texture>&)
{
    find_invoke(
        textures_, t, [](detail::Texture& texture) { --texture.use_count; });
}

void TextureManager::retain(const Texture& t, const Passkey<Texture>&)
{
    find_invoke(
        textures_, t, [](detail::Texture& texture) { ++texture.use_count; });
}

auto TextureManager::create_texture(std::string id) -> Texture
{
    GLuint name;
    glGenTextures(1, &name);
    textures_.emplace_back(std::move(id), name);

    bind(name);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_filter(min_filter_));
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_filter(mag_filter_));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return textures_.back();
}

#if RAINBOW_DEVMODE
auto TextureManager::memory_usage() const -> TextureManager::MemoryUsage
{
    constexpr double M = 1e-6;
    return {mem_used_ * M, mem_peak_ * M};
}

void TextureManager::update_usage()
{
    if (mem_used_ > mem_peak_)
        mem_peak_ = mem_used_;

    LOGD("Video: %.2f MBs of textures", memory_usage().used);
}
#endif
