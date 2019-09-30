#ifndef THIRDPARTY_IMGUI_IMCONFIG_H_
#define THIRDPARTY_IMGUI_IMCONFIG_H_

#include "Common/Color.h"
#include "Common/Logging.h"
#include "Common/TypeCast.h"
#include "Math/Vec2.h"

#define IM_ASSERT(_EXPR) R_ASSERT((_EXPR), "dear imgui")
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define IMGUI_DISABLE_DEMO_WINDOWS
#define IMGUI_DISABLE_METRICS_WINDOW
#define IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION
#define IMGUI_STB_NAMESPACE stb

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
        return rainbow::Color{rainbow::narrow_cast<uint8_t>(x * 255),          \
                              rainbow::narrow_cast<uint8_t>(y * 255),          \
                              rainbow::narrow_cast<uint8_t>(z * 255),          \
                              rainbow::narrow_cast<uint8_t>(w * 255)};         \
    }

#define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT                                  \
    struct ImDrawVert                                                          \
    {                                                                          \
        ImU32 col;                                                             \
        ImVec2 uv;                                                             \
        ImVec2 pos;                                                            \
    }

#endif
