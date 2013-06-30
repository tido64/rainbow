// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#ifdef USE_CANVAS
#include "Applications/Canvas.h"
#include "Common/Algorithm.h"
#include "Graphics/Renderer.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureManager.h"
#include "Input/Input.h"

#ifdef GL_ES_VERSION_2_0
#	include "Graphics/Shaders/Shaders.h"
#else
#	define canvas_vsh       "Shaders/Canvas.vsh"
#	define canvaseraser_fsh "Shaders/CanvasEraser.fsh"
#endif

namespace
{
	int g_canvas_program = -1;

	struct BindFramebuffer
	{
		BindFramebuffer(const unsigned int framebuffer, const unsigned int texture)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		}

		~BindFramebuffer()
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}
	};

}

Canvas::Canvas() :
	changed(false), down(false), fill(0.0f), brush_size(7), width(0), height(0),
	background_tex(0), canvas_fb(0), canvas_tex(0), brush(nullptr),
	filled(nullptr)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	this->width = viewport[2];
	this->height = viewport[3];
	R_ASSERT(this->width  > 0, "Invalid framebuffer width");
	R_ASSERT(this->height > 0, "Invalid framebuffer height");

	if (g_canvas_program < 0)
	{
		int shaders[2];
		shaders[0] = ShaderManager::Instance->create_shader(
			Shader::VERTEX_SHADER, canvas_vsh);
		shaders[1] = ShaderManager::Instance->create_shader(
			Shader::FRAGMENT_SHADER, canvaseraser_fsh);
		g_canvas_program = ShaderManager::Instance->create_program(shaders, 2);

		const float u = 2.0f / this->width;
		const float v = 2.0f / this->height;
		const float ortho[] = {
			   u, 0.0f,  0.0f, -1.0f,
			0.0f,    v,  0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,  0.0f,
			0.0f, 0.0f,  0.0f,  1.0f
		};

		const unsigned int gl_program =
			ShaderManager::Instance->get_program(g_canvas_program);
		ShaderManager::Instance->use(g_canvas_program);
		glUniformMatrix4fv(
			glGetUniformLocation(gl_program, "mvp_matrix"), 1, GL_FALSE, ortho);
		glUniform1i(glGetUniformLocation(gl_program, "canvas"), 1);
		ShaderManager::Instance->reset();
	}

	this->background_tex = TextureManager::Instance().create(
		GL_RGBA, this->width, this->height, GL_RGBA, nullptr);
	this->canvas_tex = TextureManager::Instance().create(
		GL_RGBA, this->width, this->height, GL_RGBA, nullptr);

	glGenFramebuffers(1, &this->canvas_fb);
	{
		BindFramebuffer bind(this->canvas_fb, this->canvas_tex);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			this->release();
			R_ERROR("[Rainbow] GL: Failed to create framebuffer\n");
			return;
		}
		Renderer::clear();
	}

	this->filled = new bool[width * height];
	memset(this->filled, 0, width * height * sizeof(bool));

	this->sprite[0].texcoord.x = 0.0f;
	this->sprite[0].texcoord.y = 0.0f;
	this->sprite[0].position.x = 0;
	this->sprite[0].position.y = 0;

	this->sprite[1].texcoord.x = 1.0f;
	this->sprite[1].texcoord.y = 0.0f;
	this->sprite[1].position.x = this->width;
	this->sprite[1].position.y = 0;

	this->sprite[2].texcoord.x = 1.0f;
	this->sprite[2].texcoord.y = 1.0f;
	this->sprite[2].position.x = this->width;
	this->sprite[2].position.y = this->height;

	this->sprite[3].texcoord.x = 0.0f;
	this->sprite[3].texcoord.y = 1.0f;
	this->sprite[3].position.x = 0;
	this->sprite[3].position.y = this->height;

	Input::Instance->subscribe(this, Input::TOUCH_EVENTS);
}

Canvas::~Canvas()
{
	Input::Instance->unsubscribe(this);
	this->release();
}

void Canvas::clear()
{
	BindFramebuffer bind(this->canvas_fb, this->canvas_tex);
	Renderer::clear();
	memset(this->filled, 0, width * height * sizeof(bool));
	this->changed = false;
	this->fill = 0.0f;
	this->touch_canceled();
}

float Canvas::get_filled()
{
	if (this->fill < 0.0f)
	{
		volatile int sum = 0;
		const int size = this->width * this->height;
		for (int i = 0; i < size; ++i)
		{
		#ifdef RAINBOW_IOS
			// TODO: In Xcode 4.6.3, turning on optimisations (-Os) will break
			//       this loop unless we branch and increment.
			if (this->filled[i])
				++sum;
		#else
			// Guarantee that true == 1 and not 255 like on some Android devices.
			sum += static_cast<int>(this->filled[i]) & 1;
		#endif
		}
		this->fill = static_cast<float>(sum) / static_cast<float>(size);
	}
	return this->fill;
}

