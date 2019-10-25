// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/ImGui/ImGuiHelper.h"

#include <limits>

// clang-format off
#include "ThirdParty/DisableWarnings.h"
#define IMGUI_DISABLE_OSX_FUNCTIONS 1
#include <imgui/imgui.cpp>  // NOLINT(llvm-include-order)
#include <imgui/imgui_draw.cpp>
#include <imgui/imgui_widgets.cpp>
#include "ThirdParty/ReenableWarnings.h"
// clang-format on

#include "Graphics/Buffer.h"
#include "Graphics/ElementBuffer.h"
#include "Graphics/Image.h"
#include "Graphics/Renderer.h"
#include "Graphics/VertexArray.h"
#include "Input/VirtualKey.h"
#include "Text/SystemFonts.h"

using rainbow::Image;
using rainbow::Vec2f;
using rainbow::graphics::Buffer;
using rainbow::graphics::Context;
using rainbow::graphics::ElementBuffer;
using rainbow::graphics::Filter;
using rainbow::graphics::ScopedProjection;
using rainbow::graphics::ScopedScissorTest;
using rainbow::graphics::Texture;
using rainbow::graphics::VertexArray;

namespace
{
    class RenderData
    {
    public:
        [[nodiscard]] auto element_buffer() -> ElementBuffer&
        {
            return element_buffer_;
        }

        [[nodiscard]] auto initial_window_width() const
        {
            return initial_window_width_;
        }

        [[nodiscard]] auto texture() -> Texture& { return texture_; }

        [[nodiscard]] auto vertex_array() -> VertexArray&
        {
            return vertex_array_;
        }

        [[nodiscard]] auto vertex_buffer() -> Buffer& { return vertex_buffer_; }

        [[nodiscard]] auto window_scale() const { return window_scale_; }

        void initial_window_width(float width)
        {
            initial_window_width_ = width;
        }

        void window_scale(Vec2f scale) { window_scale_ = scale; }

        void set_draw_list(const ImDrawList* list)
        {
            element_buffer_.upload(  //
                &list->IdxBuffer.front(),
                list->IdxBuffer.size() * sizeof(ImDrawIdx));
            vertex_buffer_.upload(  //
                &list->VtxBuffer.front(),
                list->VtxBuffer.size() * sizeof(ImDrawVert));
            vertex_buffer_.bind();
        }

    private:
        Vec2f window_scale_;
        float initial_window_width_ = 0.0F;
        ElementBuffer element_buffer_;
        Buffer vertex_buffer_;
        VertexArray vertex_array_;
        Texture texture_;
    };

    auto get_render_data(ImGuiIO& io) -> RenderData&
    {
        return *static_cast<RenderData*>(io.UserData);
    }
}  // namespace

