// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/Overlay.h"

#ifdef USE_HEIMDALL

#include <numeric>

#include "Common/TypeCast.h"
#include "Director.h"
#include "Graphics/Animation.h"
#include "Graphics/Label.h"
#include "Graphics/Renderer.h"
#include "Graphics/SpriteBatch.h"
#include "ThirdParty/ImGui/ImGuiHelper.h"

#define BRIEF(type, properties) "(" #type ") " properties
#define ONLY_TAG(type) BRIEF(type, "tag=\"%s\"")
#define WITH_TAG(type, properties) BRIEF(type, properties " tag=\"%s\"")
#define WRITE_PROP(obj, prop) write_prop(#prop, (obj).prop())

using heimdall::Overlay;
using rainbow::Animation;
using rainbow::Color;
using rainbow::czstring;
using rainbow::KeyStroke;
using rainbow::Label;
using rainbow::Pointer;
using rainbow::Sprite;
using rainbow::SpriteBatch;
using rainbow::Vec2f;
using rainbow::Vec2i;
using rainbow::graphics::Texture;

namespace
{
    constexpr uint64_t kStartUpMessageMaxDuration = 8000;
    constexpr float kStyleFontSize = 13.0F;
    constexpr float kStylePlotHeight = 100.0F;
#if defined(RAINBOW_OS_ANDROID) || defined(RAINBOW_OS_IOS)
    constexpr float kStyleScreenHeight = 600.0F;
#else
    constexpr float kStyleScreenHeight = 1200.0F;
#endif
    constexpr float kStyleWindowWidth = 400.0F;
    constexpr float kStyleWindowHeight = 400.0F;

    void write_address(const void* obj)
    {
        ImGui::Text("address = %p", obj);
    }

    void write_prop(std::string_view property, std::string_view value)
    {
        ImGui::Text("%s = %s", property.data(), value.data());
    }

    void write_prop(std::string_view property, bool value)
    {
        using namespace std::literals::string_view_literals;
        write_prop(property, value ? "true"sv : "false"sv);
    }

    void write_prop(std::string_view property, int value)
    {
        ImGui::Text("%s = %i", property.data(), value);
    }

    void write_prop(std::string_view property, uint32_t value)
    {
        ImGui::Text("%s = %u", property.data(), value);
    }

    void write_prop(std::string_view property, float value)
    {
        ImGui::Text("%s = %f", property.data(), value);
    }

    void write_prop(std::string_view property, Color c)
    {
        ImGui::Text("%s = ", property.data());
        ImGui::SameLine();
        ImGui::ColorButton("", c, ImGuiColorEditFlags_NoLabel, {14.0F, 14.0F});
        ImGui::SameLine();
        ImGui::Text("rgba(%u, %u, %u, %u)", c.r, c.g, c.b, c.a);
    }

    void write_prop(std::string_view property, Vec2f v)
    {
        ImGui::Text("%s = %f,%f", property.data(), v.x, v.y);
    }

