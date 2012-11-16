/// Copyright 2012 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

//#version 100

#ifdef GL_ES
#	ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#	else
precision mediump float;
#	endif
#else
#	define lowp
#endif

attribute vec4 color;
attribute vec2 vertex;

uniform mat4 mvp_matrix;

varying lowp vec4 v_color;

void main()
{
	v_color = color;
	gl_Position = vec4(vertex, 0.0, 1.0) * mvp_matrix;
}
