/// Includes correct OpenGL ES headers for corresponding platforms.
///
/// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
/// Distributed under the MIT License.
/// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_OPENGL_H_
#define GRAPHICS_OPENGL_H_

#include "Platform/Macros.h"

#if defined(RAINBOW_OS_ANDROID) || defined(RAINBOW_WEB)
#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>
#elif defined(RAINBOW_OS_IOS)
#   include <OpenGLES/ES2/gl.h>
#   include <OpenGLES/ES2/glext.h>
#elif defined(RAINBOW_OS_MACOS)
#   include <OpenGL/gl.h>
#   define glBindVertexArray     glBindVertexArrayAPPLE
#   define glDeleteVertexArrays  glDeleteVertexArraysAPPLE
#   define glGenVertexArrays     glGenVertexArraysAPPLE
#elif defined(RAINBOW_OS_WINDOWS)
#   include <Windows.h>
#   define GLEW_NO_GLU
#   define GLEW_STATIC
#   include <GL/glew.h>
#   include <GL/glext.h>
#else
#   define GL_GLEXT_PROTOTYPES 1
#   include <GL/gl.h>
#   include <GL/glext.h>
#endif

#ifdef GL_ES_VERSION_2_0
#   define GL_RGBA8              GL_RGBA
#   define glBindVertexArray     glBindVertexArrayOES
#   define glDeleteVertexArrays  glDeleteVertexArraysOES
#   define glGenVertexArrays     glGenVertexArraysOES
#endif

#ifndef GL_TEXTURE_FREE_MEMORY_ATI
#   define GL_TEXTURE_FREE_MEMORY_ATI 0x87FC
#endif
#ifndef GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX
#   define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX 0x9048
#endif
#ifndef GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX
#   define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049
#endif

#if !defined(RAINBOW_OS_ANDROID) && !defined(RAINBOW_WEB)
#   define USE_VERTEX_ARRAY_OBJECT 1
#endif

#endif
