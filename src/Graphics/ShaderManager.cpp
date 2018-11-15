// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/ShaderManager.h"

#include <memory>

#include "Common/Data.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shaders.h"

using rainbow::czstring;
using rainbow::Data;
using rainbow::File;
using rainbow::FileType;
using rainbow::graphics::ShaderManager;

namespace
{
    const Shader::AttributeParams kAttributeDefaultParams[]{
        {Shader::kAttributeVertex, "vertex"},
        {Shader::kAttributeColor, "color"},
        {Shader::kAttributeTexCoord, "texcoord"},
        {Shader::kAttributeNone, nullptr}};

    void shader_source(GLuint shader, czstring string)
    {
#ifdef GL_ES_VERSION_2_0
        auto glsl_header = rainbow::gl::GLES2_header_glsl();
#else
        auto glsl_header = rainbow::gl::GL2_1_header_glsl();
#endif
        const czstring source[]{glsl_header, string};
        glShaderSource(shader, 2, source, nullptr);
    }

    template <typename F, typename G>
    auto verify(GLuint id, GLenum pname, F&& glGetiv, G&& glGetInfoLog)
        -> std::string
    {
        GLint status = GL_FALSE;
        glGetiv(id, pname, &status);
        if (status == GL_FALSE)
        {
            GLint info_len = 0;
            glGetiv(id, GL_INFO_LOG_LENGTH, &info_len);
            if (info_len == 0)
                return {};

            std::string info(info_len, '\0');
            glGetInfoLog(id, info_len, nullptr, info.data());
            return info;
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
            const Data& glsl = File::read(shader.source, FileType::Asset);
            if (!glsl)
            {
                LOGE("Failed to load shader: %s", shader.source);
                if (shader.fallback == nullptr)
                {
                    R_ABORT("No fallback was found");
                    return ShaderManager::kInvalidProgram;
                }

                shader_source(id, shader.fallback);
            }
            else
            {
                shader_source(id, glsl.as<char*>());
            }
        }
        else
        {
            shader_source(id, shader.fallback);
        }
        glCompileShader(id);

        const auto& error =
            verify(id, GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog);
        if (!error.empty())
        {
            glDeleteShader(id);
            R_ABORT(
                "GLSL: Failed to compile %s shader: %s",
                (shader.type == Shader::kTypeVertex ? "vertex" : "fragment"),
                error.c_str());
            UNREACHABLE();
            return ShaderManager::kInvalidProgram;
        }

        return id;
    }

    auto link_program(ArraySpan<Shader::Params> shaders,
                      const Shader::AttributeParams* attributes) -> unsigned int
    {
        const GLuint program = glCreateProgram();
        for (auto&& shader : shaders)
            glAttachShader(program, shader.id);
        for (auto attrib = attributes; attrib->name != nullptr; ++attrib)
            glBindAttribLocation(program, attrib->index, attrib->name);
        glLinkProgram(program);

        const auto& error = verify(
            program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);
        if (!error.empty())
        {
            glDeleteProgram(program);
            R_ABORT("GLSL: Failed to link program: %s", error.c_str());
            UNREACHABLE();
            return ShaderManager::kInvalidProgram;
        }

        return program;
    }
}  // namespace

ShaderManager::~ShaderManager()
{
    for (const auto& details : programs_)
        glDeleteProgram(details.program);
    for (const auto shader : shaders_)
        glDeleteShader(shader);
}

bool ShaderManager::init()
{
    Shader::Params shaders[]{gl::Fixed2D_vert(), gl::Fixed2D_frag()};
    [[maybe_unused]] const auto pid = compile(shaders, nullptr);

    R_ASSERT(pid != kInvalidProgram, "Failed to compile default shader");

    make_global();
    return true;
}

auto ShaderManager::compile(ArraySpan<Shader::Params> shaders,
                            const Shader::AttributeParams* attributes)
    -> unsigned int
{
    for (auto&& shader : shaders)
    {
        if (shader.source == nullptr)
        {
            shader.id = shaders_[shader.id];
            continue;
        }

        if (shader.id > 0)
            continue;

        const unsigned int id = compile_shader(shader);
        if (id == kInvalidProgram)
            return id;

        shaders_.push_back(id);
        shader.id = id;
    }
    if (attributes == nullptr)
    {
        attributes = static_cast<const Shader::AttributeParams*>(
            kAttributeDefaultParams);
    }

    const unsigned int program = link_program(shaders, attributes);
    if (program == kInvalidProgram)
        return program;

    programs_.emplace_back(
        program, glGetUniformLocation(program, "mvp_matrix"));
    return static_cast<unsigned int>(programs_.size());
}

void ShaderManager::update_projection()
{
    R_ASSERT(
        get_program().mvp_matrix >= 0, "Shader is missing a projection matrix");

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
    const auto& rect = context_->projection;
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
