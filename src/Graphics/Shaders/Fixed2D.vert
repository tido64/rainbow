// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#version 460

layout(binding = 0) uniform MatrixBlock
{
    mat4 modelViewProjection;
};

layout(location = 0) in uvec4 color;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec2 position;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main()
{
    fragColor = color / 255.0;
    fragTexCoord = texCoord;
    gl_Position = modelViewProjection * vec4(position, 0.0, 1.0);
}
