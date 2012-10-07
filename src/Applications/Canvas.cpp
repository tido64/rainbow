// Copyright 2012 Bifrost Entertainment. All rights reserved.

#ifdef USE_CANVAS
#include "Algorithm.h"
#include "Applications/Canvas.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Shaders/Shaders.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureManager.h"
#include "Input/Input.h"

Canvas::Canvas() :
	changed(false), down(false), fill(0.0f), brush_size(7),
	width(0), height(0), background_tex(0), canvas_fb(0), canvas_tex(0),
	vsh(nullptr), function(nullptr), brush(nullptr)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	this->width = viewport[2];
	this->height = viewport[3];
	R_ASSERT(this->width  > 0, "Invalid framebuffer width");
	R_ASSERT(this->height > 0, "Invalid framebuffer height");

#ifdef RAINBOW_IOS
	this->vsh = Renderer::load_shader(GL_VERTEX_SHADER, canvas_vsh);
#else
	this->vsh = Renderer::load_shader(GL_VERTEX_SHADER, "Shaders/Canvas.vsh");
#endif
	if (!this->vsh)
		return;

#ifdef RAINBOW_IOS
	this->function = Renderer::load_shader(GL_FRAGMENT_SHADER, canvaseraser_fsh);
#else
	this->function = Renderer::load_shader(GL_FRAGMENT_SHADER, "Shaders/CanvasEraser.fsh");
#endif
	if (!this->function)
		return;

	this->draw_program.attach_shader(this->vsh);
	this->draw_program.attach_shader(this->function);
	if (!this->draw_program.link())
		return;

	const float u = 2.0f / this->width;
	const float v = 2.0f / this->height;
	const float ortho[] = {
		   u, 0.0f,  0.0f, -1.0f,
		0.0f,    v,  0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,  0.0f,
		0.0f, 0.0f,  0.0f,  1.0f
	};

	Renderer::attach_pipeline(this->draw_program);
	glUniformMatrix4fv(glGetUniformLocation(this->draw_program, "mvp_matrix"), 1, GL_FALSE, ortho);
	glUniform1i(glGetUniformLocation(this->draw_program, "canvas"), 1);
	Renderer::detach_pipeline();

	glGenFramebuffers(1, &this->canvas_fb);
	glBindFramebuffer(GL_FRAMEBUFFER, this->canvas_fb);

	this->background_tex = TextureManager::Instance().create(GL_RGBA, this->width, this->height, GL_RGBA, nullptr);
	this->canvas_tex = TextureManager::Instance().create(GL_RGBA, this->width, this->height, GL_RGBA, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->canvas_tex, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		this->release();
		R_ERROR("[Rainbow] GL: Failed to create framebuffer\n");
		return;
	}

	Renderer::clear();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Renderer::create_buffer(this->canvas_buffer, this->canvas_vao);

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

	Renderer::delete_buffer(this->canvas_buffer, this->canvas_vao);
	delete this->function;
	delete this->vsh;
}

void Canvas::clear()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->canvas_fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->canvas_tex, 0);
	Renderer::clear();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	this->changed = false;
	this->fill = 0.0f;
	this->touch_canceled();
}

float Canvas::get_filled()
{
	if (this->fill < 0.0f)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->canvas_fb);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->canvas_tex, 0);
		const size_t size = (this->width * this->height) << 2;
		unsigned char *data = new unsigned char[size];
		glReadPixels(0, 0, this->width, this->height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		unsigned int sum = 0;
		for (size_t i = 0; i < size; i += 4)
			sum += data[i + 3] > 0;
		delete[] data;
		this->fill = static_cast<float>(sum) / static_cast<float>(size >> 2);
	}
	return this->fill;
}

