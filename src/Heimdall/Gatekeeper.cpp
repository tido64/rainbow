// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/Gatekeeper.h"

using heimdall::Gatekeeper;
using rainbow::Vec2i;

#ifdef RAINBOW_OS_MACOS
#   define HORIZONTAL "────────────────────────────────────"
#else
#   define HORIZONTAL "───────────────────────────────────────────"
#endif

Gatekeeper::Gatekeeper()
    : overlay_(director_.texture_manager(), director_.render_queue()),
      overlay_activator_(&overlay_)
{
}

void Gatekeeper::init(const Vec2i& screen)
{
    overlay_.initialize();
    director_.input().subscribe(overlay_);
    director_.input().subscribe(overlay_activator_);

    LOGI("┌" HORIZONTAL "┐");
    LOGI("│  " RAINBOW_SHORTCUT_DIAGNOSTIC_TOOLS ".  │");
    LOGI("└" HORIZONTAL "┘");

    director_.init(screen);
}

void Gatekeeper::update(uint64_t dt)
{
    director_.update(dt);

    if (!overlay_.is_enabled())
        overlay_activator_.update(dt);

    overlay_.update(dt);
}
