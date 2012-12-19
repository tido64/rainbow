/// Includes correct OpenGL ES headers for corresponding platforms.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#ifndef OPENGL_H_
#define OPENGL_H_

#include "Platform.h"

#if defined(RAINBOW_ANDROID)
#	include <GLES2/gl2.h>
#	include <GLES2/gl2ext.h>
#	define GL_DRAW_FRAMEBUFFER GL_FRAMEBUFFER
#elif defined(RAINBOW_IOS)
#	include <OpenGLES/ES2/gl.h>
#	include <OpenGLES/ES2/glext.h>
#	define GL_DRAW_FRAMEBUFFER GL_DRAW_FRAMEBUFFER_APPLE
#elif defined(RAINBOW_MAC)
#	include <OpenGL/gl.h>
#	define GL_RGB565             GL_RGB
#	define glBindVertexArray     glBindVertexArrayAPPLE
#	define glDeleteVertexArrays  glDeleteVertexArraysAPPLE
#	define glGenVertexArrays     glGenVertexArraysAPPLE
#elif defined(RAINBOW_WIN)
#	ifndef WIN32
#		define WIN32 1
#	endif
#	define GLEW_NO_GLU
#	define GLEW_STATIC
#	include <GL/glew.h>
#	include <GL/glext.h>
#else
#	define GL_GLEXT_PROTOTYPES 1
#	include <GL/gl.h>
#	include <GL/glext.h>
#endif

#if defined(RAINBOW_ANDROID) || defined(RAINBOW_IOS)
#	define GL_RGB8               GL_RGB
#	define GL_RGBA8              GL_RGBA
#	define glBindVertexArray     glBindVertexArrayOES
#	define glDeleteVertexArrays  glDeleteVertexArraysOES
#	define glGenVertexArrays     glGenVertexArraysOES
#endif

#endif
