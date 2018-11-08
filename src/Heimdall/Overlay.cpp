// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/Overlay.h"

#ifdef USE_HEIMDALL

#include <numeric>

#include "Director.h"
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
    do                                                                         \
    {                                                                          \
        ImGui::Text(#prop " = ");                                              \
        ImGui::SameLine();                                                     \
        ImGui::ColorButton(                                                    \
            "", (source).prop(), ImGuiColorEditFlags_NoLabel, ImVec2(14, 14)); \
        ImGui::SameLine();                                                     \
        ImGui::Text("rgba(%u, %u, %u, %u)",                                    \
                    (source).prop().r,                                         \
                    (source).prop().g,                                         \
                    (source).prop().b,                                         \
                    (source).prop().a);                                        \
    } while (false)
#define IM_TEXT_NUMBER(source, prop) ImGui::Text(#prop " = %f", (source).prop())
#define IM_TEXT_UINT32(source, prop) ImGui::Text(#prop " = %u", (source).prop())
#define IM_TEXT_VEC2(source, prop)                                             \
    ImGui::Text(#prop " = %f,%f", (source).prop().x, (source).prop().y)

using heimdall::Overlay;
using rainbow::Animation;
using rainbow::Color;
using rainbow::czstring;
using rainbow::IDrawable;
using rainbow::KeyStroke;
using rainbow::Label;
using rainbow::Pointer;
using rainbow::Sprite;
using rainbow::SpriteBatch;
using rainbow::Vec2i;

namespace graphics = rainbow::graphics;

namespace
{
    constexpr uint64_t kStartUpMessageMaxDuration = 8000;
    constexpr float kStyleFontSize = 13.0f;
    constexpr float kStylePlotHeight = 100.0f;
#if defined(RAINBOW_OS_ANDROID) || defined(RAINBOW_OS_IOS)
    constexpr float kStyleScreenHeight = 600.0f;
#else
    constexpr float kStyleScreenHeight = 1200.0f;
#endif
    constexpr float kStyleWindowWidth = 400.0f;
    constexpr float kStyleWindowHeight = 400.0f;

    template <typename T>
    void print_address(const T* obj)
    {
        ImGui::Text("address = %p", static_cast<const void*>(obj));
    }

    template <typename T>
    auto at(void* data, int i) -> float
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

    auto display_scale(Vec2i resolution, float screen_height)
    {
        return std::max(
            std::min(resolution.x, resolution.y) / screen_height, 1.0f);
    }

    template <typename Container>
    auto mean(const Container& container)
    {
        return std::accumulate(std::begin(container),
                               std::end(container),
                               typename Container::value_type{}) /
               static_cast<float>(container.size());
    }

    template <typename... Args>
    void snprintf_q(Args&&... args)
    {
        static_cast<void>(std::snprintf(std::forward<Args>(args)...));
    }

    template <typename Container>
    auto upper_limit(const Container& container)
    {
        return rainbow::ceil_pow2(static_cast<unsigned int>(std::ceil(
            *std::max_element(std::begin(container), std::end(container)))));
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
                IM_TEXT_COLOR(*label, color);
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
}  // namespace

Overlay::~Overlay()
{
    if (ImGui::GetCurrentContext() == nullptr)
        return;

    rainbow::imgui::shutdown();
}

void Overlay::initialize(Vec2i resolution)
{
    const float scale = display_scale(resolution, kStyleScreenHeight);
    rainbow::imgui::init(kStyleFontSize, scale);
}

auto Overlay::surface_height() const
{
    return director_.graphics_context().surface_size.y;
}

void Overlay::draw_menu_bar()
{
    if (!ImGui::BeginMenuBar())
        return;

    if (ImGui::BeginMenu("Debug"))
    {
        if (ImGui::MenuItem("Restart"))
            director_.restart();
        ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
}

void Overlay::draw_performance(float scale)
{
    if (!ImGui::CollapsingHeader("Performance",
                                 ImGuiTreeNodeFlags_NoAutoOpenOnLog |
                                     ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    const ImVec2 graph_size{
        kStyleWindowWidth * scale, kStylePlotHeight * scale};

    ImGui::TextWrapped("Draw count: %u", graphics::draw_count());

    char buffer[128];
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
                     upper_limit(vmem_usage_),
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

void Overlay::draw_render_queue()
{
    constexpr ImGuiTreeNodeFlags kCollapsingHeaderFlags =
        ImGuiTreeNodeFlags_NoTreePushOnOpen |
        ImGuiTreeNodeFlags_NoAutoOpenOnLog |
        ImGuiTreeNodeFlags_CollapsingHeader;

    auto& render_queue = director_.render_queue();
    if (!ImGui::TreeNodeEx(&render_queue,
                           kCollapsingHeaderFlags,
                           "Render Queue (%zu unit%s)",
                           render_queue.size(),
                           render_queue.size() == 1 ? "" : "s"))
    {
        return;
    }

    for (auto&& unit : render_queue)
        rainbow::visit(CreateNode{unit.tag().c_str()}, unit.object());
}

void Overlay::draw_startup_message()
{
    constexpr ImGuiWindowFlags kMinimalWindowFlags =  //
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav;

    ImGui::SetNextWindowPos({4.0f, 4.0f}, ImGuiCond_Once);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    if (ImGui::Begin("", nullptr, kMinimalWindowFlags))
    {
        const ImVec4 yellow = Color{0xff, 0xeb, 0x3b};
        ImGui::TextColored(yellow, RAINBOW_SHORTCUT_DIAGNOSTIC_TOOLS ".");
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void Overlay::draw_impl()
{
    auto draw_data = ImGui::GetDrawData();
    if (draw_data == nullptr || !draw_data->Valid)
        return;

    rainbow::imgui::render(director_.graphics_context(), draw_data);
}

void Overlay::update_impl(uint64_t dt)
{
    static uint64_t startup_message_duration = 0;

    frame_times_.pop_front();
    frame_times_.push_back(dt);
    vmem_usage_.pop_front();
    vmem_usage_.push_back(director_.texture_manager().memory_usage().used);

    if (!is_enabled())
    {
        if (startup_message_duration < kStartUpMessageMaxDuration)
        {
            startup_message_duration += dt;

            rainbow::imgui::new_frame(director_.graphics_context(), dt);
            draw_startup_message();
            ImGui::Render();
        }
        return;
    }

    startup_message_duration = kStartUpMessageMaxDuration;

    rainbow::imgui::new_frame(director_.graphics_context(), dt);
    if (ImGui::Begin("Rainbow (built " __DATE__ ")",
                     &enabled_,
                     rainbow::imgui::kDefaultWindowFlags))
    {
        draw_menu_bar();

        const float scale =
            ImGui::GetIO().Fonts->Fonts[0]->FontSize / kStyleFontSize;
        const ImVec2 window_size{
            kStyleWindowWidth * scale, kStyleWindowHeight * scale};
        if (ImGui::BeginChild("Body", window_size))
        {
            draw_performance(scale);
            draw_render_queue();
        }
        ImGui::EndChild();
    }

    ImGui::End();
    ImGui::Render();
}

bool Overlay::on_key_down_impl(const KeyStroke& key)
{
    return is_enabled() && rainbow::imgui::set_key_state(key, true);
}

bool Overlay::on_key_up_impl(const KeyStroke& key)
{
    return is_enabled() && rainbow::imgui::set_key_state(key, false);
}

bool Overlay::on_mouse_wheel_impl(const ArrayView<Pointer>& w)
{
    return is_enabled() && rainbow::imgui::set_mouse_wheel(w);
}

bool Overlay::on_pointer_began_impl(const ArrayView<Pointer>& p)
{
    return is_enabled() &&
           rainbow::imgui::set_mouse_state(p, surface_height(), true);
}

bool Overlay::on_pointer_canceled_impl()
{
    auto& state = ImGui::GetIO().MouseDown;
    std::fill(std::begin(state), std::end(state), false);
    return is_enabled();
}

bool Overlay::on_pointer_ended_impl(const ArrayView<Pointer>& p)
{
    return is_enabled() &&
           rainbow::imgui::set_mouse_state(p, surface_height(), false);
}

bool Overlay::on_pointer_moved_impl(const ArrayView<Pointer>& p)
{
    return is_enabled() && rainbow::imgui::set_mouse_state(p, surface_height());
}

#endif  // USE_HEIMDALL
