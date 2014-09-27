// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/ShaderManager.h"

#include <memory>

#include "Common/Data.h"
#include "Graphics/Shaders.h"

// For platforms not using GLEW.
#ifndef GLAPIENTRY
#	define GLAPIENTRY
#endif

using unique_str = std::unique_ptr<char[]>;

namespace
{
	const Shader::AttributeParams kAttributeDefaultParams[] = {
		{ Shader::kAttributeVertex, "vertex" },
		{ Shader::kAttributeColor, "color" },
		{ Shader::kAttributeTexCoord, "texcoord" },
		{ Shader::kAttributeNone, nullptr }
	};

	void set_projection_matrix(const Shader::Details &details,
	                           const std::array<float, 16> &ortho)
	{
		const int location = glGetUniformLocation(details.program, "mvp_matrix");
		R_ASSERT(location >= 0, "Shader is missing a projection matrix");
		glUniformMatrix4fv(location, 1, GL_FALSE, ortho.data());
	}

	unique_str verify(const GLuint id,
	                  const GLenum pname,
	                  void (GLAPIENTRY *glGetiv)(GLuint, GLenum, GLint*),
	                  void (GLAPIENTRY *glGetInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*))
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

	int compile_shader(const Shader::ShaderParams &shader)
	{
	#ifdef GL_ES_VERSION_2_0
		const char *source = shader.source;
	#else
		const Data &glsl = Data::load_asset(shader.source);
		if (!glsl)
		{
			R_ASSERT(glsl, "Failed to load shader");
			return -1;
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
			R_ERROR("[Rainbow] GLSL: Failed to compile %s shader: %s\n",
			        (shader.type == Shader::kTypeVertex) ? "vertex"
			                                             : "fragment",
			        error.get());
			glDeleteShader(id);
			return -1;
		}

		return id;
	}

	int link_program(const Shader::ShaderParams *shaders,
	                 const Shader::AttributeParams *attributes)
	{
		const GLuint program = glCreateProgram();
		for (auto shader = shaders; shader->type != Shader::kTypeInvalid; ++shader)
			glAttachShader(program, shader->id);
		for (auto attrib = attributes; attrib->name; ++attrib)
			glBindAttribLocation(program, attrib->index, attrib->name);
		glLinkProgram(program);

		const unique_str &error = verify(
		    program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);
		if (error.get())
		{
			R_ERROR("[Rainbow] GLSL: Failed to link program: %s\n",
			        error.get());
			glDeleteProgram(program);
			return -1;
		}

		return program;
	}
}

int ShaderManager::compile(Shader::ShaderParams *shaders,
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
		const int id = compile_shader(*shader);
		if (id < 0)
			return id;
		shaders_.push_back(id);
		shader->id = id;
	}
	if (!attributes)
		attributes = kAttributeDefaultParams;
	const int program = link_program(shaders, attributes);
	if (program < 0)
		return program;
	programs_.emplace_back(program);
	return programs_.size() - 1;
}

void ShaderManager::set(const Vec2i &resolution)
{
	ortho_[0] = 2.0f / resolution.width;
	ortho_[5] = 2.0f / resolution.height;
	if (active_ < 0)
	{
		active_ = 0;
		const Shader::Details &details = programs_[active_];
		glUseProgram(details.program);
		set_projection_matrix(details, ortho_);
		glUniform1i(glGetUniformLocation(programs_[0].program, "texture"), 0);
		return;
	}
	set_projection_matrix(programs_[active_], ortho_);
}

void ShaderManager::set_projection(const float left, const float right,
                                   const float bottom, const float top)
{
	ortho_[ 0] = 2.0f / (right - left);
	ortho_[ 5] = 2.0f / (top - bottom);
	ortho_[12] = -(right + left) / (right - left);
	ortho_[13] = -(top + bottom) / (top - bottom);
	set_projection_matrix(programs_[active_], ortho_);
}

void ShaderManager::use(const int program)
{
	if (program == active_)
		return;

	const Shader::Details &current = programs_[active_];
	active_ = program;
	const Shader::Details &details = programs_[active_];
	glUseProgram(details.program);

	set_projection_matrix(details, ortho_);

	if (details.texture0 != current.texture0)
	{
		if (!details.texture0)
			glDisableVertexAttribArray(Shader::kAttributeTexCoord);
		else
			glEnableVertexAttribArray(Shader::kAttributeTexCoord);
	}
}

ShaderManager::ShaderManager()
    : active_(-1), ortho_{{ 1.0f,  0.0f,  0.0f, 0.0f,
                            0.0f,  1.0f,  0.0f, 0.0f,
                            0.0f,  0.0f, -1.0f, 0.0f,
                           -1.0f, -1.0f,  0.0f, 1.0f }}
{ }

ShaderManager::~ShaderManager()
{
	if (active_ < 0)
		return;

	glUseProgram(0);
	for (const auto &details : programs_)
		glDeleteProgram(details.program);
	for (const auto shader : shaders_)
		glDeleteShader(shader);
}

bool ShaderManager::init()
{
	Shader::ShaderParams shaders[] = {
		{ Shader::kTypeVertex, 0, Rainbow::Shaders::kFixed2Dv },
		{ Shader::kTypeFragment, 0, Rainbow::Shaders::kFixed2Df },
		{ Shader::kTypeInvalid, 0, nullptr }
	};
	const int pid = compile(shaders, nullptr);
	if (pid < 0)
	{
		R_ASSERT(pid >= 0, "Failed to compile default shader");
		return false;
	}
	make_global();
	return true;
}
