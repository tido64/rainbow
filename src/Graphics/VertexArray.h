// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_VERTEXARRAY_H_
#define GRAPHICS_VERTEXARRAY_H_

namespace Renderer
{
	/// Implements the equivalent of OpenGL's vertex array object.
	class VertexArray
	{
	public:
		static void bind(const VertexArray &array);
		static void unbind();

		unsigned int count;  ///< Vertex count.

		VertexArray();
		~VertexArray();

		void update(void *data, const size_t size) const;

	private:
		unsigned int array;   ///< GL vertex array object.
		unsigned int buffer;  ///< GL vertex buffer.
	};
}

#endif
