// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

uniform sampler2D texture;

varying lowp vec4 v_color;
varying vec2 v_texcoord;

void main()
{
    gl_FragColor = texture2D(texture, v_texcoord) * v_color;
}
