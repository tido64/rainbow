// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_BUFFER_H_
#define GRAPHICS_BUFFER_H_

#include <cstddef>

class Buffer
{
public:
	Buffer();
	~Buffer();

	/// Used by Label and SpriteBatch for interleaved vertex buffer.
	void bind() const;

	/// Used by SpriteBatch for normal buffers.
	void bind(const unsigned int index) const;

	/// Uploads \p data of size \p size to the GPU buffer.
	void upload(const void *const data, const size_t size) const;

private:
	unsigned int id_;
};

#endif
