// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Shaders.h"

#include "Graphics/OpenGL.h"

namespace rainbow
{
	namespace shaders
	{
#ifdef GL_ES_VERSION_2_0
#	include "Graphics/Shaders/Shaders.h"
#else
		const char kDiffuseLight2Df[]      = "Shaders/DiffuseLight2D.fsh";
		const char kDiffuseLightNormalf[]  = "Shaders/DiffuseLightNormal.fsh";
		const char kFixed2Df[]             = "Shaders/Fixed2D.fsh";
		const char kFixed2Dv[]             = "Shaders/Fixed2D.vsh";
		const char kNormalMappedv[]        = "Shaders/NormalMapped.vsh";
		const char kSimple2Dv[]            = "Shaders/Simple2D.vsh";
		const char kSimplef[]              = "Shaders/Simple.fsh";
#endif
	}
}
