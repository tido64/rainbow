// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_IMGUI_IMGUIHELPER_H_
#define THIRDPARTY_IMGUI_IMGUIHELPER_H_

#include "ThirdParty/ImGui/imconfig.h"

// clang-format off
#include "ThirdParty/DisableWarnings.h"
#include <imgui/imgui.h>  // NOLINT(llvm-include-order)
#include "ThirdParty/ReenableWarnings.h"
// clang-format on

#include "Input/Pointer.h"
#include "Memory/Array.h"

namespace rainbow
{
    struct KeyStroke;
}  // namespace rainbow

namespace rainbow::graphics
{
    struct Context;
}  // namespace rainbow::graphics

namespace rainbow::imgui
{
    constexpr ImGuiWindowFlags kDefaultWindowFlags =  //
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_MenuBar;

    void init(float font_size, float scale);
    void new_frame(const graphics::Context&, uint64_t dt);
    void render(graphics::Context&, ImDrawData*);
    auto set_key_state(const KeyStroke& key, bool down) -> bool;
    auto set_mouse_state(ArrayView<Pointer>, int surface_height) -> bool;
    auto set_mouse_state(ArrayView<Pointer>, int surface_height, bool down)
        -> bool;
    auto set_mouse_wheel(ArrayView<Pointer> wheels) -> bool;
    void shutdown();
}  // namespace rainbow::imgui

#endif
