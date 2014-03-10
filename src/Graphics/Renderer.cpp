// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Label.h"
#include "Graphics/Renderer.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/ShaderManager.h"
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

template<typename T>
void Renderer::draw(const T &obj)
{
	obj.vertex_array().bind();
	glDrawElements(GL_TRIANGLES, obj.count(), GL_UNSIGNED_SHORT, nullptr);
}

void Renderer::draw(const SceneGraph::Node &node)
{
	if (!node.enabled)
		return;

	ShaderManager::Context context;
	if (node.program >= 0)
		ShaderManager::Instance->use(node.program);

	switch (node.type)
	{
		case SceneGraph::Node::DrawableNode:
			node.drawable->draw();
			break;
		case SceneGraph::Node::LabelNode:
			Renderer::draw(*node.label);
			break;
		case SceneGraph::Node::SpriteBatchNode:
			Renderer::draw(*node.sprite_batch);
			break;
		default:
			break;
	}
	for (auto child : node.children)
		draw(*child);
}

void Renderer::draw_elements(const SpriteVertex *vertices,
                             const unsigned int count)
{
	VertexArray::unbind();
	glVertexAttribPointer(
	    Shader::kAttributeColor, 4, GL_UNSIGNED_BYTE, GL_TRUE,
	    sizeof(SpriteVertex), &vertices->color);
	glVertexAttribPointer(
	    Shader::kAttributeTexCoord, 2, GL_FLOAT, GL_FALSE,
	    sizeof(SpriteVertex), &vertices->texcoord);
	glVertexAttribPointer(
	    Shader::kAttributeVertex, 2, GL_FLOAT, GL_TRUE,
	    sizeof(SpriteVertex), &vertices->position);

	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, nullptr);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to draw elements");
}

Renderer::Renderer()
    : zoom_mode(ZoomMode::LetterBox), index_buffer(0), scale(1.0f, 1.0f) { }

Renderer::~Renderer()
{
	if (this->index_buffer)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &this->index_buffer);
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

	glEnable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	this->shader_manager.init();
	if (!ShaderManager::Instance)
		return false;

	const unsigned short kDefaultIndices[] = { S256(0) };
	static_assert(sizeof(kDefaultIndices) ==
	                  kNumSprites * 6 * sizeof(kDefaultIndices[0]),
	              "Number of indices do not match set number of sprites");
	glGenBuffers(1, &this->index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kDefaultIndices),
	             kDefaultIndices, GL_STATIC_DRAW);

	return glGetError() == GL_NO_ERROR;
}

void Renderer::set_resolution(const Vec2i &resolution)
{
	R_ASSERT(ShaderManager::Instance,
	         "Cannot set resolution with an uninitialised renderer");

	this->view = resolution;
	ShaderManager::Instance->set(resolution);
	if (this->window.is_zero())
		this->window = this->view;
	this->set_window_size(this->window);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to set resolution");
}

void Renderer::set_window_size(const Vec2i &size)
{
	R_ASSERT(ShaderManager::Instance,
	         "Cannot set window size with an uninitialised renderer");

	if (this->zoom_mode == ZoomMode::Stretch || size == this->view)
	{
		this->origin.x = 0;
		this->origin.y = 0;
		this->window = size;
	}
	else
	{
		const Vec2f ratio(size.width / static_cast<float>(this->view.width),
		                  size.height / static_cast<float>(this->view.height));
		if ((this->zoom_mode == ZoomMode::Zoom && ratio.x < ratio.y)
		    || (this->zoom_mode == ZoomMode::LetterBox && ratio.x > ratio.y))
		{
			this->window.width = this->view.width * ratio.y;
			this->window.height = size.height;
			this->origin.x = (size.width - this->window.width) / 2;
			this->origin.y = 0;
		}
		else
		{
			this->window.width = size.width;
			this->window.height = this->view.height * ratio.x;
			this->origin.x = 0;
			this->origin.y = (size.height - this->window.height) / 2;
		}
	}
	glViewport(this->origin.x, this->origin.y,
	           this->window.width, this->window.height);
	this->scale.x = static_cast<float>(this->view.width) / this->window.width;
	this->scale.y = static_cast<float>(this->view.height) / this->window.height;
	this->window.height = size.height;
}

void Renderer::set_zoom_mode(const ZoomMode zoom)
{
	if (zoom == this->zoom_mode)
		return;

	this->zoom_mode = zoom;
	this->set_window_size(this->window);
}

Vec2i Renderer::convert_to_flipped_view(const Vec2i &p) const
{
	return this->convert_to_view(Vec2i(p.x, this->window.height - p.y));
}

Vec2i Renderer::convert_to_view(const Vec2i &p) const
{
	return Vec2i((p.x - this->origin.x) * this->scale.x,
	             (p.y - this->origin.y) * this->scale.y);
}
