// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_ELEMENTBUFFER_H_
#define GRAPHICS_ELEMENTBUFFER_H_

#include <cstdlib>

namespace rainbow { namespace graphics
{
    class ElementBuffer
    {
    public:
        ~ElementBuffer();

        void bind() const;
        void upload(const void* data, size_t size) const;

        auto operator=(unsigned int buffer) -> ElementBuffer&
        {
            buffer_ = buffer;
            return *this;
        }

    private:
        unsigned int buffer_ = 0;
    };
}}  // namespace rainbow::graphics

#endif
