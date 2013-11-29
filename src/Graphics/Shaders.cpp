// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/OpenGL.h"
#include "Graphics/Shaders.h"

namespace Rainbow
{
	namespace Shaders
	{
	#ifdef GL_ES_VERSION_2_0
	#	include "Graphics/Shaders/Shaders.h"
	#else
		const char *const kDiffuseLight2Df      = "Shaders/DiffuseLight2D.fsh";
		const char *const kDiffuseLightNormalf  = "Shaders/DiffuseLightNormal.fsh";
		const char *const kFixed2Df             = "Shaders/Fixed2D.fsh";
		const char *const kFixed2Dv             = "Shaders/Fixed2D.vsh";
		const char *const kNormalMappedv        = "Shaders/NormalMapped.vsh";
		const char *const kSimple2Dv            = "Shaders/Simple2D.vsh";
		const char *const kSimplef              = "Shaders/Simple.fsh";
	#endif
	}
}
