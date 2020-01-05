// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_BUFFER_H_
#define GRAPHICS_BUFFER_H_

#include <cstddef>

namespace rainbow
{
    struct ISolemnlySwearThatIAmOnlyTesting;
}

namespace rainbow::graphics
{
    class Buffer
    {
    public:
        Buffer();
        Buffer(Buffer&&) noexcept;
        ~Buffer();

        /// <summary>
        ///   Used by Label and SpriteBatch for interleaved vertex buffer.
        /// </summary>
        void bind() const;

        /// <summary>Used by SpriteBatch for normal buffers.</summary>
        void bind(unsigned int index) const;

        /// <summary>
        ///   Uploads <paramref name="data"/> of size <paramref name="size"/> to
        ///   the GPU buffer.
        /// </summary>
        void upload(const void* data, size_t size) const;

#ifdef RAINBOW_TEST
        explicit Buffer(const ISolemnlySwearThatIAmOnlyTesting&) : id_(0) {}
#endif

    private:
        unsigned int id_;
    };
}  // namespace rainbow::graphics

#endif
