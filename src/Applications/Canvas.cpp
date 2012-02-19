// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Applications/Canvas.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Input/Input.h"

#ifdef RAINBOW_IOS
#	define GL_DRAW_FRAMEBUFFER          GL_DRAW_FRAMEBUFFER_APPLE
#	define GL_DRAW_FRAMEBUFFER_BINDING  GL_DRAW_FRAMEBUFFER_BINDING_APPLE
#endif

Canvas::Canvas(const int w, const int h) :
	changed(false), down(false), brush_size(7), background_tex(0),
	canvas_fb(0), canvas_tex(0), x(0), y(0), width(w), height(h),
	vsh(nullptr), fsh(nullptr), brush(nullptr)
{
	this->vsh = Renderer::load_shader(GL_VERTEX_SHADER, "Shaders/Canvas.vsh");
	if (!this->vsh)
		return;

	this->fsh = Renderer::load_shader(GL_FRAGMENT_SHADER, "Shaders/Canvas.fsh");
	if (!this->fsh)
		return;

	this->pipeline.attach_shader(this->vsh);
	this->pipeline.attach_shader(this->fsh);
	if (!this->pipeline.link())
		return;

	int program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	glUseProgram(this->pipeline);

	this->brush_attr = glGetAttribLocation(this->pipeline, "brush_size");
	const float u = 2.0f / this->width;
	const float v = 2.0f / this->height;
	const float ortho[] = {
		   u, 0.0f,  0.0f, -1.0f,
		0.0f,    v,  0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,  0.0f,
		0.0f, 0.0f,  0.0f,  1.0f
	};
	glUniformMatrix4fv(glGetUniformLocation(this->pipeline, "mvp_matrix"), 1, 0, ortho);
	glUniform1i(glGetUniformLocation(this->pipeline, "canvas"), 1);
	glUniform2f(glGetUniformLocation(this->pipeline, "dimension"), this->width, this->height);
	glUseProgram(program);

	glGenFramebuffers(1, &this->canvas_fb);
	glBindFramebuffer(GL_FRAMEBUFFER, this->canvas_fb);

	Renderer::create_texture(this->background_tex, GL_RGBA, this->width, this->height, GL_RGBA, nullptr);
	Renderer::create_texture(this->canvas_tex, GL_RGBA, this->width, this->height, GL_RGBA, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->canvas_tex, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		this->release();
		return;
	}

	Renderer::clear();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->sprite[0].texcoord.x = 1.0f;
	this->sprite[0].texcoord.y = 0.0f;

	this->sprite[1].texcoord.x = 0.0f;
	this->sprite[1].texcoord.y = 0.0f;

	this->sprite[2].texcoord.x = 1.0f;
	this->sprite[2].texcoord.y = 1.0f;

	this->sprite[3].texcoord.x = 0.0f;
	this->sprite[3].texcoord.y = 1.0f;

	this->sprite[0].position.x = this->width * 0.5f;
	this->sprite[0].position.y = this->height * 0.5f;

	this->sprite[1].position.x = this->sprite[0].position.x - this->width;
	this->sprite[1].position.y = this->sprite[0].position.y;

	this->sprite[2].position.x = this->sprite[0].position.x;
	this->sprite[2].position.y = this->sprite[0].position.y - this->height;

	this->sprite[3].position.x = this->sprite[1].position.x;
	this->sprite[3].position.y = this->sprite[2].position.y;

	Input::Instance().subscribe(this, RAINBOW_TOUCH_EVENTS);
}

Canvas::~Canvas()
{
	Input::Instance().unsubscribe(this);
	this->release();

	delete this->fsh;
	delete this->vsh;
}

void Canvas::clear()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, this->canvas_fb);
	Renderer::clear();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Canvas::set_background(const unsigned int color)
{
	// Set clear colour so we can wipe the background.
	Colorf c(color);
	glClearColor(c.r, c.g, c.b, c.a);

	// Change background colour.
	glBindFramebuffer(GL_FRAMEBUFFER, this->canvas_fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->background_tex, 0);
	Renderer::clear();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->canvas_tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Reset clear colour.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Canvas::set_background(const Texture &texture)
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->canvas_fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->background_tex, 0);

	Renderer::bind_texture(texture);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->canvas_tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::set_foreground(const unsigned int color)
{
	int program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	glUseProgram(this->pipeline);

	Colorf c(color);
	glUniform4f(glGetUniformLocation(this->pipeline, "color"), c.r, c.g, c.b, c.a);

	glUseProgram(program);
}

void Canvas::set_position(const int x, const int y)
{
	const int dt_x = x - this->x;
	const int dt_y = y - this->y;

	for (unsigned int i = 0; i < 4; ++i)
	{
		this->sprite[i].position.x += dt_x;
		this->sprite[i].position.y += dt_y;
	}

	this->x = x;
	this->y = y;
}

