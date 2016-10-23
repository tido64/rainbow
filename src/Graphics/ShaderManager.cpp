// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/ShaderManager.h"

#include <memory>

#include "Common/Data.h"
#include "FileSystem/FileSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shaders.h"

// For platforms not using GLEW.
#ifndef GLAPIENTRY
#define GLAPIENTRY
#endif

namespace
{
    using String = std::unique_ptr<char[]>;

    const Shader::AttributeParams kAttributeDefaultParams[]{
        {Shader::kAttributeVertex, "vertex"},
        {Shader::kAttributeColor, "color"},
        {Shader::kAttributeTexCoord, "texcoord"},
        {Shader::kAttributeNone, nullptr}};

    template <typename F, typename G>
    auto verify(GLuint id, GLenum pname, F&& glGetiv, G&& glGetInfoLog)
        -> String
    {
        GLint status = GL_FALSE;
        glGetiv(id, pname, &status);
        if (status == GL_FALSE)
        {
            GLint info_len = 0;
            glGetiv(id, GL_INFO_LOG_LENGTH, &info_len);
            if (info_len == 0)
                return {};

            auto log = std::make_unique<char[]>(info_len);
            glGetInfoLog(id, info_len, nullptr, log.get());
            return log;
        }

        return {};
    }

    auto compile_shader(const Shader::Params& shader) -> unsigned int
    {
        const GLuint id = glCreateShader(shader.type);
        std::error_code error_code;
        if (rainbow::filesystem::is_regular_file(
                rainbow::filesystem::relative(shader.source), error_code))
        {
            const Data& glsl = Data::load_asset(shader.source);
            if (!glsl)
            {
                LOGE("Failed to load shader: %s", shader.source);
                if (!shader.fallback)
                {
                    R_ABORT("No fallback was found");
                    return ShaderManager::kInvalidProgram;
                }
                // Local variable required for Android compiler (NDK r10e).
                const char* source = shader.fallback;
                glShaderSource(id, 1, &source, nullptr);
            }
            else
            {
                const char* source = glsl;
                glShaderSource(id, 1, &source, nullptr);
            }
        }
        else
        {
            // Local variable required for Android compiler (NDK r10e).
            const char* source = shader.fallback;
            glShaderSource(id, 1, &source, nullptr);
        }
        glCompileShader(id);

        const String& error =
            verify(id, GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog);
        if (error.get())
        {
            glDeleteShader(id);
            R_ABORT(
                "GLSL: Failed to compile %s shader: %s",
                (shader.type == Shader::kTypeVertex ? "vertex" : "fragment"),
                error.get());
            UNREACHABLE();
            return ShaderManager::kInvalidProgram;
        }

        return id;
    }

    auto link_program(const Shader::Params* shaders,
                      const Shader::AttributeParams* attributes) -> unsigned int
    {
        const GLuint program = glCreateProgram();
        for (auto shader = shaders; shader->type != Shader::kTypeInvalid;
             ++shader)
        {
            glAttachShader(program, shader->id);
        }
        for (auto attrib = attributes; attrib->name; ++attrib)
            glBindAttribLocation(program, attrib->index, attrib->name);
        glLinkProgram(program);

        const String& error = verify(
            program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);
        if (error.get())
        {
            glDeleteProgram(program);
            R_ABORT("GLSL: Failed to link program: %s", error.get());
            UNREACHABLE();
            return ShaderManager::kInvalidProgram;
        }

        return program;
    }
}

ShaderManager::~ShaderManager()
{
    for (const auto& details : programs_)
        glDeleteProgram(details.program);
    for (const auto shader : shaders_)
        glDeleteShader(shader);
}

bool ShaderManager::init()
{
    Shader::Params shaders[]{
        {Shader::kTypeVertex, 0, rainbow::shaders::kFixed2Dv,
         rainbow::shaders::integrated::kFixed2Dv},
        {Shader::kTypeFragment, 0, rainbow::shaders::kFixed2Df,
         rainbow::shaders::integrated::kFixed2Df},
        {Shader::kTypeInvalid, 0, nullptr, nullptr}};
    const auto pid = compile(shaders, nullptr);

    R_ASSERT(pid != kInvalidProgram, "Failed to compile default shader");
    NOT_USED(pid);

    make_global();
    return true;
}

auto ShaderManager::compile(Shader::Params* shaders,
                            const Shader::AttributeParams* attributes)
    -> unsigned int
{
    for (auto shader = shaders; shader->type != Shader::kTypeInvalid; ++shader)
    {
        if (!shader->source)
        {
            shader->id = shaders_[shader->id];
            continue;
        }
        if (shader->id > 0)
            continue;
        const unsigned int id = compile_shader(*shader);
        if (id == kInvalidProgram)
            return id;
        shaders_.push_back(id);
        shader->id = id;
    }
    if (!attributes)
        attributes = kAttributeDefaultParams;

    const unsigned int program = link_program(shaders, attributes);
    if (program == kInvalidProgram)
        return program;

    programs_.emplace_back(program,
                           glGetUniformLocation(program, "mvp_matrix"));
    return static_cast<unsigned int>(programs_.size());
}

void ShaderManager::update_projection()
{
    R_ASSERT(get_program().mvp_matrix >= 0,
             "Shader is missing a projection matrix");

    // The orthographic projection matrix is defined as:
    //
    //   | 2 / (r - l)       0             0       -(r + l) / (r - l) |
    //   |      0       2 / (t - b)        0       -(t + b) / (t - b) |
    //   |      0            0       -2 / (f - n)  -(f + n) / (f - n) |
    //   |      0            0             0                1         |
    //
    // Where <c>b</c> = bottom, <c>f</c> = far, <c>l</c> = left, <c>n</c> =
    // near, <c>r</c> = right, <c>t</c> = top, and near = -1.0 and far = 1.0.
    // The matrix is stored in column-major order.
    const auto& rect = rainbow::graphics::projection();
    const float projection[]{
        2.0f / (rect.right - rect.left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (rect.top - rect.bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -(rect.right + rect.left) / (rect.right - rect.left),
        -(rect.top + rect.bottom) / (rect.top - rect.bottom),
        0.0f, 1.0f};
    glUniformMatrix4fv(get_program().mvp_matrix, 1, GL_FALSE, projection);
}

void ShaderManager::update_viewport()
{
    if (current_ == kInvalidProgram)
    {
        current_ = kDefaultProgram;
        const Shader::Details& details = get_program();
        glUseProgram(details.program);
        update_projection();
        glUniform1i(glGetUniformLocation(details.program, "texture"), 0);
        return;
    }
    update_projection();
}

void ShaderManager::use(unsigned int program)
{
    if (program != current_)
    {
#ifndef USE_VERTEX_ARRAY_OBJECT
        const Shader::Details& current = get_program();
#endif  // !USE_VERTEX_ARRAY_OBJECT
        current_ = program;

        if (current_ == kInvalidProgram)
            return;

        const Shader::Details& details = get_program();
        glUseProgram(details.program);

        update_projection();

#ifndef USE_VERTEX_ARRAY_OBJECT
        if (details.texture0 != current.texture0)
        {
            if (!details.texture0)
                glDisableVertexAttribArray(Shader::kAttributeTexCoord);
            else
                glEnableVertexAttribArray(Shader::kAttributeTexCoord);
        }
#endif  // !USE_VERTEX_ARRAY_OBJECT
    }
}
