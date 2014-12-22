// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <memory>

#include <Box2D/Common/b2Draw.h>

namespace b2
{
	class StableWorld;

	class DebugDraw : public b2Draw
	{
	public:
		static void Draw();

		DebugDraw();

		void Add(StableWorld *);
		void Remove(StableWorld *);

		void DrawAllWorlds();

		// Implement b2Draw

		void DrawPolygon(const b2Vec2 *vertices,
		                 int32 vertex_count,
		                 const b2Color &color) override;
		void DrawSolidPolygon(const b2Vec2 *vertices,
		                      int32 vertex_count,
		                      const b2Color &color) override;
		void DrawCircle(const b2Vec2 &center,
		                float32 radius,
		                const b2Color &color) override;
		void DrawSolidCircle(const b2Vec2 &center,
		                     float32 radius,
		                     const b2Vec2 &axis,
		                     const b2Color &color) override;
		void DrawSegment(const b2Vec2 &p1,
		                 const b2Vec2 &p2,
		                 const b2Color &color) override;
		void DrawTransform(const b2Transform &xf) override;

	private:
		static const size_t kMaxNumWorlds = 6;

		struct Vertex
		{
			b2Color color;
			b2Vec2 vertex;
		};

		std::unique_ptr<Vertex[]> vertices_;
		int32 buffer_size_;
		float32 ptm_;
		StableWorld *worlds_[kMaxNumWorlds];

		void UpdateBuffer(const b2Vec2 *vertices,
		                  const int32 vertex_count,
		                  const b2Color &color);
		void UpdateCircleBuffer(const b2Vec2 &center,
		                        const float32 radius,
		                        const b2Color &color);
	};
}
