// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#version 100

#ifdef GL_ES
#	ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#	else
precision mediump float;
#	endif
#else
#	define lowp
#endif

varying lowp vec4 v_color;

void main()
{
	gl_FragColor = v_color;
}
