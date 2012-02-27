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
	  0,   1,   2,   2,   3,   0,   4,   5,   6,   6,   7,   4,
	  8,   9,  10,  10,  11,   8,  12,  13,  14,  14,  15,  12,
	 16,  17,  18,  18,  19,  16,  20,  21,  22,  22,  23,  20,
	 24,  25,  26,  26,  27,  24,  28,  29,  30,  30,  31,  28,
	 32,  33,  34,  34,  35,  32,  36,  37,  38,  38,  39,  36,
	 40,  41,  42,  42,  43,  40,  44,  45,  46,  46,  47,  44,
	 48,  49,  50,  50,  51,  48,  52,  53,  54,  54,  55,  52,
	 56,  57,  58,  58,  59,  56,  60,  61,  62,  62,  63,  60,
	 64,  65,  66,  66,  67,  64,  68,  69,  70,  70,  71,  68,
	 72,  73,  74,  74,  75,  72,  76,  77,  78,  78,  79,  76,
	 80,  81,  82,  82,  83,  80,  84,  85,  86,  86,  87,  84,
	 88,  89,  90,  90,  91,  88,  92,  93,  94,  94,  95,  92,
	 96,  97,  98,  98,  99,  96, 100, 101, 102, 102, 103, 100,
	104, 105, 106, 106, 107, 104, 108, 109, 110, 110, 111, 108,
	112, 113, 114, 114, 115, 112, 116, 117, 118, 118, 119, 116,
	120, 121, 122, 122, 123, 120, 124, 125, 126, 126, 127, 124,
	128, 129, 130, 130, 131, 128, 132, 133, 134, 134, 135, 132,
	136, 137, 138, 138, 139, 136, 140, 141, 142, 142, 143, 140,
	144, 145, 146, 146, 147, 144, 148, 149, 150, 150, 151, 148,
	152, 153, 154, 154, 155, 152, 156, 157, 158, 158, 159, 156,
	160, 161, 162, 162, 163, 160, 164, 165, 166, 166, 167, 164,
	168, 169, 170, 170, 171, 168, 172, 173, 174, 174, 175, 172,
	176, 177, 178, 178, 179, 176, 180, 181, 182, 182, 183, 180,
	184, 185, 186, 186, 187, 184, 188, 189, 190, 190, 191, 188,
	192, 193, 194, 194, 195, 192, 196, 197, 198, 198, 199, 196,
	200, 201, 202, 202, 203, 200, 204, 205, 206, 206, 207, 204,
	208, 209, 210, 210, 211, 208, 212, 213, 214, 214, 215, 212,
	216, 217, 218, 218, 219, 216, 220, 221, 222, 222, 223, 220,
	224, 225, 226, 226, 227, 224, 228, 229, 230, 230, 231, 228,
	232, 233, 234, 234, 235, 232, 236, 237, 238, 238, 239, 236,
	240, 241, 242, 242, 243, 240, 244, 245, 246, 246, 247, 244,
	248, 249, 250, 250, 251, 248, 252, 253, 254, 254, 255, 252
};
