// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/Scene.h"

#include "FileSystem/File.h"
#include "Graphics/Label.h"
#include "Graphics/SpriteBatch.h"
#include "JavaScript/Helper.h"

using rainbow::Animation;
using rainbow::Label;
using rainbow::Passkey;
using rainbow::Scene;
using rainbow::ScopeStack;
using rainbow::SpriteBatch;
using rainbow::TextureAtlas;
using rainbow::czstring;

namespace duk = rainbow::duk;

namespace
{
    enum class AssetType
    {
        Unknown,
        Animation,
        Audio,
        Font,
        Label,
        SpriteBatch,
        Texture,
    };

    auto get_asset_type(duk_context* ctx)
    {
        if (!duk::has_prop_literal(ctx, -1, "id"))
            return AssetType::Unknown;

        if (duk::has_prop_literal(ctx, -1, "sprite") &&
            duk::has_prop_literal(ctx, -1, "frames"))
        {
            return AssetType::Animation;
        }

        if (duk::has_prop_literal(ctx, -1, "font") &&
            duk::has_prop_literal(ctx, -1, "text"))
        {
            return AssetType::Label;
        }

        if (duk::has_prop_literal(ctx, -1, "texture") &&
            duk::has_prop_literal(ctx, -1, "sprites"))
        {
            return AssetType::SpriteBatch;
        }

        return AssetType::Unknown;
    }

    auto get_asset_type(czstring path, size_t length)
    {
        if (rainbow::ends_with({path, length}, "mp3") ||
            rainbow::ends_with({path, length}, "ogg"))
        {
            return AssetType::Audio;
        }

        if (rainbow::ends_with({path, length}, "dds") ||
            rainbow::ends_with({path, length}, "png") ||
            rainbow::ends_with({path, length}, "pvr") ||
            rainbow::ends_with({path, length}, "svg"))
        {
            return AssetType::Texture;
        }

        if (rainbow::ends_with({path, length}, "otf") ||
            rainbow::ends_with({path, length}, "ttf"))
        {
            return AssetType::Font;
        }

        return AssetType::Unknown;
    }

    template <typename T, size_t N>
    auto get_prop_literal(duk_context* ctx,
                          duk_idx_t obj_idx,
                          const char (&key)[N])
    {
        duk::get_prop_literal(ctx, obj_idx, key);
        auto prop = duk::get<T>(ctx, -1);
        duk_pop(ctx);
        return prop;
    }

    template <typename F>
    auto visit_assets(duk_context* ctx, F&& visitor)
    {
        duk::ScopedStack stack{ctx};

        if (!duk::get_prop_literal(ctx, -1, "assets"))
            return false;

        auto asset_count = static_cast<duk_uarridx_t>(duk_get_length(ctx, -1));
        for (duk_uarridx_t i = 0; i < asset_count; ++i)
        {
            duk::ScopedStack stack{ctx};

            duk_get_prop_index(ctx, -1, i);
            if (!duk::get_prop_literal(ctx, -1, "path"))
                continue;

            duk_size_t length;
            const char* path = duk_require_lstring(ctx, -1, &length);
            visitor(ctx, get_asset_type(path, length), path);
        }

        duk_pop(ctx);
        if (!duk::get_prop_literal(ctx, -1, "entities"))
            return false;

        auto entity_count = static_cast<duk_uarridx_t>(duk_get_length(ctx, -1));
        for (duk_uarridx_t i = 0; i < entity_count; ++i)
        {
            duk::ScopedStack stack{ctx};

            duk_get_prop_index(ctx, -1, i);
            visitor(ctx, get_asset_type(ctx), nullptr);
        }

        return true;
    }
}

auto Scene::from_json(duk_context* ctx, czstring path) -> std::unique_ptr<Scene>
{
    auto data = File::read(path, FileType::Asset);
    duk_push_lstring(ctx, data.as<const char*>(), data.size());
    if (duk_safe_call(  //
            ctx,
            [](duk_context* ctx, void*) -> duk_ret_t {
                duk_json_decode(ctx, -1);
                return 1;
            },
            nullptr,
            1,
            1) != DUK_EXEC_SUCCESS)
    {
        duk_pop(ctx);
        return nullptr;
    }

    size_t total_size = 0;
    visit_assets(ctx, [&total_size](duk_context*, AssetType type, czstring) {
        switch (type)
        {
            case AssetType::Animation:
                total_size += ScopeStack::size_of<Animation>();
                break;
            case AssetType::Audio:
                total_size += ScopeStack::size_of<void*>();
                break;
            case AssetType::Label:
                total_size += ScopeStack::size_of<Label>();
                break;
            case AssetType::SpriteBatch:
                total_size += ScopeStack::size_of<SpriteBatch>();
                break;
            case AssetType::Texture:
                total_size += ScopeStack::size_of<TextureAtlas>();
                break;
            default:
                break;
        }
    });

    return std::make_unique<Scene>(ctx, total_size, Passkey<Scene>{});
}

