/// Includes correct OpenGL ES headers for corresponding platforms.

/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef OPENGL_H_
#define OPENGL_H_

#include "Platform.h"

#if defined(RAINBOW_ANDROID)
#	include <GLES/gl.h>
#elif defined(RAINBOW_IOS)
#	include <OpenGLES/ES1/gl.h>
#elif defined(RAINBOW_WIN)
#	define WIN32
#	include <GL/GLee.h>
#else
#	define GL_GLEXT_PROTOTYPES 1
#	include <GL/gl.h>
#endif

#ifdef GL_VERSION_ES_CM_1_1
#	define GL_COORD_REPLACE  GL_COORD_REPLACE_OES
#	define GL_POINT_SPRITE   GL_POINT_SPRITE_OES
#endif

#endif
