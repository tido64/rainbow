/// Copyright 2012 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#version 100

#ifdef GL_ES
#	if GL_FRAGMENT_PRECISION_HIGH
precision highp float;  // As recommended by Apple.
#	else
precision mediump float;
#	endif
#else
#	define lowp
#	define mediump
#endif

uniform lowp sampler2D canvas;
uniform lowp sampler2D texture;
uniform mediump vec2 dimension;
uniform lowp vec4 color;

varying lowp vec2 v_texcoord;

void main()
{
	vec4 brush = texture2D(texture, v_texcoord);
	gl_FragColor.rgb = brush.rgb * color.rgb;
	gl_FragColor.a = texture2D(canvas, gl_FragCoord.xy / dimension).a + brush.a;
}