Scene::Scene(duk_context* ctx, size_t size, const Passkey<Scene>&)
    : allocator_(size), stack_(allocator_)
{
    visit_assets(ctx, [this](duk_context* ctx, AssetType type, czstring) {
        switch (type)
        {
            case AssetType::Animation:
            {
                auto animation = stack_.allocate<Animation>(
                    get_prop_literal<SpriteRef>(ctx, -1, "sprite"),
                    get_prop_literal<Animation::Frames>(ctx, -1, "frames"),
                    get_prop_literal<int>(ctx, -1, "frameRate"));

                if (duk::get_prop_literal(ctx, -1, "start") &&
                    duk_get_boolean(ctx, -1))
                {
                    animation->start();
                }

                store_asset(ctx, animation);
                break;
            }

            case AssetType::Audio:
            {
                break;
            }

            case AssetType::Label:
            {
                auto label = stack_.allocate<Label>();
                label->set_font(get_prop_literal<const char*>(ctx, -1, "font"));
                label->set_text(get_prop_literal<const char*>(ctx, -1, "text"));
                label->set_position(
                    get_prop_literal<Vec2f>(ctx, -1, "position"));

                if (duk::get_prop_literal(ctx, -1, "color"))
                    label->set_color(duk::get<Color>(ctx, -1));
                duk_pop(ctx);

                if (duk::get_prop_literal(ctx, -1, "scale"))
                    label->set_scale(duk::get<float>(ctx, -1));
                duk_pop(ctx);

                if (duk::get_prop_literal(ctx, -1, "alignment"))
                {
                    auto alignment = text_alignment_from_char(
                        duk::get<const char*>(ctx, -1)[0]);
                    label->set_alignment(alignment);
                }
                duk_pop(ctx);

                if (duk::get_prop_literal(ctx, -1, "rotation"))
                    label->set_rotation(duk::get<float>(ctx, -1));
                duk_pop(ctx);

                store_asset(ctx, label);
                break;
            }

            case AssetType::SpriteBatch:
            {
                auto texture = get<TextureAtlas>(
                    get_prop_literal<const char*>(ctx, -1, "texture"));

                duk::get_prop_literal(ctx, -1, "sprites");
                const auto sprite_count =
                    static_cast<duk_uarridx_t>(duk_get_length(ctx, -1));

                auto batch = stack_.allocate<SpriteBatch>(sprite_count);
                batch->set_texture(SharedPtr<TextureAtlas>{texture});

                for (duk_uarridx_t i = 0; i < sprite_count; ++i)
                {
                    duk::ScopedStack stack{ctx};

                    duk_get_prop_index(ctx, -1, i);

                    auto sprite = batch->create_sprite(
                        get_prop_literal<int>(ctx, -1, "width"),
                        get_prop_literal<int>(ctx, -1, "height"));

                    if (duk::get_prop_literal(ctx, -1, "mirrored") &&
                        duk::get<bool>(ctx, -1))
                    {
                        sprite->mirror();
                    }
                    duk_pop(ctx);

                    sprite->set_position(
                        get_prop_literal<Vec2f>(ctx, -1, "position"));
                    sprite->set_texture(
                        get_prop_literal<int>(ctx, -1, "texture"));

                    if (duk::get_prop_literal(ctx, -1, "color"))
                        sprite->set_color(duk::get<Color>(ctx, -1));
                    duk_pop(ctx);

                    if (duk::get_prop_literal(ctx, -1, "rotation"))
                        sprite->set_rotation(duk::get<float>(ctx, -1));
                    duk_pop(ctx);

                    if (duk::get_prop_literal(ctx, -1, "pivot"))
                        sprite->set_pivot(duk::get<Vec2f>(ctx, -1));
                    duk_pop(ctx);

                    if (duk::get_prop_literal(ctx, -1, "scale"))
                        sprite->set_scale(duk::get<float>(ctx, -1));
                    duk_pop(ctx);
                }

                store_asset(ctx, batch);
                break;
            }

            case AssetType::Texture:
            {
                auto texture = stack_.allocate<TextureAtlas>(
                    get_prop_literal<const char*>(ctx, -1, "path"));

                duk::get_prop_literal(ctx, -2, "regions");
                const auto region_count =
                    static_cast<duk_uarridx_t>(duk_get_length(ctx, -1));
                for (duk_uarridx_t i = 0; i < region_count; ++i)
                {
                    duk::ScopedStack stack{ctx};

                    duk_get_prop_index(ctx, -1, i);
                    if (duk_get_length(ctx, -1) != 4)
                    {
                        LOGW(
                            "A texture region is defined by x, y, width, "
                            "height");
                        continue;
                    }

                    duk_get_prop_index(ctx, -1, 0);
                    duk_get_prop_index(ctx, -2, 1);
                    duk_get_prop_index(ctx, -3, 2);
                    duk_get_prop_index(ctx, -4, 3);
                    texture->add_region(  //
                        duk::get<int>(ctx, -4),
                        duk::get<int>(ctx, -3),
                        duk::get<int>(ctx, -2),
                        duk::get<int>(ctx, -1));
                }

                store_asset(ctx, texture);
                break;
            }

            default:
                break;
        }
    });
}

void Scene::store_asset(duk_context* ctx, asset_type ptr)
{
    auto id = get_prop_literal<const char*>(ctx, -1, "id");
    R_ASSERT(id != nullptr, "Assets must have an id");

    [[maybe_unused]] bool inserted;
    std::tie(std::ignore, inserted) =
        asset_map_.try_emplace(id, std::move(ptr));

    if (!inserted)
        LOGW("Scene: Multiple assets declared with the same id '%s'", id);
}

void Scene::draw_impl()
{
    graphics::draw(render_queue_);
}

void Scene::update_impl(uint64_t dt)
{
    dt_ = dt;
}

void Scene::upload_impl()
{
    graphics::update_parallel(render_queue_, dt_);
}
