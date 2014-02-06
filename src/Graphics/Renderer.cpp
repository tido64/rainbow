// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Label.h"
#include "Graphics/Renderer.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/SpriteBatch.h"

#define S4(i)       S1((i)),   S1((i) +   1),   S1((i) +   2),   S1((i) +   3)
#define S16(i)      S4((i)),   S4((i) +   4),   S4((i) +   8),   S4((i) +  12)
#define S64(i)     S16((i)),  S16((i) +  16),  S16((i) +  32),  S16((i) +  48)
#define S256(i)    S64((i)),  S64((i) +  64),  S64((i) + 128),  S64((i) + 192)
#define S1024(i)  S256((i)), S256((i) + 256), S256((i) + 512), S256((i) + 768)

namespace
{
	// TODO: REFACTOR!

	int g_origin_x = 0;
	int g_origin_y = 0;
	int g_view_width = 0;
	int g_view_height = 0;
	int g_window_width = 0;
	int g_window_height = 0;
	float g_scale_x = 1.0f;
	float g_scale_y = 1.0f;
	Renderer::ZoomMode g_zoom_mode = Renderer::ZoomMode::LetterBox;
}

namespace Renderer
{
	extern const size_t kNumSprites = 256;  ///< Hard-coded limit on number of sprites.
	unsigned int g_index_buffer = 0;        ///< Index buffer object.

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

		ShaderManager::Instance = new ShaderManager();
		if (!*ShaderManager::Instance)
		{
			delete ShaderManager::Instance;
			return false;
		}

		TextureManager::Instance = new TextureManager();

		const unsigned short kDefaultIndices[] = {
		#define S0(i) (i) * 4
		#define S1(i) S0(i), S0(i) + 1, S0(i) + 2, S0(i) + 2, S0(i) + 3, S0(i)
			S256(0)
		#undef S1
		#undef S0
		};
		static_assert(sizeof(kDefaultIndices) ==
		                  kNumSprites * 6 * sizeof(kDefaultIndices[0]),
		              "Number of indices do not match set number of sprites");
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
		delete ShaderManager::Instance;
		delete TextureManager::Instance;
	}

	Vec2i resolution()
	{
		return Vec2i(g_view_width, g_view_height);
	}

	Vec2i window_size()
	{
		return Vec2i(g_window_width, g_window_height);
	}

	void set_resolution(const int width, const int height)
	{
		R_ASSERT(ShaderManager::Instance,
		         "Cannot set resolution with an uninitialized renderer");

		g_view_width = width;
		g_view_height = height;
		ShaderManager::Instance->set(width, height);
		if ((g_window_width | g_window_height) == 0)
		{
			g_window_width = g_view_width;
			g_window_height = g_view_height;
		}
		set_window_size(g_window_width, g_window_height);

		R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to set resolution");
	}

	void set_window_size(const int width, const int height)
	{
		R_ASSERT(ShaderManager::Instance,
		         "Cannot set window size with an uninitialized renderer");

		if (g_zoom_mode == ZoomMode::Stretch
		    || (width == g_view_width && height == g_view_height))
		{
			g_origin_x = 0;
			g_origin_y = 0;
			g_window_width = width;
			g_window_height = height;
		}
		else
		{
			const Vec2f ratio(width / static_cast<float>(g_view_width),
			                  height / static_cast<float>(g_view_height));
			if ((g_zoom_mode == ZoomMode::Zoom && ratio.x < ratio.y)
			    || (g_zoom_mode == ZoomMode::LetterBox && ratio.x > ratio.y))
			{
				g_window_width = g_view_width * ratio.y;
				g_window_height = height;
				g_origin_x = (width - g_window_width) / 2;
				g_origin_y = 0;
			}
			else
			{
				g_window_width = width;
				g_window_height = g_view_height * ratio.x;
				g_origin_x = 0;
				g_origin_y = (height - g_window_height) / 2;
			}
		}
		glViewport(g_origin_x, g_origin_y, g_window_width, g_window_height);
		g_scale_x = static_cast<float>(g_view_width) / g_window_width;
		g_scale_y = static_cast<float>(g_view_height) / g_window_height;
		g_window_height = height;
	}

	void set_zoom_mode(const ZoomMode zoom)
	{
		if (zoom == g_zoom_mode)
			return;

		g_zoom_mode = zoom;
		set_window_size(g_window_width, g_window_height);
	}

	void clear()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	Vec2i convert_to_flipped_view(const Vec2i &p)
	{
		return convert_to_view(Vec2i(p.x, g_window_height - p.y));
	}

	Vec2i convert_to_view(const Vec2i &p)
	{
		return Vec2i((p.x - g_origin_x) * g_scale_x,
		             (p.y - g_origin_y) * g_scale_y);
	}

	template<typename T>
	void draw(const T &obj)
	{
		obj.vertex_array().bind();
		glDrawElements(GL_TRIANGLES, obj.count(), GL_UNSIGNED_SHORT, nullptr);
	}

	void draw(const SceneGraph::Node &node)
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

	void draw_elements(const SpriteVertex *vertices, const unsigned int count)
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
}
