// This file was auto-generated with 'tools/shaders-gen.py'.
// Copyright (c) 2016 Bifrost Entertainment AS and Tommy Nguyen.
// Distributed under the MIT License.

const char kDiffuseLight2Df[] =
R"(
#ifdef GL_ES
#	ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#	else
precision mediump float;
#	endif
#else
#	define lowp
#endif

uniform float cutoff;
uniform float radius;
uniform vec3 light;
uniform sampler2D texture;

varying lowp vec4 v_color;
varying vec2 v_texcoord;

void main()
{
	vec3 L = light - gl_FragCoord.xyz;
	float distance = length(L);
	float d = max(distance - radius, 0.0);
	L /= distance;

	float f = d / cutoff;
	d /= 1.0 - f * f;

	f = d / radius + 1.0;
	float attenuation = 1.0 / (f * f);

	gl_FragColor = texture2D(texture, v_texcoord) * v_color
	             * max(L.z, 0.0)
	             * attenuation;
}
)";

const char kDiffuseLightNormalf[] =
R"(
#ifdef GL_ES
#	ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#	else
precision mediump float;
#	endif
#else
#	define lowp
#endif

uniform float cutoff;
uniform float radius;
uniform vec3 light;
uniform sampler2D normal;
uniform sampler2D texture;

varying lowp vec4 v_color;
varying vec2 v_normal;
varying vec2 v_texcoord;

void main()
{
	vec3 L = light - gl_FragCoord.xyz;
	float distance = length(L);
	float d = max(distance - radius, 0.0);
	L /= distance;

	float f = d / cutoff;
	d /= 1.0 - f * f;

	f = d / radius + 1.0;
	float attenuation = 1.0 / (f * f);

	gl_FragColor = texture2D(texture, v_texcoord) * v_color
	             * max(dot(L, texture2D(normal, v_normal).xyz * 2.0 - 1.0), 0.0)
	             * attenuation;
}
)";

const char kFixed2Df[] =
R"(
#ifdef GL_ES
#	ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#	else
precision mediump float;
#	endif
#else
#	define lowp
#endif

uniform sampler2D texture;

varying lowp vec4 v_color;
varying vec2 v_texcoord;

void main()
{
	gl_FragColor = texture2D(texture, v_texcoord) * v_color;
}
)";

const char kFixed2Dv[] =
R"(
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
attribute vec2 texcoord;
attribute vec2 vertex;

uniform mat4 mvp_matrix;

varying lowp vec4 v_color;
varying vec2 v_texcoord;

void main()
{
	v_color = color;
	v_texcoord = texcoord;
	gl_Position = mvp_matrix * vec4(vertex, 0.0, 1.0);
}
)";

const char kNormalMappedv[] =
R"(
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
attribute vec2 normal;
attribute vec2 texcoord;
attribute vec2 vertex;

uniform mat4 mvp_matrix;

varying lowp vec4 v_color;
varying vec2 v_normal;
varying vec2 v_texcoord;

void main()
{
	v_color = color;
	v_normal = normal;
	v_texcoord = texcoord;
	gl_Position = mvp_matrix * vec4(vertex, 0.0, 1.0);
}
)";

const char kSimplef[] =
R"(
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
)";

const char kSimple2Dv[] =
R"(
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
	gl_Position = mvp_matrix * vec4(vertex, 0.0, 1.0);
}
)";