void rainbow::imgui::init(Context& ctx, float font_size, float scale)
{
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
#ifdef RAINBOW_SDL
    io.KeyMap[ImGuiKey_Tab] = to_keycode(VirtualKey::Tab);
    io.KeyMap[ImGuiKey_LeftArrow] = to_scancode(VirtualKey::Left);
    io.KeyMap[ImGuiKey_RightArrow] = to_scancode(VirtualKey::Right);
    io.KeyMap[ImGuiKey_UpArrow] = to_scancode(VirtualKey::Up);
    io.KeyMap[ImGuiKey_DownArrow] = to_scancode(VirtualKey::Down);
    io.KeyMap[ImGuiKey_PageUp] = to_scancode(VirtualKey::PageUp);
    io.KeyMap[ImGuiKey_PageDown] = to_scancode(VirtualKey::PageDown);
    io.KeyMap[ImGuiKey_Home] = to_scancode(VirtualKey::Home);
    io.KeyMap[ImGuiKey_End] = to_scancode(VirtualKey::End);
    io.KeyMap[ImGuiKey_Delete] = to_keycode(VirtualKey::Delete);
    io.KeyMap[ImGuiKey_Backspace] = to_keycode(VirtualKey::Backspace);
    io.KeyMap[ImGuiKey_Enter] = to_keycode(VirtualKey::Return);
    io.KeyMap[ImGuiKey_Escape] = to_keycode(VirtualKey::Escape);
    io.KeyMap[ImGuiKey_A] = to_keycode(VirtualKey::A);
    io.KeyMap[ImGuiKey_C] = to_keycode(VirtualKey::C);
    io.KeyMap[ImGuiKey_V] = to_keycode(VirtualKey::V);
    io.KeyMap[ImGuiKey_X] = to_keycode(VirtualKey::X);
    io.KeyMap[ImGuiKey_Y] = to_keycode(VirtualKey::Y);
    io.KeyMap[ImGuiKey_Z] = to_keycode(VirtualKey::Z);
#endif  // RAINBOW_SDL

    const float scaled_font_size = font_size * scale;
    if (text::monospace_font_path() == nullptr)
    {
        ImFontConfig font_config;
        font_config.SizePixels = scaled_font_size;
        io.Fonts->AddFontDefault(&font_config);
    }
    else
    {
        constexpr ImWchar kCommandKey = 0x2318;
        constexpr ImWchar kOptionKey = 0x2325;
        static constexpr std::array<ImWchar, 7> kGlyphRanges{
            0x0020,
            0x00ff,
            kCommandKey,
            kCommandKey,
            kOptionKey,
            kOptionKey,
            0,
        };
        io.Fonts->AddFontFromFileTTF(text::monospace_font_path(),
                                     scaled_font_size,
                                     nullptr,
                                     kGlyphRanges.data());
    }

    auto render_data = std::make_unique<RenderData>().release();
    io.UserData = render_data;

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    render_data->element_buffer() = buffer;
    render_data->vertex_array().reconfigure([] {
        auto& render_data = get_render_data(ImGui::GetIO());
        render_data.element_buffer().bind();
        render_data.vertex_buffer().bind();
    });

    unsigned char* pixels;
    int width, height, bytes_per_pixel;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

    render_data->texture() = ctx.texture_provider.get(  //
        "rainbow://dear_imgui/monospace",
        Image{Image::Format::RGBA,
              narrow_cast<uint32_t>(width),
              narrow_cast<uint32_t>(height),
              narrow_cast<uint32_t>(bytes_per_pixel),
              0,
              narrow_cast<size_t>(width * height * bytes_per_pixel),
              pixels},
        Filter::Nearest,
        Filter::Nearest);

    io.Fonts->TexID = static_cast<void*>(&render_data->texture());
}

void rainbow::imgui::new_frame(const Context& ctx, uint64_t dt)
{
    auto& io = ImGui::GetIO();
    auto& render_data = get_render_data(io);
    if (render_data.window_scale().x == 0.0F)
    {
        render_data.initial_window_width(ctx.window_size.x);
        render_data.window_scale(
            {narrow_cast<float>(ctx.surface_size.x) / ctx.window_size.x,
             narrow_cast<float>(ctx.surface_size.y) / ctx.window_size.y});
        io.DisplaySize = ctx.window_size;
    }

    io.DeltaTime = std::max(dt * 0.001F, std::numeric_limits<float>::epsilon());
    io.DisplayFramebufferScale = render_data.window_scale() *
                                 ctx.window_size.x /
                                 render_data.initial_window_width();

    ImGui::NewFrame();
}

void rainbow::imgui::render(Context& ctx, ImDrawData* draw_data)
{
    auto& io = ImGui::GetIO();
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    ScopedProjection projection(
        ctx, {0.0F, io.DisplaySize.y, io.DisplaySize.x, -io.DisplaySize.y});
    ScopedScissorTest scissor_test;

    const auto window_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
    auto& render_data = get_render_data(io);
    render_data.vertex_array().bind();
    for (int i = 0; i < draw_data->CmdListsCount; ++i)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[i];
        const ImDrawIdx* idx_buffer_offset = nullptr;

        render_data.set_draw_list(cmd_list);
        for (auto&& cmd : cmd_list->CmdBuffer)
        {
            if (cmd.UserCallback != nullptr)
            {
                cmd.UserCallback(cmd_list, &cmd);
            }
            else
            {
                graphics::bind(ctx, *static_cast<Texture*>(cmd.TextureId));
                graphics::scissor(
                    ctx,
                    truncate<int>(cmd.ClipRect.x),
                    truncate<int>(window_height - cmd.ClipRect.w),
                    truncate<int>(cmd.ClipRect.z - cmd.ClipRect.x),
                    truncate<int>(cmd.ClipRect.w - cmd.ClipRect.y));
                glDrawElements(  //
                    GL_TRIANGLES,
                    static_cast<GLsizei>(cmd.ElemCount),
                    sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT
                                           : GL_UNSIGNED_INT,
                    idx_buffer_offset);
            }

            idx_buffer_offset += cmd.ElemCount;
        }
    }

    draw_data->Clear();
    graphics::bind_element_array();
}