    void write_prop(std::string_view property, const Texture* texture)
    {
        if (texture == nullptr)
            ImGui::Text("%s = nullptr", property.data());
        else
            write_prop(property, texture->key());
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
            write_address(&sprite);
            WRITE_PROP(sprite, position);
            WRITE_PROP(sprite, color);
            WRITE_PROP(sprite, scale);
            WRITE_PROP(sprite, angle);
            WRITE_PROP(sprite, pivot);
            WRITE_PROP(sprite, is_flipped);
            WRITE_PROP(sprite, is_hidden);
            WRITE_PROP(sprite, is_mirrored);
            ImGui::TreePop();
        }
    }

    auto display_scale(Vec2i resolution, float screen_height)
    {
        return std::max(
            std::min(resolution.x, resolution.y) / screen_height, 1.0F);
    }

    template <typename Container>
    auto mean(const Container& container)
    {
        return std::accumulate(std::begin(container),
                               std::end(container),
                               typename Container::value_type{}) /
               rainbow::narrow_cast<float>(container.size());
    }

    template <typename... Args>
    void snprintf_q(Args&&... args)
    {
        static_cast<void>(std::snprintf(std::forward<Args>(args)...));
    }

    template <typename Container>
    auto upper_limit(const Container& container)
    {
        return rainbow::ceil_pow2(rainbow::truncate<unsigned int>(std::ceil(
            *std::max_element(std::begin(container), std::end(container)))));
    }

    struct CreateNode
    {
        czstring tag;  // NOLINT

        void operator()(Animation* animation) const
        {
            if (ImGui::TreeNode(animation,
                                WITH_TAG(Animation, "%s frame=%u"),
                                animation->is_stopped() ? "stopped" : "playing",
                                animation->current_frame(),
                                tag))
            {
                write_address(animation);
                WRITE_PROP(*animation, frame_rate);
                create_node(*animation->sprite());
                ImGui::TreePop();
            }
        }

        void operator()(Label* label) const
        {
            if (ImGui::TreeNode(
                    label, WITH_TAG(Label, "%s"), label->text(), tag))
            {
                write_address(label);
                WRITE_PROP(*label, position);
                WRITE_PROP(*label, font);
                WRITE_PROP(*label, font_size);
                WRITE_PROP(*label, color);
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
                write_address(batch);
                WRITE_PROP(*batch, texture);
                WRITE_PROP(*batch, normal);
                WRITE_PROP(*batch, is_visible);

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
                write_address(drawable);
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
    rainbow::imgui::init(director_.graphics_context(), kStyleFontSize, scale);
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
    namespace graphics = rainbow::graphics;

    if (!ImGui::CollapsingHeader("Performance",
                                 ImGuiTreeNodeFlags_NoAutoOpenOnLog |
                                     ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    const ImVec2 graph_size{
        kStyleWindowWidth * scale, kStylePlotHeight * scale};

    ImGui::TextWrapped("Draw count: %u", graphics::draw_count());

    std::array<char, 128> buffer;
    snprintf_q(  //
        buffer.data(),
        buffer.size(),
        "Frame time: %.01f ms/frame",
        mean(frame_times_));
    ImGui::PlotLines(  //
        "",
        at<decltype(frame_times_)>,
        &frame_times_,
        rainbow::narrow_cast<int>(frame_times_.size()),
        0,
        buffer.data(),
        std::numeric_limits<float>::min(),
        100.0F,
        graph_size);

    snprintf_q(  //
        buffer.data(),
        buffer.size(),
        "Video memory: %.2f MBs",
        vmem_usage_.back());
    ImGui::PlotLines(  //
        "",
        at<decltype(vmem_usage_)>,
        &vmem_usage_,
        rainbow::narrow_cast<int>(vmem_usage_.size()),
        0,
        buffer.data(),
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
        rainbow::visit(CreateNode{unit.tag().data()}, unit.object());
}

void Overlay::draw_startup_message()
{
    constexpr ImGuiWindowFlags kMinimalWindowFlags =  //
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav;

    ImGui::SetNextWindowPos({4.0F, 4.0F}, ImGuiCond_Once);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);

    if (ImGui::Begin("Help", nullptr, kMinimalWindowFlags))
    {
        const ImVec4 yellow = Color{0xff, 0xeb, 0x3b};
        ImGui::TextColored(yellow, RAINBOW_SHORTCUT_DIAGNOSTIC_TOOLS ".");
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void Overlay::draw_impl() const
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

    auto [used, peak] = director_.texture_provider().memory_usage();
    vmem_usage_.pop_front();
    vmem_usage_.push_back(used * 1e-6);

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

auto Overlay::on_key_down_impl(const KeyStroke& key) -> bool
{
    return is_enabled() && rainbow::imgui::set_key_state(key, true);
}

auto Overlay::on_key_up_impl(const KeyStroke& key) -> bool
{
    return is_enabled() && rainbow::imgui::set_key_state(key, false);
}

auto Overlay::on_mouse_wheel_impl(const ArrayView<Pointer>& w) -> bool
{
    return is_enabled() && rainbow::imgui::set_mouse_wheel(w);
}

auto Overlay::on_pointer_began_impl(const ArrayView<Pointer>& p) -> bool
{
    return is_enabled() &&
           rainbow::imgui::set_mouse_state(p, surface_height(), true);
}

auto Overlay::on_pointer_canceled_impl() -> bool
{
    auto& state = ImGui::GetIO().MouseDown;
    std::fill(std::begin(state), std::end(state), false);
    return is_enabled();
}

auto Overlay::on_pointer_ended_impl(const ArrayView<Pointer>& p) -> bool
{
    return is_enabled() &&
           rainbow::imgui::set_mouse_state(p, surface_height(), false);
}

auto Overlay::on_pointer_moved_impl(const ArrayView<Pointer>& p) -> bool
{
    return is_enabled() && rainbow::imgui::set_mouse_state(p, surface_height());
}

#endif  // USE_HEIMDALL
