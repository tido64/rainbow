// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/DebugDraw.h"

#include <memory>

#include "Graphics/Shaders.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/VertexArray.h"

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
		if (g_debug_draw)
			g_debug_draw->DrawAllWorlds();
	}

	DebugDraw::DebugDraw() : ptm_(0)
	{
		SetFlags(b2Draw::e_shapeBit);
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

	void DebugDraw::Add(DebuggableWorld *world)
	{
		auto i = std::find(std::begin(worlds_), std::end(worlds_), nullptr);
		if (i == std::end(worlds_))
			return;
		*i = world;
	}

	void DebugDraw::Remove(DebuggableWorld *world)
	{
		std::fill(std::remove(std::begin(worlds_), std::end(worlds_), world),
		          std::end(worlds_),
		          nullptr);
	}

	void DebugDraw::DrawAllWorlds()
	{
		if (std::none_of(std::begin(worlds_),
		                 std::end(worlds_),
		                 [](const DebuggableWorld *world) { return world; }))
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

		upload(triangles_.size() * sizeof(triangles_[0]), triangles_.data());
		glDrawArrays(GL_TRIANGLES, 0, triangles_.size());
		triangles_.clear();

		upload(lines_.size() * sizeof(lines_[0]), lines_.data());
		glDrawArrays(GL_LINES, 0, lines_.size());
		lines_.clear();
	}

	void DebugDraw::DrawPolygon(const b2Vec2 *vertices,
	                            int32 vertex_count,
	                            const b2Color &color)
	{
		b2Vec2 p0 = ptm_ * vertices[vertex_count - 1];
		for (int32 i = 0; i < vertex_count; ++i)
		{
			const b2Vec2 p1 = ptm_ * vertices[i];
			lines_.emplace_back(color, p0);
			lines_.emplace_back(color, p1);
			p0 = p1;
		}
	}

	void DebugDraw::DrawSolidPolygon(const b2Vec2 *vertices,
	                                 int32 vertex_count,
	                                 const b2Color &color)
	{
		const b2Color c(color.r, color.g, color.b, 0.5f);
		for (int32 i = 1; i < vertex_count - 1; ++i)
		{
			triangles_.emplace_back(c, ptm_ * vertices[0]);
			triangles_.emplace_back(c, ptm_ * vertices[i]);
			triangles_.emplace_back(c, ptm_ * vertices[i + 1]);
		}
		DrawPolygon(vertices, vertex_count, color);
	}

	void DebugDraw::DrawCircle(const b2Vec2 &center,
	                           float32 radius,
	                           const b2Color &color)
	{
		const float32 sin_inc = sinf(kCircleIncrement);
		const float32 cos_inc = cosf(kCircleIncrement);
		b2Vec2 r1(1.0f, 0.0f);
		b2Vec2 v1 = center + radius * r1;
		for (int32 i = 0; i < kCircleSegments; ++i)
		{
			// Perform rotation to avoid additional trigonometry.
			const b2Vec2 r2(cos_inc * r1.x - sin_inc * r1.y,
			                sin_inc * r1.x + cos_inc * r1.y);
			const b2Vec2 v2 = center + radius * r2;
			lines_.emplace_back(color, ptm_ * v1);
			lines_.emplace_back(color, ptm_ * v2);
			r1 = r2;
			v1 = v2;
		}
	}

	void DebugDraw::DrawSolidCircle(const b2Vec2 &center,
	                                float32 radius,
	                                const b2Vec2 &axis,
	                                const b2Color &color)
	{
		const float32 sin_inc = sinf(kCircleIncrement);
		const float32 cos_inc = cosf(kCircleIncrement);
		b2Vec2 v0 = center;
		b2Vec2 r1(cos_inc, sin_inc);
		b2Vec2 v1 = center + radius * r1;
		const b2Color c(color.r, color.g, color.b, 0.5f);
		for (int32 i = 0; i < kCircleSegments; ++i)
		{
			// Perform rotation to avoid additional trigonometry.
			const b2Vec2 r2(cos_inc * r1.x - sin_inc * r1.y,
			                sin_inc * r1.x + cos_inc * r1.y);
			const b2Vec2 v2 = center + radius * r2;
			triangles_.emplace_back(c, ptm_ * v0);
			triangles_.emplace_back(c, ptm_ * v1);
			triangles_.emplace_back(c, ptm_ * v2);
			r1 = r2;
			v1 = v2;
		}

		DrawCircle(center, radius, color);

		// Draw the axis line
		DrawSegment(center, center + radius * axis, color);
	}

	void DebugDraw::DrawSegment(const b2Vec2 &p1,
	                            const b2Vec2 &p2,
	                            const b2Color &color)
	{
		lines_.emplace_back(color, ptm_ * p1);
		lines_.emplace_back(color, ptm_ * p2);
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
}
