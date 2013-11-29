// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SHADERDETAILS_H_
#define GRAPHICS_SHADERDETAILS_H_

#include "Graphics/OpenGL.h"

namespace Shader
{
	enum Attribute
	{
		VERTEX,
		COLOR,
		TEXCOORD
	};

	enum Type
	{
		FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
		VERTEX_SHADER = GL_VERTEX_SHADER,
	#ifdef GL_GEOMETRY_SHADER
		GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
	#endif
		INVALID = -1
	};

	struct Details
	{
		bool texture0;
		bool texture1;
		const unsigned int program;

		Details(const unsigned int program) :
			texture0(true), texture1(false), program(program) { }
	};
}

#endif
