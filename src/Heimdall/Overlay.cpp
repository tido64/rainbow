// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/Overlay.h"

#include <numeric>

#include "Graphics/Renderer.h"
#include "ThirdParty/ImGui/ImGuiHelper.h"

using heimdall::Overlay;
using rainbow::KeyStroke;
using rainbow::Rect;
using rainbow::SceneNode;

namespace
{
    constexpr size_t kDataSampleSize = 100;

    template <typename T>
    float at(void* data, int i)
    {
        auto& container = *static_cast<T*>(data);
        return container[i];
    }

    template <typename Container>
    float mean(const Container& container)
    {
        return static_cast<float>(
                   std::accumulate(container.cbegin(),
                                   container.cend(),
                                   typename Container::value_type{})) /
               container.size();
    }

    template <typename... Args>
    void snprintf_q(Args&&... args)
    {
        static_cast<void>(std::snprintf(std::forward<Args>(args)...));
    }
}

Overlay::Overlay()
    : node_(nullptr), frame_times_(kDataSampleSize),
      vmem_usage_(kDataSampleSize), pinned_(false)
{
}

Overlay::~Overlay()
{
    rainbow::imgui::shutdown();
}

void Overlay::initialize(SceneNode& parent)
{
    node_ = parent.add_child(*this);
    node_->set_enabled(false);

    rainbow::imgui::init();
}

void Overlay::draw_impl()
{
    if (!is_enabled())
        return;

    ImGui::Render();
}

void Overlay::update_impl(unsigned long dt)
{
    frame_times_.pop_front();
    frame_times_.push_back(dt);
    vmem_usage_.pop_front();
    vmem_usage_.push_back(TextureManager::Get()->memory_usage().used);

    if (!is_enabled())
        return;

    if (!rainbow::imgui::new_frame(dt) && !pinned_)
    {
        disable();
        return;
    }

    char buffer[128];
    if (ImGui::Begin("", nullptr, rainbow::imgui::kDefaultWindowFlags))
    {
        ImGui::LabelText("", "Rainbow (built " __DATE__ ")");
        ImGui::SameLine(0.0f, 120.0f);
        ImGui::Checkbox(pinned_ ? "Unpin" : "Pin", &pinned_);

        if (ImGui::CollapsingHeader("Performance",
                                    nullptr,
                                    ImGuiTreeNodeFlags_NoAutoOpenOnLog |
                                        ImGuiTreeNodeFlags_DefaultOpen))
        {
            const ImVec2 graph_size(400, 100);

            ImGui::LabelText(
                "", "Draw count: %u", rainbow::graphics::draw_count());

            snprintf_q(buffer,
                       rainbow::array_size(buffer),
                       "Frame time: %.01f ms/frame",
                       mean(frame_times_));
            ImGui::PlotLines("",
                             at<decltype(frame_times_)>,
                             &frame_times_,
                             frame_times_.size(),
                             0,
                             buffer,
                             std::numeric_limits<float>::min(),
                             100.0f,
                             graph_size);

            snprintf_q(buffer,
                       rainbow::array_size(buffer),
                       "Video memory: %.2f MBs",
                       vmem_usage_.back());
            ImGui::PlotLines("",
                             at<decltype(vmem_usage_)>,
                             &vmem_usage_,
                             vmem_usage_.size(),
                             0,
                             buffer,
                             std::numeric_limits<float>::min(),
                             1.0f,
                             graph_size);
        }

        ImGui::End();
    }
}

bool Overlay::on_key_down_impl(const KeyStroke& key)
{
    return is_enabled() && rainbow::imgui::set_key_state(key, true);
}

bool Overlay::on_key_up_impl(const KeyStroke& key)
{
    return is_enabled() && rainbow::imgui::set_key_state(key, false);
}

bool Overlay::on_pointer_began_impl(const ArrayView<Pointer>& p)
{
    return is_enabled() && rainbow::imgui::set_mouse_state(p, true);
}

bool Overlay::on_pointer_canceled_impl()
{
    for (auto&& state : ImGui::GetIO().MouseDown)
        state = false;
    return is_enabled();
}

bool Overlay::on_pointer_ended_impl(const ArrayView<Pointer>& p)
{
    return is_enabled() && rainbow::imgui::set_mouse_state(p, false);
}

bool Overlay::on_pointer_moved_impl(const ArrayView<Pointer>& p)
{
    return is_enabled() && rainbow::imgui::set_mouse_state(p);
}
