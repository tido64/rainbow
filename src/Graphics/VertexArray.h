#ifndef VERTEXARRAY_H_
#define VERTEXARRAY_H_

namespace Renderer
{
	class VertexArray;

	/// Helper class for binding a VertexArray.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	struct BindVertexArray
	{
		BindVertexArray(const VertexArray &array);
		~BindVertexArray();
	};

	/// Implements the equivalent of OpenGL's vertex array object.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class VertexArray
	{
		friend struct BindVertexArray;

	public:
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
