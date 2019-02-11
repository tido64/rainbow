// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/NoGame.h"

#include "Resources/Rainbow.svg.h"
#include "Script/Transition.h"

using rainbow::NoGame;
using rainbow::Vec2i;

void NoGame::init_impl(const Vec2i& screen)
{
    constexpr float kDesiredHeight = 1440.0f;

    const float scale = screen.y / kDesiredHeight;
    const float logo_scale = 0.1f * scale;
    const uint32_t logo_width = assets::kLogoWidth * logo_scale;
    const uint32_t logo_height = assets::kLogoHeight * logo_scale;

    auto texture = std::make_shared<TextureAtlas>(
        assets::kLogoURI,
        Data::from_literal(assets::kLogo),
        logo_scale,
        std::make_tuple(0, 64, 0, 0),                     // 0
        std::make_tuple(0, 0, logo_width, logo_height));  // 1

    const uint32_t logo_padding = 32 * scale;
    const Vec2f center{screen.x * 0.5f, screen.y * 0.5f};

    batch_ = make_spritebatch(
        texture,
        Sprite{logo_width + logo_padding, logo_height + logo_padding}  // 0
            .position(center)
            .texture(0)
            .color(0xffffff00),
        Sprite{logo_width, logo_height}  // 1
            .position(center)
            .texture(1)
            .color(0x000000ff));

    render_queue().emplace_back(batch_);

#if !defined(RAINBOW_OS_IOS)  // TODO: Can't access system fonts on iOS
    if (error_)
    {
        label_ = make_label();
        label_->set_color(Color{0xF4, 0x43, 0x36});
        label_->set_font_size(24 * scale);
        label_->set_text(error_.message().c_str());

        // Force an update to get measurements early.
        label_->update();
        const float margin = 16 * scale;
        label_->set_position(
            {margin, narrow_cast<float>(screen.y) - label_->height() - margin});

        render_queue().emplace_back(label_);
    }
#endif
}

void NoGame::update_impl(uint64_t)
{
    static bool run_once = false;
    if (!run_once)
    {
        run_once = true;
        rainbow::fade(&batch_->at(0), 1.0f, 2000, &rainbow::timing::linear);
    }
}
