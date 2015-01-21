// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/ShaderManager.h"

#include <memory>

#include "Common/Data.h"
#include "Graphics/Shaders.h"

// For platforms not using GLEW.
#ifndef GLAPIENTRY
#define GLAPIENTRY
#endif

using unique_str = std::unique_ptr<char[]>;

namespace
{
	const Shader::AttributeParams kAttributeDefaultParams[] = {
	    {Shader::kAttributeVertex, "vertex"},
	    {Shader::kAttributeColor, "color"},
	    {Shader::kAttributeTexCoord, "texcoord"},
	    {Shader::kAttributeNone, nullptr}};

	constexpr std::array<float, 16> kProjectionMatrix()
	{
		return {{1.0f,  0.0f,  0.0f, 0.0f,
		         0.0f,  1.0f,  0.0f, 0.0f,
		         0.0f,  0.0f, -1.0f, 0.0f,
		        -1.0f, -1.0f,  0.0f, 1.0f}};
	}

	void set_projection_matrix(const Shader::Details &details,
	                           const std::array<float, 16> &ortho)
	{
		const int location =
		    glGetUniformLocation(details.program, "mvp_matrix");
		R_ASSERT(location >= 0, "Shader is missing a projection matrix");
		glUniformMatrix4fv(location, 1, GL_FALSE, ortho.data());
	}

	template<typename F, typename G>
	unique_str verify(const GLuint id,
	                  const GLenum pname,
	                  F&& glGetiv,
	                  G&& glGetInfoLog)
	{
		GLint status = GL_FALSE;
		glGetiv(id, pname, &status);
		if (status == GL_FALSE)
		{
			GLint info_len = 0;
			glGetiv(id, GL_INFO_LOG_LENGTH, &info_len);
			if (info_len == 0)
				return unique_str();
			char *log = new char[info_len];
			glGetInfoLog(id, info_len, nullptr, log);
			log[info_len] = '\0';
			return unique_str(log);
		}
		return unique_str();
	}

	unsigned int compile_shader(const Shader::Params &shader)
	{
#ifdef GL_ES_VERSION_2_0
		const char *source = shader.source;
#else
		const Data &glsl = Data::load_asset(shader.source);
		if (!glsl)
		{
			R_ABORT("Failed to load shader");
			return ShaderManager::kInvalidProgram;
		}
		const char *source = glsl;
#endif

		const GLuint id = glCreateShader(shader.type);
		glShaderSource(id, 1, &source, nullptr);
		glCompileShader(id);

		const unique_str &error =
		    verify(id, GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog);
		if (error.get())
		{
			R_ABORT("GLSL: Failed to compile %s shader: %s",
			        shader.type == Shader::kTypeVertex ? "vertex" : "fragment",
			        error.get());
			glDeleteShader(id);
			return ShaderManager::kInvalidProgram;
		}

		return id;
	}

	unsigned int link_program(const Shader::Params *shaders,
	                          const Shader::AttributeParams *attributes)
	{
		const GLuint program = glCreateProgram();
		for (auto shader = shaders; shader->type != Shader::kTypeInvalid;
		     ++shader)
			glAttachShader(program, shader->id);
		for (auto attrib = attributes; attrib->name; ++attrib)
			glBindAttribLocation(program, attrib->index, attrib->name);
		glLinkProgram(program);

		const unique_str &error = verify(
		    program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);
		if (error.get())
		{
			R_ABORT("GLSL: Failed to link program: %s", error.get());
			glDeleteProgram(program);
			return ShaderManager::kInvalidProgram;
		}

		return program;
	}
}

unsigned int ShaderManager::compile(Shader::Params *shaders,
                                    const Shader::AttributeParams *attributes)
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
	programs_.emplace_back(program);
	return static_cast<unsigned int>(programs_.size());
}

void ShaderManager::set(const Vec2i &resolution)
{
	ortho_[0] = 2.0f / resolution.x;
	ortho_[5] = 2.0f / resolution.y;
	if (current_ == kInvalidProgram)
	{
		current_ = kDefaultProgram;
		const Shader::Details &details = get_program();
		glUseProgram(details.program);
		set_projection_matrix(details, ortho_);
		glUniform1i(glGetUniformLocation(details.program, "texture"), 0);
		return;
	}
	set_projection_matrix(get_program(), ortho_);
}

void ShaderManager::set_projection(const float left,
                                   const float right,
                                   const float bottom,
                                   const float top)
{
	ortho_[0] = 2.0f / (right - left);
	ortho_[5] = 2.0f / (top - bottom);
	ortho_[12] = -(right + left) / (right - left);
	ortho_[13] = -(top + bottom) / (top - bottom);
	set_projection_matrix(get_program(), ortho_);
}

void ShaderManager::use(const unsigned int program)
{
	if (program != current_)
	{
#ifndef USE_VERTEX_ARRAY_OBJECT
		const Shader::Details &current = get_program();
#endif  // !USE_VERTEX_ARRAY_OBJECT
		current_ = program;
		const Shader::Details &details = get_program();
		glUseProgram(details.program);

		set_projection_matrix(details, ortho_);

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

ShaderManager::ShaderManager()
    : current_(kInvalidProgram), ortho_(kProjectionMatrix()) {}

ShaderManager::~ShaderManager()
{
	for (const auto &details : programs_)
		glDeleteProgram(details.program);
	for (const auto shader : shaders_)
		glDeleteShader(shader);
}

bool ShaderManager::init()
{
	Shader::Params shaders[] = {
	    {Shader::kTypeVertex, 0, rainbow::shaders::kFixed2Dv},
	    {Shader::kTypeFragment, 0, rainbow::shaders::kFixed2Df},
	    {Shader::kTypeInvalid, 0, nullptr}};
	const unsigned int pid = compile(shaders, nullptr);
	if (pid == kInvalidProgram)
	{
		R_ABORT("Failed to compile default shader");
		return false;
	}
	make_global();
	return true;
}
