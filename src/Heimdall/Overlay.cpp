// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/Overlay.h"

#include <numeric>

#include "Graphics/Animation.h"
#include "Graphics/Label.h"
#include "Graphics/Renderer.h"
#include "Graphics/SpriteBatch.h"
#include "ThirdParty/ImGui/ImGuiHelper.h"

#define BRIEF(type, properties) "(" #type ") " properties
#define ONLY_TAG(type) BRIEF(type, "tag=\"%s\"")
#define WITH_TAG(type, properties) BRIEF(type, properties " tag=\"%s\"")

#define IM_TEXT_BOOL(source, prop)                                             \
    ImGui::Text(#prop " = %s", (source).prop() ? "true" : "false")
#define IM_TEXT_COLOR(source, prop)                                            \
    ImGui::Text(#prop " = rgba(%u, %u, %u, %u)",                               \
                (source).prop().r,                                             \
                (source).prop().g,                                             \
                (source).prop().b,                                             \
                (source).prop().a)
#define IM_TEXT_NUMBER(source, prop) ImGui::Text(#prop " = %f", (source).prop())
#define IM_TEXT_UINT32(source, prop) ImGui::Text(#prop " = %u", (source).prop())
#define IM_TEXT_VEC2(source, prop)                                             \
    ImGui::Text(#prop " = %f,%f", (source).prop().x, (source).prop().y)

using heimdall::Overlay;
using rainbow::Animation;
using rainbow::IDrawable;
using rainbow::KeyStroke;
using rainbow::Label;
using rainbow::Pointer;
using rainbow::Sprite;
using rainbow::SpriteBatch;
using rainbow::czstring;
using rainbow::graphics::TextureManager;

namespace graphics = rainbow::graphics;

namespace
{
    template <typename T>
    void print_address(const T* obj)
    {
        ImGui::Text("address = %p", static_cast<const void*>(obj));
    }

    template <typename T>
    float at(void* data, int i)
    {
        auto& container = *static_cast<T*>(data);
        return container[i];
    }

    void create_node(const Sprite& sprite)
    {
        if (ImGui::TreeNode(&sprite,
                            BRIEF(Sprite, "id=%i width=%u height=%u"),
                            sprite.id(),
                            sprite.width(),
                            sprite.height()))
        {
            print_address(&sprite);
            IM_TEXT_VEC2(sprite, position);
            IM_TEXT_COLOR(sprite, color);
            IM_TEXT_VEC2(sprite, scale);
            IM_TEXT_NUMBER(sprite, angle);
            IM_TEXT_VEC2(sprite, pivot);
            IM_TEXT_BOOL(sprite, is_flipped);
            IM_TEXT_BOOL(sprite, is_hidden);
            IM_TEXT_BOOL(sprite, is_mirrored);
            ImGui::TreePop();
        }
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

    struct CreateNode
    {
        czstring tag;

        void operator()(Animation* animation) const
        {
            if (ImGui::TreeNode(animation,
                                WITH_TAG(Animation, "%s frame=%u"),
                                animation->is_stopped() ? "stopped" : "playing",
                                animation->current_frame(),
                                tag))
            {
                print_address(animation);
                IM_TEXT_UINT32(*animation, frame_rate);
                create_node(*animation->sprite());
                ImGui::TreePop();
            }
        }

        void operator()(Label* label) const
        {
            if (ImGui::TreeNode(
                    label, WITH_TAG(Label, "%s"), label->text(), tag))
            {
                print_address(label);
                IM_TEXT_VEC2(*label, position);
                ImGui::TreePop();
            }
        }

        void operator()(SpriteBatch* batch) const
        {
            if (ImGui::TreeNode(batch,
                                WITH_TAG(SpriteBatch, "size=%u"),
                                batch->size(),
                                tag))
            {
                print_address(batch);
                IM_TEXT_BOOL(*batch, is_visible);

                for (auto&& sprite : *batch)
                    create_node(sprite);

                ImGui::TreePop();
            }
        }

        template <typename T>
        void operator()(T&& drawable) const
        {
            if (ImGui::TreeNode(drawable, ONLY_TAG(IDrawable), tag))
            {
                print_address(drawable);
                ImGui::TreePop();
            }
        }
    };
}

Overlay::~Overlay()
{
    rainbow::imgui::shutdown();
}

void Overlay::initialize()
{
    rainbow::imgui::init();
}

void Overlay::draw_impl()
{
    if (!is_enabled())
        return;

    ImGui::Render();
}

void Overlay::update_impl(uint64_t dt)
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

            ImGui::TextWrapped("Draw count: %u", graphics::draw_count());

            snprintf_q(buffer,
                       rainbow::array_size(buffer),
                       "Frame time: %.01f ms/frame",
                       mean(frame_times_));
            ImGui::PlotLines("",
                             at<decltype(frame_times_)>,
                             &frame_times_,
                             static_cast<int>(frame_times_.size()),
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
                             static_cast<int>(vmem_usage_.size()),
                             0,
                             buffer,
                             std::numeric_limits<float>::min(),
                             1.0f,
                             graph_size);

            ImGui::TextWrapped("OpenGL %s", graphics::gl_version());
            ImGui::TextWrapped("Vendor: %s", graphics::vendor());
            ImGui::TextWrapped("Renderer: %s", graphics::renderer());
            const auto meminfo = graphics::memory_info();
            if (meminfo.current_available > 0)
            {
                if (meminfo.total_available > 0)
                {
                    ImGui::TextWrapped(  //
                        "Video memory: %i / %i kB free",
                        meminfo.current_available,
                        meminfo.total_available);
                }
                else
                {
                    ImGui::TextWrapped(
                        "Video memory: %i kB free", meminfo.current_available);
                }
            }
        }

        if (ImGui::CollapsingHeader(
                "Render Queue", nullptr, ImGuiTreeNodeFlags_NoAutoOpenOnLog))
        {
            for (auto&& unit : render_queue_)
                rainbow::visit(CreateNode{unit.tag().c_str()}, unit.object());
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
