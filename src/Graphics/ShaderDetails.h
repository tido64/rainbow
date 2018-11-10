// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SHADERDETAILS_H_
#define GRAPHICS_SHADERDETAILS_H_

#include "Common/String.h"
#include "Graphics/OpenGL.h"

namespace Shader
{
    enum Attribute : unsigned int
    {
        kAttributeVertex,
        kAttributeColor,
        kAttributeTexCoord,
        kAttributeNormal,
        kAttributeNone
    };

    enum Type : unsigned int
    {
        kTypeFragment = GL_FRAGMENT_SHADER,
        kTypeVertex = GL_VERTEX_SHADER
#ifdef GL_GEOMETRY_SHADER
        , kTypeGeometry = GL_GEOMETRY_SHADER
#endif
    };

    struct AttributeParams
    {
        unsigned int index;
        rainbow::czstring name;
    };

    struct Details
    {
        bool texture0;
        bool texture1;
        const unsigned int program;
        const int mvp_matrix;

        Details(unsigned int program, int mvp_matrix)
            : texture0(true), texture1(false), program(program),
              mvp_matrix(mvp_matrix) {}
    };

    struct Params
    {
        Type type;
        unsigned int id;
        rainbow::czstring source;
        rainbow::czstring fallback;
    };
}

#endif
