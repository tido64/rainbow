// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

uniform float cutoff;  ///< Max distance from light source.
uniform float radius;  ///< Radius of max light intensity.
uniform vec3 light;    ///< Light source's screen position.
uniform sampler2D texture;

varying lowp vec4 v_color;
varying vec2 v_texcoord;

// See: http://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
//      http://imdoingitwrong.wordpress.com/2011/02/10/improved-light-attenuation/
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

    // The normal of the plane is always vec3(0.0, 0.0, 1.0) since we're in 2D
    // space; skip calculating the dot product of L and N.
    gl_FragColor = texture2D(texture, v_texcoord) * v_color
                 * max(L.z, 0.0)
                 * attenuation;
}
