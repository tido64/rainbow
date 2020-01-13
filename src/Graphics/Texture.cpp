// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Texture.h"

#include "Common/Logging.h"
#include "FileSystem/File.h"
#include "Graphics/Image.h"

using rainbow::Data;
using rainbow::File;
using rainbow::FileType;
using rainbow::Image;
using rainbow::Passkey;
using rainbow::graphics::Filter;
using rainbow::graphics::ITextureAllocator;
using rainbow::graphics::Texture;
using rainbow::graphics::TextureData;
using rainbow::graphics::TextureProvider;

TextureProvider::TextureProvider(ITextureAllocator& allocator)
    : allocator_(allocator)
{
    R_ASSERT(Texture::s_texture_provider == nullptr,  //
             "An instance already exists");

    Texture::s_texture_provider = this;
}

TextureProvider::~TextureProvider()
{
    R_ASSERT(Texture::s_texture_provider == this, "This shouldn't happen.");

    Texture::s_texture_provider = nullptr;

    for (auto&& texture : texture_map_)
        allocator_.destroy(texture.second.data);
}

template <typename T>
auto TextureProvider::get(std::string_view path,
                          [[maybe_unused]] T data,
                          [[maybe_unused]] float scale,
                          Filter mag_filter,
                          Filter min_filter) -> Texture
{
    auto [iter, inserted] = texture_map_.emplace(path, TextureData{});
    if (inserted)
    {
        if constexpr (std::is_same_v<T, std::nullptr_t>)
        {
            auto file = File::read(path.data(), FileType::Asset);
            load(iter, Image::decode(file, scale), mag_filter, min_filter);
        }
        else if constexpr (std::is_same_v<T, const Data&>)
        {
            load(iter, Image::decode(data, scale), mag_filter, min_filter);
        }
        else if constexpr (std::is_same_v<T, const Image&>)
        {
            load(iter, data, mag_filter, min_filter);
        }
    }
    ++iter->second.use_count;
    return Texture{path, Passkey<TextureProvider>{}};
}

auto TextureProvider::get(std::string_view path,
                          float scale,
                          Filter mag_filter,
                          Filter min_filter) -> Texture
{
    return get(path, nullptr, scale, mag_filter, min_filter);
}

auto TextureProvider::get(std::string_view path,
                          const Data& data,
                          float scale,
                          Filter mag_filter,
                          Filter min_filter) -> Texture
{
    return get<const Data&>(path, data, scale, mag_filter, min_filter);
}

auto TextureProvider::get(std::string_view path,
                          const Image& image,
                          Filter mag_filter,
                          Filter min_filter) -> Texture
{
    return get<const Image&>(path, image, 1.0F, mag_filter, min_filter);
}

auto TextureProvider::raw_get(const Texture& texture) const -> TextureData
{
    auto iter = texture_map_.find(texture.key());
    return iter->second;
}

void TextureProvider::release(const Texture& texture)
{
    auto iter = texture_map_.find(texture.key());
    if (iter == texture_map_.end())
        return;

    auto& texture_data = iter->second;
    if (--texture_data.use_count == 0)
    {
        IF_DEVMODE(mem_used_ -= texture_data.size);
        allocator_.destroy(texture_data.data);
        texture_map_.erase(iter);
    }
}

auto TextureProvider::try_get(const Texture& texture)
    -> std::optional<TextureData>
{
    auto iter = texture_map_.find(texture.key());
    if (iter == texture_map_.end())
        return std::nullopt;

    ++iter->second.use_count;
    return std::make_optional(iter->second);
}

void TextureProvider::update(const Texture& texture,
                             const Image& image,
                             Filter mag_filter,
                             Filter min_filter)
{
    allocator_.update(raw_get(texture).data, image, mag_filter, min_filter);
}

void TextureProvider::load(TextureMap::iterator i,
                           const Image& image,
                           Filter mag_filter,
                           Filter min_filter)
{
    R_ASSERT(allocator_.max_size() <= sizeof(TextureData::data),
             "Texture data size is too small for the current graphics API.");

    auto& texture = i->second;
    allocator_.construct(texture.data, image, mag_filter, min_filter);
    texture.width = image.width;
    texture.height = image.height;

    IF_DEVMODE(texture.size = image.size);
    IF_DEVMODE(record_usage(image.size));
}

TextureProvider* Texture::s_texture_provider = nullptr;

Texture::~Texture()
{
    if (key_.empty())
        return;

    s_texture_provider->release(*this);
}

auto Texture::operator=(const Texture& texture) -> Texture&
{
    if (&texture == this)
        return *this;

    if (!key_.empty())
        s_texture_provider->release(*this);

    auto handle = s_texture_provider->get(texture.key());
    key_ = std::move(handle.key_);
    return *this;
}

auto Texture::operator=(Texture&& texture) noexcept -> Texture&
{
    if (!key_.empty())
        s_texture_provider->release(*this);

    key_ = std::move(texture.key_);
    return *this;
}
