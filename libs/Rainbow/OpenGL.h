/// Includes correct OpenGL ES headers for corresponding platforms.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include <Rainbow/Hardware/Platform.h>

#if defined(ONWIRE_ANDROID)
#	include <GLES/gl.h>
#elif defined(ONWIRE_IOS)
#	include <OpenGLES/ES1/gl.h>
#endif
