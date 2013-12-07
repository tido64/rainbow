#ifndef GRAPHICS_VERTEXARRAY_H_
#define GRAPHICS_VERTEXARRAY_H_

namespace Renderer
{
	class VertexArray;

	/// Implements the equivalent of OpenGL's vertex array object.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class VertexArray
	{
	public:
		/// Helper class for binding a VertexArray.
		///
		/// Copyright 2013 Bifrost Entertainment. All rights reserved.
		/// \author Tommy Nguyen
		struct Bind
		{
			Bind(const VertexArray &array);
			~Bind();
		};

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
