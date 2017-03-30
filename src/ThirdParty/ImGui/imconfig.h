//-----------------------------------------------------------------------------
// USER IMPLEMENTATION
// This file contains compile-time options for ImGui.
// Other options (memory allocation overrides, callbacks, etc.) can be set at runtime via the ImGuiIO structure - ImGui::GetIO().
//-----------------------------------------------------------------------------

#ifndef THIRDPARTY_IMGUI_IMCONFIG_H_
#define THIRDPARTY_IMGUI_IMCONFIG_H_

#include "Common/Color.h"
#include "Common/Logging.h"
#include "Math/Vec2.h"

//---- Define assertion handler. Defaults to calling assert().
#define IM_ASSERT(_EXPR)  R_ASSERT((_EXPR), "dear imgui")

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows.
//#define IMGUI_API __declspec( dllexport )
//#define IMGUI_API __declspec( dllimport )

//---- Include imgui_user.h at the end of imgui.h
//#define IMGUI_INCLUDE_IMGUI_USER_H

//---- Don't implement default handlers for Windows (so as not to link with OpenClipboard() and others Win32 functions)
//#define IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCS
//#define IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCS

//---- Don't implement help and test window functionality (ShowUserGuide()/ShowStyleEditor()/ShowTestWindow() methods will be empty)
//#define IMGUI_DISABLE_TEST_WINDOWS

//---- Don't define obsolete functions names
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

//---- Implement STB libraries in a namespace to avoid conflicts
#define IMGUI_STB_NAMESPACE stb

//---- Define constructor and implicit cast operators to convert back<>forth from your math types and ImVec2/ImVec4.
#define IM_VEC2_CLASS_EXTRA                                                    \
    ImVec2(const rainbow::Vec2f& f) : x(f.x), y(f.y) {}                        \
    ImVec2(const rainbow::Vec2i& i) : x(i.x), y(i.y) {}                        \
    operator rainbow::Vec2f() const { return rainbow::Vec2f{x, y}; }

#define IM_VEC4_CLASS_EXTRA                                                    \
    ImVec4(const rainbow::Color& c)                                            \
        : x(c.r / 255.0f), y(c.g / 255.0f), z(c.b / 255.0f), w(c.a / 255.0f)   \
    {                                                                          \
    }                                                                          \
                                                                               \
    operator rainbow::Color() const                                            \
    {                                                                          \
        return rainbow::Color{static_cast<uint8_t>(x * 255),                   \
                              static_cast<uint8_t>(y * 255),                   \
                              static_cast<uint8_t>(z * 255),                   \
                              static_cast<uint8_t>(w * 255)};                  \
    }

//---- Tip: You can add extra functions within the ImGui:: namespace, here or in your own headers files.
//---- e.g. create variants of the ImGui::Value() helper for your low-level math types, or your own widgets/helpers.
/*
namespace ImGui
{
    void    Value(const char* prefix, const MyMatrix44& v, const char* float_format = NULL);
}
*/

#define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT                                  \
    struct ImDrawVert                                                          \
    {                                                                          \
        ImU32 col;                                                             \
        ImVec2 uv;                                                             \
        ImVec2 pos;                                                            \
    }

#endif
