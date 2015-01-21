// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/DebugDraw.h"

#include "Graphics/Shaders.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/VertexArray.h"
#include "ThirdParty/Box2D/StableWorld.h"

namespace
{
	const float32 kAxisScale = 0.4f;
	const int32 kCircleSegments = 16;
	const float32 kCircleIncrement = 2.0f * b2_pi / kCircleSegments;

	unsigned int g_debug_draw_buffer = 0;
	unsigned int g_debug_draw_program = 0;
	VertexArray g_debug_draw_vao;

	void upload(const GLsizei size, const void *data)
	{
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STREAM_DRAW);
	}
}

namespace b2
{
	std::unique_ptr<b2::DebugDraw> g_debug_draw;

	void DebugDraw::Draw()
	{
		R_ASSERT(g_debug_draw.get(), "b2::DebugDraw uninitialised");

		g_debug_draw->DrawAllWorlds();
	}

	DebugDraw::DebugDraw() : buffer_size_(kCircleSegments), ptm_(0)
	{
		SetFlags(b2Draw::e_shapeBit);
		vertices_.reset(new Vertex[buffer_size_]);
		std::fill(std::begin(worlds_), std::end(worlds_), nullptr);

		if (g_debug_draw_program == 0)
		{
			Shader::Params shaders[] = {
			    {Shader::kTypeVertex, 0, rainbow::shaders::kSimple2Dv},
			    {Shader::kTypeFragment, 0, rainbow::shaders::kSimplef},
			    {Shader::kTypeInvalid, 0, nullptr}};
			const Shader::AttributeParams attributes[] = {
			    {Shader::kAttributeVertex, "vertex"},
			    {Shader::kAttributeColor, "color"},
			    {Shader::kAttributeNone, nullptr}};
			g_debug_draw_program =
			    ShaderManager::Get()->compile(shaders, attributes);
			Shader::Details &details =
			    ShaderManager::Get()->get_program(g_debug_draw_program);
			details.texture0 = false;

			glGenBuffers(1, &g_debug_draw_buffer);
			g_debug_draw_vao.reconfigure([] {
				glBindBuffer(GL_ARRAY_BUFFER, g_debug_draw_buffer);
				glEnableVertexAttribArray(Shader::kAttributeColor);
				glVertexAttribPointer(
				    Shader::kAttributeColor, 4, GL_FLOAT, GL_FALSE,
				    sizeof(Vertex),
				    reinterpret_cast<void*>(offsetof(Vertex, color)));
				glEnableVertexAttribArray(Shader::kAttributeVertex);
				glVertexAttribPointer(
				    Shader::kAttributeVertex, 2, GL_FLOAT, GL_TRUE,
				    sizeof(Vertex),
				    reinterpret_cast<void*>(offsetof(Vertex, vertex)));
			});
		}
		g_debug_draw.reset(this);
	}

	void DebugDraw::Add(StableWorld *world)
	{
		auto i = std::find(std::begin(worlds_), std::end(worlds_), nullptr);
		if (i == std::end(worlds_))
			return;
		*i = world;
	}

	void DebugDraw::Remove(StableWorld *world)
	{
		std::fill(std::remove(std::begin(worlds_), std::end(worlds_), world),
		          std::end(worlds_),
		          nullptr);
	}

	void DebugDraw::DrawAllWorlds()
	{
		if (std::none_of(std::begin(worlds_),
		                 std::end(worlds_),
		                 [](const StableWorld *world) { return world; }))
			return;

		ShaderManager::Context context;
		ShaderManager::Get()->use(g_debug_draw_program);
		g_debug_draw_vao.bind();
		glBindBuffer(GL_ARRAY_BUFFER, g_debug_draw_buffer);  // For uploading.
		for (auto world : worlds_)
		{
			if (!world)
				break;

			ptm_ = world->GetPTM();
			world->DrawDebugData();
		}
	}

	void DebugDraw::DrawPolygon(const b2Vec2 *vertices,
	                            int32 vertex_count,
	                            const b2Color &color)
	{
		UpdateBuffer(vertices, vertex_count, color);
		glDrawArrays(GL_LINE_LOOP, 0, vertex_count);
	}

	void DebugDraw::DrawSolidPolygon(const b2Vec2 *vertices,
	                                 int32 vertex_count,
	                                 const b2Color &color)
	{
		const b2Color c(color.r, color.g, color.b, 0.5f);
		UpdateBuffer(vertices, vertex_count, c);
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_count);

		DrawPolygon(vertices, vertex_count, color);
	}

	void DebugDraw::DrawCircle(const b2Vec2 &center,
	                           float32 radius,
	                           const b2Color &color)
	{
		UpdateCircleBuffer(center, radius, color);
		glDrawArrays(GL_TRIANGLE_FAN, 0, kCircleSegments);
	}

	void DebugDraw::DrawSolidCircle(const b2Vec2 &center,
	                                float32 radius,
	                                const b2Vec2 &axis,
	                                const b2Color &color)
	{
		DrawCircle(center, radius, b2Color(color.r, color.g, color.b, 0.5f));

		UpdateCircleBuffer(center, radius, color);
		glDrawArrays(GL_LINE_LOOP, 0, kCircleSegments);

		// Draw the axis line
		DrawSegment(center, center + radius * axis, color);
	}

	void DebugDraw::DrawSegment(const b2Vec2 &p1,
	                            const b2Vec2 &p2,
	                            const b2Color &color)
	{
		vertices_[0].color = color;
		vertices_[0].vertex = ptm_ * p1;
		vertices_[1].color = color;
		vertices_[1].vertex = ptm_ * p2;
		upload(2 * sizeof(vertices_[0]), vertices_.get());
		glDrawArrays(GL_LINES, 0, 2);
	}

	void DebugDraw::DrawTransform(const b2Transform &xf)
	{
		const b2Vec2 p1 = xf.p;
		b2Vec2 p2;

		p2 = p1 + kAxisScale * xf.q.GetXAxis();
		DrawSegment(p1, p2, b2Color(1.0f, 0.0f, 0.0f));

		p2 = p1 + kAxisScale * xf.q.GetYAxis();
		DrawSegment(p1, p2, b2Color(0.0f, 1.0f, 0.0f));
	}

	void DebugDraw::UpdateBuffer(const b2Vec2 *vertices,
	                             const int32 vertex_count,
	                             const b2Color &color)
	{
		if (vertex_count > buffer_size_)
		{
			vertices_.reset(new Vertex[vertex_count]);
			buffer_size_ = vertex_count;
		}
		for (int32 i = 0; i < vertex_count; ++i)
		{
			vertices_[i].color = color;
			vertices_[i].vertex = ptm_ * vertices[i];
		}
		upload(vertex_count * sizeof(vertices_[0]), vertices_.get());
	}

	void DebugDraw::UpdateCircleBuffer(const b2Vec2 &center,
	                                   const float32 radius,
	                                   const b2Color &color)
	{
		if (kCircleSegments > buffer_size_)
		{
			vertices_.reset(new Vertex[kCircleSegments]);
			buffer_size_ = kCircleSegments;
		}
		float32 theta = 0.0f;
		for (int32 i = 0; i < kCircleSegments; ++i)
		{
			vertices_[i].color = color;
			vertices_[i].vertex =
			    ptm_ * (center + radius * b2Vec2(cosf(theta), sinf(theta)));
			theta += kCircleIncrement;
		}
		upload(kCircleSegments * sizeof(vertices_[0]), vertices_.get());
	}
}
