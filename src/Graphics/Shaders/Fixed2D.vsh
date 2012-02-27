/// Copyright 2012 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

//#version 100

#ifndef GL_ES
#	define lowp
#endif

attribute vec4 color;
attribute vec2 texcoord;
attribute vec2 vertex;

uniform mat4 mvp_matrix;

varying lowp vec4 v_color;
varying lowp vec2 v_texcoord;

void main()
{
	v_color = color;
	v_texcoord = texcoord;
	gl_Position = vec4(vertex, 0.0, 1.0) * mvp_matrix;
}