void Canvas::draw()
{
	Renderer::bind_texture(this->canvas_tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->background_tex);
	Renderer::draw_elements(this->sprite, 6);
	glActiveTexture(GL_TEXTURE0);
}

void Canvas::update()
{
	if (!this->changed)
		return;

	BrushVertex vx[4];
	Vector<BrushVertex> vertices;
	if (this->touch.x == this->prev_point.x && this->touch.y == this->prev_point.y)
	{
		this->create_point(vx, this->touch.x, this->touch.y);
		for (unsigned int i = 0; i < 4; ++i)
			vertices.push_back(vx[i]);
	}
	else
	{
		const int dt_x = this->touch.x - this->prev_point.x;
		const int dt_y = this->touch.y - this->prev_point.y;

		int points = (fabsf(dt_x) < fabsf(dt_y)) ? fabsf(dt_y) : fabsf(dt_x);
		R_ASSERT(points > 0, "update: No points to draw");

		for (int i = 0; i < points; ++i)
		{
			const float p = static_cast<float>(i) / static_cast<float>(points);
			this->create_point(vx, this->prev_point.x + dt_x * p, this->prev_point.y + dt_y * p);
			for (unsigned int i = 0; i < 4; ++i)
				vertices.push_back(vx[i]);
		}

		// Update previous point. We use this method because input events might
		// occur several times between frames, so (x0,y0) becomes unreliable.
		this->prev_point.x = this->touch.x;
		this->prev_point.y = this->touch.y;
	}

	int program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	glUseProgram(this->pipeline);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->canvas_fb);
	glDisableVertexAttribArray(Pipeline::COLOR_LOCATION);

	// Bind canvas and brush textures.
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->canvas_tex);
	glActiveTexture(GL_TEXTURE0);
	Renderer::bind_texture(*this->brush);

	glVertexAttribPointer(Pipeline::TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(BrushVertex), &vertices[0].texcoord);
	glVertexAttribPointer(Pipeline::VERTEX_LOCATION, 2, GL_FLOAT, GL_TRUE, sizeof(BrushVertex), &vertices[0].position);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());

	glEnableVertexAttribArray(Pipeline::COLOR_LOCATION);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glUseProgram(program);

	this->changed = false;
}

void Canvas::touch_began(const Touch *const touches, const unsigned int)
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

void Canvas::touch_ended(const Touch *const touches, const unsigned int count)
{
#ifdef RAINBOW_SDL
	if (!this->down)
		return;
#endif

	for (unsigned int i = 0; i < count; ++i)
	{
		if (touches[i] == this->touch)
		{
			if (this->touch.x == touches->x && this->touch.y == touches->y)
				this->touch.hash = 0;
			else
			{
				this->touch = *touches;
				this->changed = true;
			}
			break;
		}
	}
#ifdef RAINBOW_SDL
	this->down = false;
#endif
}

void Canvas::touch_moved(const Touch *const touches, const unsigned int count)
{
#ifdef RAINBOW_SDL
	if (!this->down)
		return;
#endif

	for (unsigned int i = 0; i < count; ++i)
	{
		if (touches[i] == this->touch)
		{
			this->touch = *touches;
			this->changed = true;
			break;
		}
	}
}

void Canvas::create_point(BrushVertex *vertex, const int x, const int y)
{
	R_ASSERT(this->brush, "create_point: No brush assigned");

	vertex[0].texcoord = this->brush->vx[0];
	vertex[1].texcoord = this->brush->vx[1];
	vertex[2].texcoord = this->brush->vx[2];
	vertex[3].texcoord = this->brush->vx[3];

	const int a_x = x - (this->brush_size >> 1);
	const int a_y = y - (this->brush_size >> 1);

	vertex[0].position.x = a_x + this->brush_size;
	vertex[0].position.y = a_y + this->brush_size;

	vertex[1].position.x = a_x;
	vertex[1].position.y = vertex[0].position.y;

	vertex[2].position.x = vertex[0].position.x;
	vertex[2].position.y = a_y;

	vertex[3].position.x = vertex[1].position.x;
	vertex[3].position.y = vertex[2].position.y;
}

void Canvas::release()
{
	if (this->canvas_fb)
		glDeleteFramebuffers(1, &this->canvas_fb);
	this->canvas_fb = 0;

	if (this->canvas_tex)
		Renderer::delete_texture(this->canvas_tex);
	this->canvas_tex = 0;

	if (this->background_tex)
		Renderer::delete_texture(this->background_tex);
	this->background_tex = 0;
}
