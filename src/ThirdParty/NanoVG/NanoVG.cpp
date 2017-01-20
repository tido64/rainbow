// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/NanoVG/NanoVG.h"

#include "Graphics/OpenGL.h"
#ifdef GL_ES_VERSION_2_0
#   define NANOVG_GLES2_IMPLEMENTATION 1
#   define nvgCreateContext nvgCreateGLES2
#   define nvgDeleteContext nvgDeleteGLES2
#else
#   define NANOVG_GL2_IMPLEMENTATION 1
#   define nvgCreateContext nvgCreateGL2
#   define nvgDeleteContext nvgDeleteGL2
#endif
#ifdef __GNUC__
#   pragma GCC diagnostic push
#   ifdef __clang__
#       pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#       pragma clang diagnostic ignored "-Wnested-anon-types"
#   else
#       pragma GCC diagnostic ignored "-pedantic"
#   endif
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#elif _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4201)
#endif
#include <nanovg_gl.h>
#ifdef __GNUC__
#   pragma GCC diagnostic pop
#elif _MSC_VER
#   pragma warning(pop)
#endif

#include "Graphics/Renderer.h"
#include "Graphics/VertexArray.h"

using nvg::Canvas;

Canvas::Canvas() : context_(nvgCreateContext(NVG_ANTIALIAS)) {}

Canvas::~Canvas() { nvgDeleteContext(context_); }

void Canvas::draw_impl()
{
    ShaderManager::Context context;
    rainbow::graphics::unbind_all();

    const Vec2i& res = rainbow::graphics::resolution();
    nvgBeginFrame(context_, res.x, res.y, 1.0f);
    paint_impl();
    nvgEndFrame(context_);

    rainbow::graphics::reset();
}
