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

namespace Renderer
{
	extern const int kNumSprites = 256;  ///< Hard-coded limit on number of sprites.
	unsigned int g_index_buffer = 0;     ///< Index buffer object.

	namespace
	{
		/// Default vertex indices (currently limited to \c kNumSprites sprites).
		template<typename T>
		const T* default_indices(T *buffer)
		{
			int i = -1;
			for (int j = 0; j < kNumSprites * 4; j += 4)
			{
				buffer[++i] = j;
				buffer[++i] = j + 1;
				buffer[++i] = j + 2;
				buffer[++i] = j + 2;
				buffer[++i] = j + 3;
				buffer[++i] = j;
			}
			return buffer;
		}
	}

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

		unsigned short buffer[kNumSprites * 6];
		glGenBuffers(1, &g_index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(buffer),
		             default_indices(buffer), GL_STATIC_DRAW);

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
		label.font_->bind();
		draw(label.array_);
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
		batch.texture->bind();
		draw(batch.array);
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
