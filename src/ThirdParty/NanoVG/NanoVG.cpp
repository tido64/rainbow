// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/NanoVG/NanoVG.h"

#include "Graphics/OpenGL.h"
#ifdef GL_ES_VERSION_2_0
#	define NANOVG_GLES2_IMPLEMENTATION 1
#	define nvgCreateContext nvgCreateGLES2
#	define nvgDeleteContext nvgDeleteGLES2
#else
#	define NANOVG_GL2_IMPLEMENTATION 1
#	define nvgCreateContext nvgCreateGL2
#	define nvgDeleteContext nvgDeleteGL2
#endif
#ifdef __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#	pragma GCC diagnostic ignored "-Wnested-anon-types"
#	pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#include <nanovg_gl.h>
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif

#include "Graphics/Renderer.h"
#include "Graphics/VertexArray.h"

using nvg::Context;

Context::Context() : context_(nvgCreateContext(NVG_ANTIALIAS)) {}

Context::~Context() { nvgDeleteContext(context_); }

void Context::draw()
{
	ShaderManager::Context context;
	auto renderer = Renderer::Get();
	renderer->unbind_all();

	const Vec2i &res = renderer->resolution();
	nvgBeginFrame(context_, res.x, res.y, 1.0f);
	for (auto&& cmd : commands_)
		cmd(context_);
	nvgEndFrame(context_);
	commands_.clear();

	renderer->reset();
}
