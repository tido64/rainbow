/// Copyright 2012 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

//#version 100

#ifdef GL_ES
#	ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#	else
precision mediump float;
#	endif
precision lowp sampler2D;
#endif

uniform sampler2D canvas;
uniform sampler2D texture;

varying vec2 v_texcoord;

void main()
{
	gl_FragColor = texture2D(canvas, v_texcoord);
	gl_FragColor.a = max(0.0, gl_FragColor.a - texture2D(texture, v_texcoord).a);
}
