// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_VERTEXARRAY_H_
#define GRAPHICS_VERTEXARRAY_H_

#include <functional>

#include "Common/NonCopyable.h"
#include "Graphics/OpenGL.h"

namespace rainbow::graphics
{
    /// <summary>
    ///   Manages a vertex array object for any drawable type. On Android,
    ///   vertex array objects are emulated.
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
        template <typename F>
        void reconfigure(F&& array_state)
        {
#ifdef USE_VERTEX_ARRAY_OBJECT
            GLuint array = init_state();
            array_state();
            glBindVertexArray(0);
            if (array_ != 0)
                glDeleteVertexArrays(1, &array_);
            array_ = array;
#else
            array_ = std::forward<F>(array_state);
#endif
        }

        /// <summary>
        ///   Returns whether this vertex array object is valid.
        /// </summary>
        explicit operator bool() const
        {
            return static_cast<bool>(array_);
        }

    private:
#ifdef USE_VERTEX_ARRAY_OBJECT
        uint32_t array_ = 0;
#else
        std::function<void()> array_;
#endif

        auto init_state() const -> GLuint;
    };
}  // namespace rainbow::graphics

#endif
