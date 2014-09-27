// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Renderer.h"

#include "Graphics/Label.h"
#include "Graphics/SpriteBatch.h"

#define S0(i)  (i) * 4
#define S1(i)  S0(i), S0(i) + 1, S0(i) + 2, S0(i) + 2, S0(i) + 3, S0(i)

#define S4(i)       S1((i)),   S1((i) +   1),   S1((i) +   2),   S1((i) +   3)
#define S16(i)      S4((i)),   S4((i) +   4),   S4((i) +   8),   S4((i) +  12)
#define S64(i)     S16((i)),  S16((i) +  16),  S16((i) +  32),  S16((i) +  48)
#define S256(i)    S64((i)),  S64((i) +  64),  S64((i) + 128),  S64((i) + 192)
#define S1024(i)  S256((i)), S256((i) + 256), S256((i) + 512), S256((i) + 768)

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

void Renderer::set_resolution(const Vec2i &resolution)
{
	R_ASSERT(is_global(),
	         "Cannot set resolution with an uninitialised renderer");

	view_ = resolution;
	shader_manager_.set(resolution);
	if (window_.is_zero())
		window_ = view_;
	set_window_size(window_);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to set resolution");
}

void Renderer::set_window_size(const Vec2i &size)
{
	R_ASSERT(is_global(),
	         "Cannot set window size with an uninitialised renderer");

	if (zoom_mode_ == ZoomMode::Stretch || size == view_)
	{
		origin_.x = 0;
		origin_.y = 0;
		window_ = size;
	}
	else
	{
		const Vec2f ratio(size.width / static_cast<float>(view_.width),
		                  size.height / static_cast<float>(view_.height));
		if ((zoom_mode_ == ZoomMode::Zoom && ratio.x < ratio.y)
		    || (zoom_mode_ == ZoomMode::LetterBox && ratio.x > ratio.y))
		{
			window_.width = view_.width * ratio.y;
			window_.height = size.height;
			origin_.x = (size.width - window_.width) / 2;
			origin_.y = 0;
		}
		else
		{
			window_.width = size.width;
			window_.height = view_.height * ratio.x;
			origin_.x = 0;
			origin_.y = (size.height - window_.height) / 2;
		}
	}
	glViewport(origin_.x, origin_.y, window_.width, window_.height);
	scale_.x = static_cast<float>(view_.width) / window_.width;
	scale_.y = static_cast<float>(view_.height) / window_.height;
	window_.height = size.height;
}

void Renderer::set_zoom_mode(const ZoomMode zoom)
{
	if (zoom == zoom_mode_)
		return;

	zoom_mode_ = zoom;
	set_window_size(window_);
}

void Renderer::bind_element_array() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
}

Vec2i Renderer::convert_to_flipped_view(const Vec2i &p) const
{
	return convert_to_view(Vec2i(p.x, window_.height - p.y));
}

Vec2i Renderer::convert_to_view(const Vec2i &p) const
{
	return Vec2i((p.x - origin_.x) * scale_.x, (p.y - origin_.y) * scale_.y);
}

Renderer::Renderer()
    : index_buffer_(0), zoom_mode_(ZoomMode::LetterBox), scale_(1.0f, 1.0f) { }

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
		R_ERROR("[Rainbow] Failed to initialise GLEW: %s\n",
		        glewGetErrorString(err));
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
