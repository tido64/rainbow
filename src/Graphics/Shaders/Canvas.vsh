/// Copyright 2012 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

//#version 100

#ifdef GL_ES
#	ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#	else
precision mediump float;
#	endif
#endif

attribute vec2 texcoord;
attribute vec2 vertex;

uniform mat4 mvp_matrix;

varying vec2 v_texcoord;

void main()
{
	v_texcoord = texcoord;
	gl_Position = vec4(vertex, 0.0, 1.0) * mvp_matrix;
}
