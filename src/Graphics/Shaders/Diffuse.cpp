// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Shaders/Diffuse.h"

#include "Graphics/Renderer.h"
#include "Graphics/Shaders.h"

using rainbow::graphics::ShaderManager;

namespace
{
    Shader::Params sDiffuseLight2Df{
        Shader::kTypeFragment, 0, rainbow::shaders::kDiffuseLight2Df,
        rainbow::shaders::integrated::kDiffuseLight2Df};
    Shader::Params sDiffuseLightNormalf{
        Shader::kTypeFragment, 0, rainbow::shaders::kDiffuseLightNormalf,
        rainbow::shaders::integrated::kDiffuseLightNormalf};
    Shader::Params sNormalMappedf{
        Shader::kTypeVertex, 0, rainbow::shaders::kNormalMappedv,
        rainbow::shaders::integrated::kNormalMappedv};
}

namespace rainbow { namespace shaders
{
    Diffuse::Diffuse(bool normal)
        : cutoff_(0), radius_(0), position_(0),
          program_(ShaderManager::kInvalidProgram)
    {
        if (normal)
        {
            Shader::Params shaders[]{
                sNormalMappedf,
                sDiffuseLightNormalf,
                {Shader::kTypeInvalid, 0, nullptr, nullptr}};
            const Shader::AttributeParams attributes[]{
                {Shader::kAttributeVertex, "vertex"},
                {Shader::kAttributeColor, "color"},
                {Shader::kAttributeTexCoord, "texcoord"},
                {Shader::kAttributeNormal, "normal"},
                {Shader::kAttributeNone, nullptr}};
            program_ = ShaderManager::Get()->compile(shaders, attributes);
        }
        else
        {
            Shader::Params shaders[]{
                {Shader::kTypeVertex, 0, nullptr, nullptr},  // kFixed2Dv
                sDiffuseLight2Df,
                {Shader::kTypeInvalid, 0, nullptr, nullptr}};
            program_ = ShaderManager::Get()->compile(shaders, nullptr);
        }
        if (program_ == ShaderManager::kInvalidProgram)
            return;

        ShaderManager::Context context;
        ShaderManager::Get()->use(program_);
        Shader::Details& details = ShaderManager::Get()->get_program(program_);
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
        ShaderManager::Context context;
        ShaderManager::Get()->use(program_);
        glUniform1f(cutoff_, cutoff);
    }

    void Diffuse::set_radius(float radius) const
    {
        ShaderManager::Context context;
        ShaderManager::Get()->use(program_);
        glUniform1f(radius_, radius);
    }

    void Diffuse::set_position(float x, float y, float z) const
    {
        ShaderManager::Context context;
        ShaderManager::Get()->use(program_);
        auto&& p = graphics::convert_to_screen(Vec2i(x, y));
        glUniform3f(position_, p.x, p.y, z);
    }
}}  // namespace rainbow::shaders
