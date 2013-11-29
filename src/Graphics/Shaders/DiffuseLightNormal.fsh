// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef GL_ES
#	ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#	else
precision mediump float;
#	endif
#else
#	define lowp
#endif

uniform float cutoff;  ///< Max distance from light source.
uniform float radius;  ///< Radius of max light intensity.
uniform vec3 light;    ///< Light source's screen position.
uniform sampler2D normal;
uniform sampler2D texture;

varying lowp vec4 v_color;
varying vec2 v_normal;
varying vec2 v_texcoord;

// See: http://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
//      http://imdoingitwrong.wordpress.com/2011/02/10/improved-light-attenuation/
//      https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson6
//      http://www.alkemi-games.com/a-game-of-tricks/
void main()
{
	// Calculate distance from light source to point on plane.
	vec3 L = light - gl_FragCoord.xyz;
	float distance = length(L);
	float d = max(distance - radius, 0.0);
	L /= distance;

	// Transform distance for smoother cutoff.
	// Formula: d' = d / (1 - (d / dmax)^2)
	float f = d / cutoff;
	d /= 1.0 - f * f;

	// Calculate attenuation.
	// Formula: att = 1 / (d' / r + 1)^2
	f = d / radius + 1.0;
	float attenuation = 1.0 / (f * f);

	// fragment = diffuse color * diffuse * attenuation
	gl_FragColor = texture2D(texture, v_texcoord) * v_color
	             * max(dot(L, texture2D(normal, v_normal).xyz * 2.0 - 1.0), 0.0)
	             * attenuation;
}
