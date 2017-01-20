// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_VERTEXARRAY_H_
#define GRAPHICS_VERTEXARRAY_H_

#include <functional>

#include "Common/NonCopyable.h"
#include "Graphics/OpenGL.h"

namespace rainbow { namespace graphics
{
    /// <summary>
    ///   Manages a vertex array object for any drawable type. On Android, vertex
    ///   array objects are emulated.
    /// </summary>
    class VertexArray : NonCopyable<VertexArray>
    {
    public:
        /// <summary>Unbinds any bound vertex array object.</summary>
        static void unbind();

        VertexArray() = default;
        VertexArray(VertexArray&&) noexcept;
        ~VertexArray();

        /// <summary>Binds this vertex array object.</summary>
        void bind() const;

        /// <summary>
        ///   Reconfigures this vertex array object with a new set of states.
        /// </summary>
        void reconfigure(std::function<void()>&& array_state);

        /// <summary>
        ///   Returns whether this vertex array object is valid.
        /// </summary>
        explicit operator bool() const
        {
            return static_cast<const bool>(array_);
        }

    private:
#ifdef USE_VERTEX_ARRAY_OBJECT
        uint32_t array_ = 0;
#else
        std::function<void()> array_;
#endif
    };
}}  // namespace rainbow::graphics

#endif