void Canvas::set_background(const unsigned int color)
{
	// Set clear colour so we can wipe the background.
	Colorf c(color);
	glClearColor(c.r, c.g, c.b, c.a);

	// Change background colour.
	BindFramebuffer bind(this->canvas_fb, this->background_tex);
	Renderer::clear();

	// Reset clear colour.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Canvas::set_background(const Texture &texture, const int width, const int height)
{
	SpriteVertex vx[4];
	for (size_t i = 0; i < 4; ++i)
		vx[i].texcoord = texture.vx[i];

	if (width < height)
	{
		vx[0].position.y = 0;
		vx[2].position.y = this->height;
		vx[2].position.x = static_cast<float>(this->height)
		                 * static_cast<float>(width)
		                 / static_cast<float>(height);
		vx[0].position.x = (this->width - vx[2].position.x) / 2.0f;
		vx[2].position.x += vx[0].position.x;
	}
	else  // width >= height
	{
		vx[0].position.x = 0;
		vx[2].position.x = this->width;
		vx[2].position.y = static_cast<float>(this->width)
		                 * static_cast<float>(height)
		                 / static_cast<float>(width);
		vx[0].position.y = (this->height - vx[2].position.y) / 2.0f;
		vx[2].position.y += vx[0].position.y;
	}
	vx[1].position.x = vx[2].position.x;
	vx[1].position.y = vx[0].position.y;
	vx[3].position.x = vx[0].position.x;
	vx[3].position.y = vx[2].position.y;

	BindFramebuffer bind(this->canvas_fb, this->background_tex);
	R_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
	         "Failed to set background");

	Renderer::clear();
	TextureManager::Instance().bind(texture);
	Renderer::draw_elements(vx, 6);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to set background");
}

void Canvas::set_foreground(const unsigned int color)
{
	this->foreground_color = color;
}

void Canvas::create_point_and_mark(SpriteVertex *vertex, const int x, const int y)
{
	R_ASSERT(this->brush, "No brush assigned");

	int left = x - (this->brush_size >> 1);
	int right = left + this->brush_size;
	int bottom = y - (this->brush_size >> 1);
	int top = bottom + this->brush_size;

	left = Rainbow::max<int>(0.0, left);
	bottom = Rainbow::max<int>(0.0, bottom);
	right = Rainbow::min<int>(this->width, right);
	top = Rainbow::min<int>(this->height, top);

	vertex[0].texcoord = this->brush->vx[0];
	vertex[0].position.x = left;
	vertex[0].position.y = bottom;

	vertex[1].texcoord = this->brush->vx[1];
	vertex[1].position.x = right;
	vertex[1].position.y = bottom;

	vertex[2].texcoord = this->brush->vx[2];
	vertex[2].position.x = right;
	vertex[2].position.y = top;

	vertex[3].texcoord = this->brush->vx[3];
	vertex[3].position.x = left;
	vertex[3].position.y = top;

	const size_t num = (right - left) * sizeof(bool);
	for (int i = bottom; i < top; ++i)
	{
		const size_t offset = left + this->width * i;
		memset(this->filled + offset, 0xff, num);
	}
}

void Canvas::release()
{
	if (this->canvas_fb)
	{
		glDeleteFramebuffers(1, &this->canvas_fb);
		this->canvas_fb = 0;
	}
	if (this->canvas_tex)
	{
		TextureManager::Instance().remove(this->canvas_tex);
		this->canvas_tex = 0;
	}
	if (this->background_tex)
	{
		TextureManager::Instance().remove(this->background_tex);
		this->background_tex = 0;
	}
}

void Canvas::draw_impl()
{
	ShaderManager::Instance->use(g_canvas_program);
	TextureManager::Instance().bind(this->canvas_tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->background_tex);
	Renderer::draw_elements(this->sprite, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	TextureManager::Instance().bind();
	ShaderManager::Instance->reset();
}

void Canvas::update_impl()
{
	if (!this->changed)
		return;

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	TextureManager::Instance().bind(*this->brush);
	BindFramebuffer bind(this->canvas_fb, this->canvas_tex);

	SpriteVertex vx[4];
	vx[0].color = this->foreground_color;
	vx[1].color = this->foreground_color;
	vx[2].color = this->foreground_color;
	vx[3].color = this->foreground_color;
	if (this->touch.x == this->prev_point.x && this->touch.y == this->prev_point.y)
	{
		this->create_point_and_mark(vx, this->touch.x, this->touch.y);
		Renderer::draw_elements(vx, 6);
	}
	else
	{
		const int dx = this->touch.x - this->prev_point.x;
		const int dy = this->touch.y - this->prev_point.y;

		const int points = Rainbow::min<int>(256, Rainbow::max<float>(fabsf(dx), fabsf(dy)));
		R_ASSERT(points > 0, "No points to draw");

		Vector<SpriteVertex> vertices(points * 4);
		for (int i = 0; i < points; ++i)
		{
			const float p = static_cast<float>(i) / static_cast<float>(points);
			this->create_point_and_mark(
				vx, this->prev_point.x + dx * p, this->prev_point.y + dy * p);
			vertices.push_back(vx[0]);
			vertices.push_back(vx[1]);
			vertices.push_back(vx[2]);
			vertices.push_back(vx[3]);
		}
		this->array.update(vertices.begin(), (points << 2) * sizeof(SpriteVertex));
		this->array.count = points * 6;
		Renderer::draw(this->array);

		// Update previous point. We use this method because input events might
		// occur several times between frames, so (x0,y0) becomes unreliable.
		this->prev_point.x = this->touch.x;
		this->prev_point.y = this->touch.y;
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	this->changed = false;
	this->fill = -1.0f;
}

void Canvas::touch_began_impl(const Touch *const touches, const size_t)
{
	if (this->touch.hash)
		return;

	this->touch = *touches;
	this->prev_point.x = this->touch.x;
	this->prev_point.y = this->touch.y;
	this->changed = true;

#ifdef RAINBOW_SDL
	this->down = true;
#endif
}

void Canvas::touch_canceled_impl()
{
	this->touch.hash = 0;

#ifdef RAINBOW_SDL
	this->down = false;
#endif
}

void Canvas::touch_ended_impl(const Touch *const, const size_t)
{
	this->touch_canceled();
}

void Canvas::touch_moved_impl(const Touch *const touches, const size_t count)
{
#ifdef RAINBOW_SDL
	if (!this->down)
		return;
#endif

	for (size_t i = 0; i < count; ++i)
	{
		if (touches[i] == this->touch)
		{
			this->touch = *touches;
			this->changed = true;
			break;
		}
	}
}

#endif