auto rainbow::imgui::set_key_state([[maybe_unused]] const KeyStroke& key,
                                   [[maybe_unused]] bool down) -> bool
{
    auto& io = ImGui::GetIO();
#ifdef RAINBOW_SDL
    switch (key.key)
    {
        case VirtualKey::Home:
        case VirtualKey::PageUp:
        case VirtualKey::End:
        case VirtualKey::PageDown:
        case VirtualKey::Right:
        case VirtualKey::Left:
        case VirtualKey::Down:
        case VirtualKey::Up:
            io.KeysDown[to_scancode(key.key)] = down;
            break;
        case VirtualKey::LeftCtrl:
            io.KeyCtrl = down;
            break;
        case VirtualKey::LeftShift:
            io.KeyShift = down;
            break;
        case VirtualKey::LeftAlt:
            io.KeyAlt = down;
            break;
        case VirtualKey::RightCtrl:
            io.KeyCtrl = down;
            break;
        case VirtualKey::RightShift:
            io.KeyShift = down;
            break;
        case VirtualKey::RightAlt:
            io.KeyAlt = down;
            break;
        default:
        {
            const int keycode = to_keycode(key.key);
            if (static_cast<unsigned>(keycode) < array_size(io.KeysDown))
            {
                io.KeysDown[keycode] = down;

                // TODO: Implement SDL_TEXTINPUT
                if (down)
                    io.AddInputCharacter(keycode);
            }
            break;
        }
    }
#endif  // RAINBOW_SDL
    return io.WantCaptureKeyboard;
}

auto rainbow::imgui::set_mouse_state(ArrayView<Pointer> p, int surface_height)
    -> bool
{
    auto& io = ImGui::GetIO();
    auto& pos = io.MousePos;
    auto window_scale = get_render_data(io).window_scale();
    pos.x = p[0].x / window_scale.x;
    pos.y = (surface_height - p[0].y) / window_scale.y;
    return io.WantCaptureMouse;
}

auto rainbow::imgui::set_mouse_state(ArrayView<Pointer> p,
                                     int surface_height,
                                     bool down) -> bool
{
    // Map SDL_BUTTON -> ImGuiIO::MouseDown.
    static constexpr std::array<int, 5> kMouseButtons{0, 2, 1, 3, 4};

    // Always update mouse position as we don't know whether an actual mouse or
    // touch pad is attached.
    set_mouse_state(p, surface_height);

    auto& io = ImGui::GetIO();
    const auto hash =
#ifndef RAINBOW_SDL
        // Map number of fingers to SDL_BUTTON.
        kMouseButtons[std::min<size_t>(p.size() - 1, 4)] + 1;
#else
        p[0].hash;
#endif
    const int index = kMouseButtons[hash - 1];
    io.MouseDown[index] = down;
    return io.WantCaptureMouse;
}

auto rainbow::imgui::set_mouse_wheel(ArrayView<Pointer> wheels) -> bool
{
    // 1 unit scrolls about 5 lines text. Adjust to make it 1:1.
    auto& io = ImGui::GetIO();
    io.MouseWheel = wheels[0].y * 0.2F;
    io.MouseWheelH = wheels[0].x * 0.2F;
    return io.WantCaptureMouse;
}

void rainbow::imgui::shutdown()
{
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete &get_render_data(ImGui::GetIO());
    ImGui::DestroyContext();
}
