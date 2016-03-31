// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/ImGui/ImGuiHelper.h"

#ifdef __GNUC__
    // TODO: This is a workaround for 'diagnostic push' bug in GCC. See
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53431 for details.
#   ifdef __clang__
#       pragma GCC diagnostic push
#   endif
#   pragma GCC diagnostic ignored "-Wformat"
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#   pragma GCC diagnostic ignored "-Wtype-limits"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <imgui/imgui.cpp>
#include <imgui/imgui_draw.cpp>
#ifdef __clang__
#   pragma GCC diagnostic pop
#endif

#include "Graphics/Buffer.h"
#include "Graphics/ElementBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/VertexArray.h"
#include "Input/VirtualKey.h"

using rainbow::graphics::Buffer;
using rainbow::graphics::ElementBuffer;
using rainbow::graphics::ScopedProjection;
using rainbow::graphics::ScopedScissorTest;
using rainbow::graphics::TextureFilter;
using rainbow::graphics::VertexArray;
using rainbow::Texture;

namespace
{
    float g_initial_window_width = 0.0f;
    Vec2f g_window_scale;

    class Renderable
    {
    public:
        auto& element_buffer() { return element_buffer_; }
        auto& texture() { return texture_; }
        auto& vertex_array() { return vertex_array_; }
        auto& vertex_buffer() { return vertex_buffer_; }

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
        ElementBuffer element_buffer_;
        Buffer vertex_buffer_;
        VertexArray vertex_array_;
        Texture texture_;
    };

    void render_draw_lists(ImDrawData* draw_data)
    {
        auto& io = ImGui::GetIO();
        draw_data->ScaleClipRects(io.DisplayFramebufferScale);

        ScopedProjection projection(
            {0.0f, io.DisplaySize.y, io.DisplaySize.x, 0.0f});
        ScopedScissorTest scissor_test;

        const int window_height =
            static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);
        auto render = static_cast<Renderable*>(io.UserData);
        render->vertex_array().bind();
        for (int i = 0; i < draw_data->CmdListsCount; ++i)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[i];
            const ImDrawIdx* idx_buffer_offset = nullptr;

            render->set_draw_list(cmd_list);
            for (auto&& cmd : cmd_list->CmdBuffer)
            {
                if (cmd.UserCallback)
                {
                    cmd.UserCallback(cmd_list, &cmd);
                }
                else
                {
                    TextureManager::Get()->bind(static_cast<GLuint>(
                        reinterpret_cast<intptr_t>(cmd.TextureId)));
                    rainbow::graphics::scissor(
                        static_cast<int>(cmd.ClipRect.x),
                        static_cast<int>(window_height - cmd.ClipRect.w),
                        static_cast<int>(cmd.ClipRect.z - cmd.ClipRect.x),
                        static_cast<int>(cmd.ClipRect.w - cmd.ClipRect.y));
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

        rainbow::graphics::bind_element_array();
    }
}

namespace rainbow { namespace imgui
{
    void init()
    {
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
        io.RenderDrawListsFn = &render_draw_lists;

        unsigned int buffer;
        glGenBuffers(1, &buffer);
        auto renderable = std::make_unique<Renderable>().release();
        renderable->element_buffer() = buffer;
        renderable->vertex_array().reconfigure([&renderable] {
            renderable->element_buffer().bind();
            renderable->vertex_buffer().bind();
        });
        io.UserData = renderable;

        auto texture_manager = TextureManager::Get();
        const TextureFilter filter = texture_manager->mag_filter();
        texture_manager->set_filter(TextureFilter::Nearest);
        renderable->texture() = texture_manager->create(
            "rainbow://dear_imgui/ProggyClean.ttf",
            [&io](TextureManager& texture_manager, const Texture& texture) {
                unsigned char* pixels;
                int width, height;
                io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
                texture_manager.upload(
                    texture, GL_RGBA8, width, height, GL_RGBA, pixels);
            });
        texture_manager->set_filter(filter);

        io.Fonts->TexID = reinterpret_cast<void*>(
            static_cast<intptr_t>(renderable->texture()));
    }

    bool new_frame(unsigned long dt)
    {
        auto& window_size = graphics::window_size();
        auto& io = ImGui::GetIO();
        if (g_window_scale.x == 0.0f)
        {
            g_initial_window_width = window_size.x;
            auto& resolution = graphics::resolution();
            g_window_scale =
                Vec2f(static_cast<float>(resolution.x) / window_size.x,
                      static_cast<float>(resolution.y) / window_size.y);
            io.DisplaySize = window_size;
        }

        io.DeltaTime = dt * 0.001f;
        io.DisplayFramebufferScale =
            g_window_scale * window_size.x / g_initial_window_width;

        ImGui::NewFrame();

        return io.WantCaptureMouse ||
               std::none_of(std::cbegin(io.MouseDown),
                            std::cend(io.MouseDown),
                            [](bool state) { return state; });
    }

    bool set_key_state(const KeyStroke& key, bool down)
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
            default: {
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
#else
        static_cast<void>(key);
        static_cast<void>(down);
#endif  // RAINBOW_SDL
        return io.WantCaptureKeyboard;
    }

    bool set_mouse_state(const ArrayView<Pointer>& p)
    {
        auto& io = ImGui::GetIO();
        auto& pos = io.MousePos;
        pos.x = p[0].x / g_window_scale.x;
        pos.y = (graphics::resolution().y - p[0].y) / g_window_scale.y;
        return false;
    }

    bool set_mouse_state(const ArrayView<Pointer>& pointers, bool down)
    {
        // For details, see SDL_mouse.h.
        static constexpr int kMouseButtons[]{0, 2, 1, 3, 4};

        auto& io = ImGui::GetIO();
        for (auto&& p : pointers)
        {
            if (p.hash > array_size(io.MouseDown))
                continue;

            const int index = kMouseButtons[p.hash - 1];
            io.MouseDown[index] = down;
        }

        return io.WantCaptureMouse;
    }

    void shutdown()
    {
        auto& io = ImGui::GetIO();
        delete static_cast<Renderable*>(io.UserData);
        ImGui::Shutdown();
    }
}}  // namespace rainbow::imgui
