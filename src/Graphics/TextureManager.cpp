// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureManager.h"

#include "FileSystem/File.h"
#include "Graphics/Image.h"
#include "Graphics/Vulkan.h"

using rainbow::Image;
using rainbow::graphics::Filter;
using rainbow::graphics::Texture;
using rainbow::graphics::TextureManager;

namespace gfx = rainbow::graphics;
namespace vk = rainbow::vk;

namespace
{
    constexpr auto to_filter(Filter f)
    {
        switch (f)
        {
            case Filter::Nearest:
                return vk::Filter::Nearest;
            case Filter::Linear:
                return vk::Filter::Linear;
            case Filter::Cubic:
                return vk::Filter::CubicIMG;
        }
    }

    template <typename T>
    constexpr auto to_texture(T& texture)
    {
        if constexpr (std::is_const_v<std::remove_reference_t<T>>)
            return reinterpret_cast<const vk::Texture*>(texture.data);
        else
            return reinterpret_cast<vk::Texture*>(texture.data);
    }
}  // namespace

TextureManager::~TextureManager()
{
    for (auto&& texture : texture_map_)
        to_texture(texture.second)->destroy(device_);
}

auto TextureManager::get(std::string_view path,
                         float scale,
                         Filter mag_filter,
                         Filter min_filter) -> const Texture&
{
    auto [iter, inserted] = texture_map_.try_emplace(path);
    if (inserted)
    {
        auto data = File::read(path.data(), FileType::Asset);
        load(Image::decode(data, scale), mag_filter, min_filter, iter);
    }
    return iter->second;
}

auto TextureManager::get(std::string_view path,
                         const Image& image,
                         Filter mag_filter,
                         Filter min_filter) -> const Texture&
{
    auto [iter, inserted] = texture_map_.try_emplace(path);
    if (inserted)
        load(image, mag_filter, min_filter, iter);
    return iter->second;
}

void TextureManager::release(std::string_view path)
{
    if (auto node = texture_map_.extract(path))
    {
        IF_DEVMODE(mem_used_ -= node.mapped().size);
        to_texture(node.mapped())->destroy(device_);
    }
}

auto TextureManager::try_get(std::string_view path) const
    -> std::optional<Texture>
{
    return try_get(path, texture_map_.hash_function()(path));
}

auto TextureManager::try_get(std::string_view path, size_t hash) const
    -> std::optional<Texture>
{
    auto iter = texture_map_.find(path, hash);
    return iter == texture_map_.end() ? std::nullopt
                                      : std::make_optional(iter->second);
}

void TextureManager::update(std::string_view path,
                            size_t hash,
                            const Image& image) const
{
    auto texture = try_get(path, hash);
    if (!texture)
        return;

    to_texture(*texture)->update(device_, image);
}

void TextureManager::load(const Image& image,
                          Filter mag_filter,
                          Filter min_filter,
                          TextureManager::container_type::iterator iter)
{
    static_assert(sizeof(Texture::data) == sizeof(vk::Texture));

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

auto vk::to_texture(const gfx::Texture& texture) -> const vk::Texture&
{
    return *::to_texture(texture);
}

void vk::update_descriptor(const vk::CommandBuffer& command_buffer,
                           const gfx::Texture& texture,
                           uint32_t binding)
{
    vk::update_descriptor(command_buffer, *::to_texture(texture), binding);
}
