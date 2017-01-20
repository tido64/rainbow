// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

//#version 100

#ifdef GL_ES
#   ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#   else
precision mediump float;
#   endif
#else
#   define lowp
#endif

uniform sampler2D texture;

varying lowp vec4 v_color;
varying vec2 v_texcoord;

void main()
{
    gl_FragColor = texture2D(texture, v_texcoord) * v_color;
}
