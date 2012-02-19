// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Common/Data.h"
#include "Common/SpriteVertex.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Pipeline.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"

bool Renderer::init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	vertex_shader = load_shader(GL_VERTEX_SHADER, "Shaders/Fixed2D.vsh");
	if (!vertex_shader)
		return false;

	fragment_shader = load_shader(GL_FRAGMENT_SHADER, "Shaders/Fixed2D.fsh");
	if (!fragment_shader)
		return false;

	pipeline = new Pipeline();
	pipeline->attach_shader(vertex_shader);
	pipeline->attach_shader(fragment_shader);
	if (!pipeline->link())
		return false;
	pipeline->use();

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 384, default_indices, GL_STATIC_DRAW);

	return glGetError() == GL_NO_ERROR;
}

void Renderer::release()
{
	glUseProgram(0);
	delete pipeline;
	delete fragment_shader;
	delete vertex_shader;
}

void Renderer::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::resize(const unsigned int width, const unsigned int height)
{
	const float u = 2.0f / width;
	const float v = 2.0f / height;
	const float ortho[] = {
		   u, 0.0f,  0.0f, -1.0f,
		0.0f,    v,  0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,  0.0f,
		0.0f, 0.0f,  0.0f,  1.0f
	};

	glUniformMatrix4fv(glGetUniformLocation(*pipeline, "mvp_matrix"), 1, 0, ortho);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to initialise OpenGL viewport");
}

void Renderer::bind_buffer(const unsigned int buffer)
{
	static unsigned int bound = 0;
	if (buffer == bound)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	bound = buffer;
}

void Renderer::bind_texture(const unsigned int texture)
{
	static unsigned int bound = 0;
	if (texture == bound)
		return;

	glBindTexture(GL_TEXTURE_2D, texture);
	bound = texture;
}

void Renderer::create_buffer(unsigned int &buffer)
{
	glGenBuffers(1, &buffer);
}

void Renderer::create_texture(unsigned int &texture, const unsigned int internal_format,
                              const unsigned int width, const unsigned int height,
                              const unsigned int format, const void *data)
{
	glGenTextures(1, &texture);
	bind_texture(texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to create texture");
}

void Renderer::delete_buffer(const unsigned int buffer)
{
	glDeleteBuffers(1, &buffer);
}

void Renderer::delete_texture(const unsigned int texture)
{
	glDeleteTextures(1, &texture);
}

void Renderer::draw_buffer(const unsigned int buffer, const unsigned int count)
{
	bind_buffer(buffer);

	glVertexAttribPointer(Pipeline::COLOR_LOCATION, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SpriteVertex), 0);
	glVertexAttribPointer(Pipeline::TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), SpriteVertex::tx_offset);
	glVertexAttribPointer(Pipeline::VERTEX_LOCATION, 2, GL_FLOAT, GL_TRUE, sizeof(SpriteVertex), SpriteVertex::vx_offset);

	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_BYTE, 0);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to draw buffer");
}

void Renderer::draw_elements(const SpriteVertex *vertices, const unsigned int count)
{
	bind_buffer(0);

	glVertexAttribPointer(Pipeline::COLOR_LOCATION, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SpriteVertex), &vertices->color);
	glVertexAttribPointer(Pipeline::TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), &vertices->texcoord);
	glVertexAttribPointer(Pipeline::VERTEX_LOCATION, 2, GL_FLOAT, GL_TRUE, sizeof(SpriteVertex), &vertices->position);

	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_BYTE, 0);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to draw elements");
}

Shader* Renderer::load_shader(const unsigned int type, const char *const src_path)
{
	const char *full_path = Data::get_path(src_path);
	Data source(full_path);
	Data::free(full_path);
	if (!source)
		return nullptr;

	Shader *shader = new Shader(type, source);
	if (!*shader)
	{
		delete shader;
		shader = nullptr;
		R_ASSERT(shader, "load_shader: Failed to compile shader");
	}
	return shader;
}

void Renderer::update_buffer(const unsigned int buffer, const unsigned int size, const void *data)
{
	bind_buffer(buffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	bind_buffer(0);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to update buffer");
}

unsigned int Renderer::index_buffer = 0;
Pipeline *Renderer::pipeline = nullptr;
Shader *Renderer::vertex_shader = nullptr;
Shader *Renderer::fragment_shader = nullptr;
const unsigned char Renderer::default_indices[] = {
	  0,   1,   2,   2,   1,   3,   4,   5,   6,   6,   5,   7,
	  8,   9,  10,  10,   9,  11,  12,  13,  14,  14,  13,  15,
	 16,  17,  18,  18,  17,  19,  20,  21,  22,  22,  21,  23,
	 24,  25,  26,  26,  25,  27,  28,  29,  30,  30,  29,  31,
	 32,  33,  34,  34,  33,  35,  36,  37,  38,  38,  37,  39,
	 40,  41,  42,  42,  41,  43,  44,  45,  46,  46,  45,  47,
	 48,  49,  50,  50,  49,  51,  52,  53,  54,  54,  53,  55,
	 56,  57,  58,  58,  57,  59,  60,  61,  62,  62,  61,  63,
	 64,  65,  66,  66,  65,  67,  68,  69,  70,  70,  69,  71,
	 72,  73,  74,  74,  73,  75,  76,  77,  78,  78,  77,  79,
	 80,  81,  82,  82,  81,  83,  84,  85,  86,  86,  85,  87,
	 88,  89,  90,  90,  89,  91,  92,  93,  94,  94,  93,  95,
	 96,  97,  98,  98,  97,  99, 100, 101, 102, 102, 101, 103,
	104, 105, 106, 106, 105, 107, 108, 109, 110, 110, 109, 111,
	112, 113, 114, 114, 113, 115, 116, 117, 118, 118, 117, 119,
	120, 121, 122, 122, 121, 123, 124, 125, 126, 126, 125, 127,
	128, 129, 130, 130, 129, 131, 132, 133, 134, 134, 133, 135,
	136, 137, 138, 138, 137, 139, 140, 141, 142, 142, 141, 143,
	144, 145, 146, 146, 145, 147, 148, 149, 150, 150, 149, 151,
	152, 153, 154, 154, 153, 155, 156, 157, 158, 158, 157, 159,
	160, 161, 162, 162, 161, 163, 164, 165, 166, 166, 165, 167,
	168, 169, 170, 170, 169, 171, 172, 173, 174, 174, 173, 175,
	176, 177, 178, 178, 177, 179, 180, 181, 182, 182, 181, 183,
	184, 185, 186, 186, 185, 187, 188, 189, 190, 190, 189, 191,
	192, 193, 194, 194, 193, 195, 196, 197, 198, 198, 197, 199,
	200, 201, 202, 202, 201, 203, 204, 205, 206, 206, 205, 207,
	208, 209, 210, 210, 209, 211, 212, 213, 214, 214, 213, 215,
	216, 217, 218, 218, 217, 219, 220, 221, 222, 222, 221, 223,
	224, 225, 226, 226, 225, 227, 228, 229, 230, 230, 229, 231,
	232, 233, 234, 234, 233, 235, 236, 237, 238, 238, 237, 239,
	240, 241, 242, 242, 241, 243, 244, 245, 246, 246, 245, 247,
	248, 249, 250, 250, 249, 251, 252, 253, 254, 254, 253, 255
};
