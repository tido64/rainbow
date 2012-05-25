/// Includes correct OpenGL ES headers for corresponding platforms.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#ifndef OPENGL_H_
#define OPENGL_H_

#include "Platform.h"

#if defined(RAINBOW_ANDROID)
#	include <GLES2/gl2.h>
#elif defined(RAINBOW_IOS)
#	include <OpenGLES/ES2/gl.h>
#	define glBindVertexArray     glBindVertexArrayOES
#	define glDeleteVertexArrays  glDeleteVertexArraysOES
#	define glGenVertexArrays     glGenVertexArraysOES
#elif defined(RAINBOW_WIN)
#	ifndef WIN32
#		define WIN32
#	endif
#	include <GL/GLee.h>
#	undef GL_LINES_ADJACENCY_EXT
#	undef GL_LINE_STRIP_ADJACENCY_EXT
#	undef GL_TRIANGLES_ADJACENCY_EXT
#	undef GL_TRIANGLE_STRIP_ADJACENCY_EXT
#	include <GL/glext.h>
#else
#	define GL_GLEXT_PROTOTYPES 1
#	include <GL/gl.h>
#	include <GL/glext.h>
#endif

#endif
