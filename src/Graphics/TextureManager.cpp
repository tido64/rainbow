// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureManager.h"

#include "Graphics/Renderer.h"

#ifdef USE_HEIMDALL
#    define IF_DEVMODE_CAPTURE(x) x
#else
#    define IF_DEVMODE_CAPTURE(x)
#endif

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
}  // namespace

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

TextureManager::TextureManager(const Passkey<rainbow::graphics::Context>&)
    : active_{}, mag_filter_(TextureFilter::Linear),
      min_filter_(TextureFilter::Linear)
#ifdef USE_HEIMDALL
    , mem_peak_(0.0), mem_used_(0.0)
#endif
{
    make_global();
}

TextureManager::~TextureManager()
{
    for (auto&& texture : textures_)
        glDeleteTextures(1, &texture.second.name);
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
    auto first = std::remove_if(  //
        textures_.begin(),
        textures_.end(),
        [IF_DEVMODE_CAPTURE(this)](auto&& texture) {
            auto& detail = texture.second;
            if (detail.use_count == 0)
            {
                IF_DEVMODE(mem_used_ -= detail.size);
                glDeleteTextures(1, &detail.name);
                return true;
            }

            return false;
        });
    auto end = textures_.end();
    if (first == end)
        return;

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

    auto t = textures_.find_value(texture);
    if (t == textures_.end())
        return;

    auto& detail = t->second;
    IF_DEVMODE(mem_used_ -= detail.size);
    detail.width = width;
    detail.height = height;
    detail.size = width * height * 4;
    IF_DEVMODE(mem_used_ += detail.size);
    IF_DEVMODE(update_usage());
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

    auto t = textures_.find_value(texture);
    if (t == textures_.end())
        return;

    auto& detail = t->second;
    detail.width = width;
    detail.height = height;
    detail.size = size;
    IF_DEVMODE(mem_used_ += detail.size);
    IF_DEVMODE(update_usage());
}

void TextureManager::release(const Texture& texture, const Passkey<Texture>&)
{
    auto t = textures_.find_value(texture);
    if (t == textures_.end())
        return;

    auto& detail = t->second;
    --detail.use_count;
}

void TextureManager::retain(const Texture& texture, const Passkey<Texture>&)
{
    auto t = textures_.find_value(texture);
    if (t == textures_.end())
        return;

    auto& detail = t->second;
    ++detail.use_count;
}

auto TextureManager::create_texture(std::string_view id) -> Texture
{
    GLuint name;
    glGenTextures(1, &name);
    textures_.emplace(id, name);

    bind(name);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_filter(min_filter_));
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_filter(mag_filter_));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return textures_.back();
}

#ifdef USE_HEIMDALL
auto TextureManager::memory_usage() const -> TextureManager::MemoryUsage
{
    constexpr double M = 1e-6;
    return {mem_used_ * M, mem_peak_ * M};
}

void TextureManager::update_usage()
{
    if (mem_used_ > mem_peak_)
        mem_peak_ = mem_used_;
}
#endif

#include "FileSystem/File.h"
#include "Graphics/Image.h"
#include "Graphics/Vulkan.h"

using rainbow::graphics::v2::Filter;
using rainbow::graphics::v2::TextureMap;
using rainbow::vk::CommandBuffer;

namespace
{
    auto to_filter(Filter f)
    {
        switch (f)
        {
            case Filter::Nearest:
                return rainbow::vk::Filter::Nearest;
            case Filter::Linear:
                return rainbow::vk::Filter::Linear;
            case Filter::Cubic:
                return rainbow::vk::Filter::CubicIMG;
        }
    }
}

void rainbow::vk::update_descriptor(
    const CommandBuffer& command_buffer,
    const rainbow::graphics::v2::Texture& texture,
    uint32_t binding)
{
    vk::update_descriptor(  //
        command_buffer,
        *reinterpret_cast<const vk::Texture*>(texture.data),
        binding);
}

TextureMap::~TextureMap()
{
    for (auto&& texture : texture_map_)
        reinterpret_cast<vk::Texture*>(texture.second.data)->destroy(device_);
}

auto TextureMap::get(std::string_view path,
                     float scale,
                     Filter mag_filter,
                     Filter min_filter) -> const Texture&
{
    static_assert(sizeof(Texture::data) == sizeof(vk::Texture));

    auto [iter, inserted] = texture_map_.try_emplace(path);
    if (inserted)
    {
        auto data = File::read(path.data(), FileType::Asset);
        auto image = Image::decode(data, scale);
        auto& texture = iter->second;
        new (texture.data) vk::Texture(
            device_, image, to_filter(mag_filter), to_filter(min_filter));
        texture.width = image.width;
        texture.height = image.height;
#ifdef USE_HEIMDALL
        texture.size = image.size;
        mem_used_ += image.size;
        if (mem_used_ > mem_peak_)
            mem_peak_ = mem_used_;
#endif
    }

    return iter->second;
}

void TextureMap::release(std::string_view path)
{
    if (auto node = texture_map_.extract(path))
    {
        IF_DEVMODE(mem_used_ -= node.mapped().size);
        reinterpret_cast<vk::Texture*>(node.mapped().data)->destroy(device_);
    }
}

auto TextureMap::try_get(std::string_view path) const -> std::optional<Texture>
{
    auto iter = texture_map_.find(path);
    return iter == texture_map_.end() ? std::nullopt
                                      : std::make_optional(iter->second);
}
