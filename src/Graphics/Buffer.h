// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_BUFFER_H_
#define GRAPHICS_BUFFER_H_

#include <cstddef>

namespace rainbow { struct ISolemnlySwearThatIAmOnlyTesting; }

class Buffer
{
public:
	Buffer();
	Buffer(Buffer&&);
	Buffer(const rainbow::ISolemnlySwearThatIAmOnlyTesting &);
	~Buffer();

	/// <summary>
	///  Used by Label and SpriteBatch for interleaved vertex buffer.
	/// </summary>
	void bind() const;

	/// <summary>Used by SpriteBatch for normal buffers.</summary>
	void bind(const unsigned int index) const;

	/// <summary>
	///   Uploads <paramref name="data"/> of size <paramref name="size"/> to the
	///   GPU buffer.
	/// </summary>
	void upload(const void *const data, const size_t size) const;

private:
	unsigned int id_;
};

#endif
