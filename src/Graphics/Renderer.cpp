// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Renderer.h"

#include <cstring>

#include "Graphics/Label.h"
#include "Graphics/SpriteBatch.h"

#define S0(i)  (i) * 4
#define S1(i)  S0(i), S0(i) + 1, S0(i) + 2, S0(i) + 2, S0(i) + 3, S0(i)

#define S4(i)       S1((i)),   S1((i) +   1),   S1((i) +   2),   S1((i) +   3)
#define S16(i)      S4((i)),   S4((i) +   4),   S4((i) +   8),   S4((i) +  12)
#define S64(i)     S16((i)),  S16((i) +  16),  S16((i) +  32),  S16((i) +  48)
#define S256(i)    S64((i)),  S64((i) +  64),  S64((i) + 128),  S64((i) + 192)
#define S1024(i)  S256((i)), S256((i) + 256), S256((i) + 512), S256((i) + 768)

namespace
{
	constexpr std::array<float, 16> kProjectionMatrix()
	{
		return {{1.0f,  0.0f,  0.0f, 0.0f,
		         0.0f,  1.0f,  0.0f, 0.0f,
		         0.0f,  0.0f, -1.0f, 0.0f,
		        -1.0f, -1.0f,  0.0f, 1.0f}};
	}
}

void Renderer::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

bool Renderer::has_extension(const char *const extension)
{
	return strstr(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)),
	              extension);
}

int Renderer::max_texture_size()
{
	static const int max_texture_size = [] {
		int max_texture_size;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
		return max_texture_size;
	}();
	return max_texture_size;
}

void Renderer::set_projection(const float left,
                              const float right,
                              const float bottom,
                              const float top)
{
	ortho_[0] = 2.0f / (right - left);
	ortho_[5] = 2.0f / (top - bottom);
	ortho_[12] = -(right + left) / (right - left);
	ortho_[13] = -(top + bottom) / (top - bottom);
	shader_manager_.update_projection();
}

void Renderer::set_resolution(const Vec2i &resolution)
{
	R_ASSERT(is_global(),
	         "Cannot set resolution with an uninitialised renderer");

	view_ = resolution;
	ortho_[0] = 2.0f / resolution.x;
	ortho_[5] = 2.0f / resolution.y;
	shader_manager_.update_viewport();
	set_window_size(window_.is_zero() ? view_ : window_);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to set resolution");
}

void Renderer::set_window_size(const Vec2i &size, const float factor)
{
	R_ASSERT(is_global(),
	         "Cannot set window size with an uninitialised renderer");

	window_ = view_;
	if (size == view_)
	{
		origin_.x = 0;
		origin_.y = 0;
	}
	else
	{
		const float scale = std::min(size.x / static_cast<float>(window_.x),
		                             size.y / static_cast<float>(window_.y));
		window_.x *= scale;
		window_.y *= scale;
		origin_.x = (size.x - window_.x) / 2;
		origin_.y = (size.y - window_.y) / 2;
	}
	scale_.x = static_cast<float>(view_.x) / window_.x;
	scale_.y = static_cast<float>(view_.y) / window_.y;
	glViewport(origin_.x * factor,
	           origin_.y * factor,
	           window_.x * factor,
	           window_.y * factor);
	// Set height in order to properly transform mouse coordinates.
	window_.y = size.y;
}

void Renderer::bind_element_array() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
}

Vec2i Renderer::convert_to_flipped_view(const Vec2i &p) const
{
	return convert_to_view(Vec2i(p.x, window_.y - p.y));
}

Vec2i Renderer::convert_to_screen(const Vec2i &p) const
{
	return Vec2i(p.x / scale_.x + origin_.x, p.y / scale_.y + origin_.y);
}

Vec2i Renderer::convert_to_view(const Vec2i &p) const
{
	return Vec2i((p.x - origin_.x) * scale_.x, (p.y - origin_.y) * scale_.y);
}

void Renderer::reset() const
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glActiveTexture(GL_TEXTURE0);
}

void Renderer::unbind_all()
{
	VertexArray::unbind();
	shader_manager_.use(ShaderManager::kInvalidProgram);
	texture_manager_.bind();
}

Renderer::Renderer()
    : index_buffer_(0), scale_(1.0f, 1.0f), ortho_(kProjectionMatrix()),
      shader_manager_(this) {}

Renderer::~Renderer()
{
	if (index_buffer_)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &index_buffer_);
	}
}

bool Renderer::init()
{
#ifdef __glew_h__
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		LOGF("Failed to initialise GLEW: %s", glewGetErrorString(err));
		return false;
	}
#endif

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!shader_manager_.init())
		return false;

	const unsigned short kDefaultIndices[] = { S256(0) };
	static_assert(sizeof(kDefaultIndices) ==
	                  kNumSprites * 6 * sizeof(kDefaultIndices[0]),
	              "Number of indices do not match set number of sprites");
	glGenBuffers(1, &index_buffer_);
	bind_element_array();
	glBufferData(
	    GL_ELEMENT_ARRAY_BUFFER, sizeof(kDefaultIndices), kDefaultIndices,
	    GL_STATIC_DRAW);

	make_global();
	return glGetError() == GL_NO_ERROR;
}
