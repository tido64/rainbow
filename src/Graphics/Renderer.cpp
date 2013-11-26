// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#include "Graphics/Label.h"
#include "Graphics/Renderer.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/SpriteBatch.h"

#ifdef GL_ES_VERSION_2_0
#	include "Graphics/Shaders/Shaders.h"
#else
#	define fixed2d_vsh "Shaders/Fixed2D.vsh"
#	define fixed2d_fsh "Shaders/Fixed2D.fsh"
#endif

#define S4(i)       S1((i))  //S1((i) +   1),   S1((i) +   2),   S1((i) +   3)
#define S16(i)      S4((i)),   S4((i) +   4),   S4((i) +   8),   S4((i) +  12)
#define S64(i)     S16((i)),  S16((i) +  16),  S16((i) +  32),  S16((i) +  48)
#define S256(i)    S64((i)),  S64((i) +  64),  S64((i) + 128),  S64((i) + 192)
#define S1024(i)  S256((i)), S256((i) + 256), S256((i) + 512), S256((i) + 768)

namespace Renderer
{
	extern const int kNumSprites = 256;  ///< Hard-coded limit on number of sprites.
	unsigned int g_index_buffer = 0;     ///< Index buffer object.

	bool init()
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

		const char *shaders[] = { fixed2d_vsh, fixed2d_fsh };
		ShaderManager::Instance = new ShaderManager(shaders, 2);
		if (!*ShaderManager::Instance)
		{
			delete ShaderManager::Instance;
			return false;
		}

		TextureManager::Instance = new TextureManager();

		const unsigned short kDefaultIndices[] = {
		#define S1(i) (i), (i) + 1, (i) + 2, (i) + 2, (i) + 3, (i)
			S256(0)
		#undef S1
		};
		glGenBuffers(1, &g_index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kDefaultIndices),
		             kDefaultIndices, GL_STATIC_DRAW);

		return glGetError() == GL_NO_ERROR;
	}

	void release()
	{
		if (g_index_buffer)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDeleteBuffers(1, &g_index_buffer);
		}
		glUseProgram(0);
		delete TextureManager::Instance;
		delete ShaderManager::Instance;
	}

	void clear()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void resize(const unsigned int width, const unsigned int height)
	{
		ShaderManager::Instance->set(width, height);
		ShaderManager::Instance->reset();
		glViewport(0, 0, width, height);

		R_ASSERT(glGetError() == GL_NO_ERROR,
		         "Failed to initialise OpenGL viewport");
	}

	void draw(const Label &label)
	{
		label.font().bind();
		draw(label.vertex_array());
	}

	void draw(const SceneGraph::Node &node)
	{
		if (!node.enabled)
			return;

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

	void draw(const SpriteBatch &batch)
	{
		batch.texture().bind();
		draw(batch.vertex_array());
	}

	void draw(const VertexArray &array)
	{
		BindVertexArray bind(array);
		glDrawElements(GL_TRIANGLES, array.count, GL_UNSIGNED_SHORT, nullptr);
		R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to draw buffer");
	}

	void draw_elements(const SpriteVertex *vertices, const unsigned int count)
	{
		glVertexAttribPointer(
				Shader::COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE,
				sizeof(SpriteVertex), &vertices->color);
		glVertexAttribPointer(
				Shader::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex),
				&vertices->texcoord);
		glVertexAttribPointer(
				Shader::VERTEX, 2, GL_FLOAT, GL_TRUE, sizeof(SpriteVertex),
				&vertices->position);

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, nullptr);

		R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to draw elements");
	}
}
