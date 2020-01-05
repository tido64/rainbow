// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Shaders/Diffuse.h"

#include "Graphics/Renderer.h"
#include "Graphics/Shaders.h"

using rainbow::graphics::ShaderManager;
using rainbow::shaders::Diffuse;

namespace gl = rainbow::graphics::gl;

Diffuse::Diffuse(ShaderManager& shader_manager, bool normal)
    : shader_manager_(shader_manager), cutoff_(0), radius_(0), position_(0),
      program_(ShaderManager::kInvalidProgram)
{
    if (normal)
    {
        Shader::Params shaders[]{
            gl::NormalMapped_vert(), gl::DiffuseLightNormal_frag()};
        const Shader::AttributeParams attributes[]{
            {Shader::kAttributeVertex, "vertex"},
            {Shader::kAttributeColor, "color"},
            {Shader::kAttributeTexCoord, "texcoord"},
            {Shader::kAttributeNormal, "normal"},
            {Shader::kAttributeNone, nullptr}};
        program_ = shader_manager_.compile(shaders, attributes);
    }
    else
    {
        Shader::Params shaders[]{
            {Shader::kTypeVertex, 0, nullptr, nullptr},  // kFixed2Dv
            gl::DiffuseLight2D_frag()};
        program_ = shader_manager_.compile(shaders, nullptr);
    }
    if (program_ == ShaderManager::kInvalidProgram)
        return;

    auto context = shader_manager_.use_scoped(program_);
    Shader::Details& details = shader_manager_.get_program(program_);
    cutoff_ = glGetUniformLocation(details.program, "cutoff");
    radius_ = glGetUniformLocation(details.program, "radius");
    position_ = glGetUniformLocation(details.program, "light");
    glUniform1i(glGetUniformLocation(details.program, "texture"), 0);
    if (normal)
    {
        glUniform1i(glGetUniformLocation(details.program, "normal"), 1);
        details.texture1 = true;
    }

    R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to load diffuse shader");
}

void Diffuse::set_cutoff(float cutoff) const
{
    auto context = shader_manager_.use_scoped(program_);
    glUniform1f(cutoff_, cutoff);
}

void Diffuse::set_radius(float radius) const
{
    auto context = shader_manager_.use_scoped(program_);
    glUniform1f(radius_, radius);
}

void Diffuse::set_position(float x, float y, float z) const
{
    auto context = shader_manager_.use_scoped(program_);
    auto p = graphics::convert_to_screen(
        shader_manager_.graphics_context(), Vec2i(x, y));
    glUniform3f(position_, p.x, p.y, z);
}
