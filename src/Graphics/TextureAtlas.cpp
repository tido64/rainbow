// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureAtlas.h"

#include "Graphics/TextureManager.h"
#include "Graphics/Vulkan.h"

using rainbow::TextureAtlas;
using rainbow::graphics::TextureManager;
using rainbow::vk::CommandBuffer;

TextureAtlas::TextureAtlas(const TextureManager& texture_manager,
                           std::string_view path)
    : path_(path), hash_(texture_manager.hash_function()(path)),
      texture_manager_(texture_manager)
{
}

auto TextureAtlas::add(Rect region) -> uint32_t
{
    const auto index = regions_.size();
    regions_.push_back(region);
    return index;
}

void TextureAtlas::set(ArrayView<Rect> rects)
{
    regions_.clear();
    regions_.reserve(rects.size());
    std::copy(rects.begin(), rects.end(), std::back_inserter(regions_));
}

void rainbow::vk::update_descriptor(const CommandBuffer& command_buffer,
                                    const TextureAtlas& atlas,
                                    uint32_t binding)
{
    auto texture = atlas.texture_manager().try_get(atlas.path(), atlas.hash());
    if (!texture)
    {
        R_ABORT("'%s' has not been initialized or was already released",
                atlas.path().data());
        return;
    }

    update_descriptor(command_buffer, *texture, binding);
}
