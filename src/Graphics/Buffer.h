// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_BUFFER_H_
#define GRAPHICS_BUFFER_H_

#include <cstddef>

namespace rainbow { struct ISolemnlySwearThatIAmOnlyTesting; }

namespace rainbow { namespace graphics
{
    class Buffer
    {
    public:
        Buffer();
        explicit Buffer(const rainbow::ISolemnlySwearThatIAmOnlyTesting&);
        Buffer(Buffer&&);
        ~Buffer();

        /// <summary>
        ///  Used by Label and SpriteBatch for interleaved vertex buffer.
        /// </summary>
        void bind() const;

        /// <summary>Used by SpriteBatch for normal buffers.</summary>
        void bind(unsigned int index) const;

        /// <summary>
        ///   Uploads <paramref name="data"/> of size <paramref name="size"/> to
        ///   the GPU buffer.
        /// </summary>
        void upload(const void* data, size_t size) const;

    private:
        unsigned int id_;
    };
}}  // namespace rainbow::graphics

#endif