void Canvas::set_background(const unsigned int color)
{
	// Set clear colour so we can wipe the background.
	Colorf c(color);
	glClearColor(c.r, c.g, c.b, c.a);

	// Change background colour.
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->canvas_fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->background_tex, 0);
	Renderer::clear();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

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
		vx[2].position.x = static_cast<float>(this->height) * static_cast<float>(width) / static_cast<float>(height);
		vx[0].position.x = (this->width - vx[2].position.x) / 2.0f;
		vx[2].position.x += vx[0].position.x;
	}
	else  // width >= height
	{
		vx[0].position.x = 0;
		vx[2].position.x = this->width;
		vx[2].position.y = static_cast<float>(this->width) * static_cast<float>(height) / static_cast<float>(width);
		vx[0].position.y = (this->height - vx[2].position.y) / 2.0f;
		vx[2].position.y += vx[0].position.y;
	}
	vx[1].position.x = vx[2].position.x;
	vx[1].position.y = vx[0].position.y;
	vx[3].position.x = vx[0].position.x;
	vx[3].position.y = vx[2].position.y;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->canvas_fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->background_tex, 0);
	R_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Failed to set background");

	Renderer::clear();
	TextureManager::Instance().bind(texture);
	Renderer::draw_elements(vx, 6);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to set background");
}

void Canvas::set_foreground(const unsigned int color)
{
	this->foreground_color = color;
}

void Canvas::draw()
{
	Renderer::attach_pipeline(this->draw_program);
	TextureManager::Instance().bind(this->canvas_tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->background_tex);
	Renderer::draw_elements(this->sprite, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	TextureManager::Instance().bind();
	Renderer::detach_pipeline();
}

void Canvas::update()
{
	if (!this->changed)
		return;

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	TextureManager::Instance().bind(*this->brush);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->canvas_fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->canvas_tex, 0);

	SpriteVertex vx[4];
	vx[0].color = this->foreground_color;
	vx[1].color = this->foreground_color;
	vx[2].color = this->foreground_color;
	vx[3].color = this->foreground_color;
	if (this->touch.x == this->prev_point.x && this->touch.y == this->prev_point.y)
	{
		this->create_point(vx, this->touch.x, this->touch.y);
		Renderer::draw_elements(vx, 6);
	}
	else
	{
		const int dt_x = this->touch.x - this->prev_point.x;
		const int dt_y = this->touch.y - this->prev_point.y;

		const int points = Rainbow::min<int>(256, (fabsf(dt_x) < fabsf(dt_y)) ? fabsf(dt_y) : fabsf(dt_x));
		R_ASSERT(points > 0, "No points to draw");

		Vector<SpriteVertex> vertices(points * 4);
		for (int i = 0; i < points; ++i)
		{
			const float p = static_cast<float>(i) / static_cast<float>(points);
			this->create_point(vx, this->prev_point.x + dt_x * p, this->prev_point.y + dt_y * p);
			vertices.push_back(vx[0]);
			vertices.push_back(vx[1]);
			vertices.push_back(vx[2]);
			vertices.push_back(vx[3]);
		}
		Renderer::update_buffer(this->canvas_buffer, (points << 2) * sizeof(SpriteVertex), vertices.begin());
	#ifndef RAINBOW_ANDROID
		Renderer::draw_buffer(this->canvas_vao, points * 6);
	#else
		Renderer::draw_buffer(this->canvas_buffer, points * 6);
	#endif

		// Update previous point. We use this method because input events might
		// occur several times between frames, so (x0,y0) becomes unreliable.
		this->prev_point.x = this->touch.x;
		this->prev_point.y = this->touch.y;
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	this->changed = false;
	this->fill = -1.0f;
}

void Canvas::touch_began(const Touch *const touches, const size_t)
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

void Canvas::touch_canceled()
{
	this->touch.hash = 0;

#ifdef RAINBOW_SDL
	this->down = false;
#endif
}

void Canvas::touch_ended(const Touch *const, const size_t)
{
	this->touch_canceled();
}

void Canvas::touch_moved(const Touch *const touches, const size_t count)
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

void Canvas::create_point(SpriteVertex *vertex, const int x, const int y)
{
	R_ASSERT(this->brush, "No brush assigned");

	const int a_x = x - (this->brush_size >> 1);
	const int a_y = y - (this->brush_size >> 1);

	vertex[0].texcoord = this->brush->vx[0];
	vertex[0].position.x = a_x;
	vertex[0].position.y = a_y;

	vertex[1].texcoord = this->brush->vx[1];
	vertex[1].position.x = a_x + this->brush_size;
	vertex[1].position.y = a_y;

	vertex[2].texcoord = this->brush->vx[2];
	vertex[2].position.x = vertex[1].position.x;
	vertex[2].position.y = a_y + this->brush_size;

	vertex[3].texcoord = this->brush->vx[3];
	vertex[3].position.x = vertex[0].position.x;
	vertex[3].position.y = vertex[2].position.y;
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

#endif
