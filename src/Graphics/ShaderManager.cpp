// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <memory>

#include "Common/Data.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Shaders.h"

// For platforms not using GLEW.
#ifndef GLAPIENTRY
#	define GLAPIENTRY
#endif

typedef std::unique_ptr<char[]> unique_str;

namespace
{
	const Shader::AttributeParams kAttributeDefaultParams[] = {
		{ Shader::kAttributeVertex, "vertex" },
		{ Shader::kAttributeColor, "color" },
		{ Shader::kAttributeTexCoord, "texcoord" },
		{ Shader::kAttributeNone, nullptr }
	};

	void set_projection_matrix(const GLuint program, const GLfloat *ortho)
	{
		const int location = glGetUniformLocation(program, "mvp_matrix");
		R_ASSERT(location >= 0, "Shader is missing a projection matrix");
		glUniformMatrix4fv(location, 1, GL_FALSE, ortho);
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

		const unique_str &error =
		    verify(program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);
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

ShaderManager *ShaderManager::Instance = nullptr;

int ShaderManager::compile(Shader::ShaderParams *shaders,
                           const Shader::AttributeParams *attributes)
{
	for (auto shader = shaders; shader->type != Shader::kTypeInvalid; ++shader)
	{
		if (!shader->source)
		{
			shader->id = this->shaders[shader->id];
			continue;
		}
		if (shader->id > 0)
			continue;
		const int id = compile_shader(*shader);
		if (id < 0)
			return id;
		this->shaders.push_back(id);
		shader->id = id;
	}
	if (!attributes)
		attributes = kAttributeDefaultParams;
	const int program = link_program(shaders, attributes);
	if (program < 0)
		return program;
	this->programs.push_back(Shader::Details(program));
	return this->programs.size() - 1;
}

void ShaderManager::set(const float width, const float height)
{
	this->ortho[0] = 2.0f / width;
	this->ortho[5] = 2.0f / height;
	if (this->active < 0)
	{
		this->initialise();
		return;
	}
	set_projection_matrix(this->programs[this->active].program, this->ortho);
}

void ShaderManager::set_projection(const float left, const float right,
                                   const float bottom, const float top)
{
	this->ortho[0] = 2.0f / (right - left);
	this->ortho[3] = -(right + left) / (right - left);
	this->ortho[5] = 2.0f / (top - bottom);
	this->ortho[7] = -(top + bottom) / (top - bottom);
	set_projection_matrix(this->programs[this->active].program, this->ortho);
}

void ShaderManager::use(const int program)
{
	if (program == this->active)
		return;

	const Shader::Details &current = this->programs[this->active];
	this->active = program;
	const Shader::Details &details = this->programs[this->active];
	glUseProgram(details.program);

	set_projection_matrix(details.program, this->ortho);

	if (details.texture0 != current.texture0)
	{
		if (!details.texture0)
			glDisableVertexAttribArray(Shader::kAttributeTexCoord);
		else
			glEnableVertexAttribArray(Shader::kAttributeTexCoord);
	}
}

ShaderManager::ShaderManager() : active(-1)
{
	R_ASSERT(Instance == nullptr, "There can be only one ShaderManager");

	Shader::ShaderParams shaders[] = {
		{ Shader::kTypeVertex, 0, Rainbow::Shaders::kFixed2Dv },
		{ Shader::kTypeFragment, 0, Rainbow::Shaders::kFixed2Df },
		{ Shader::kTypeInvalid, 0, nullptr }
	};
	const int pid = this->compile(shaders, nullptr);
	if (pid < 0)
	{
		R_ASSERT(pid >= 0, "Failed to compile default shader");
		return;
	}

	memset(this->ortho, 0, sizeof(this->ortho));
	this->ortho[ 0] =  1.0f;
	this->ortho[ 3] = -1.0f;
	this->ortho[ 5] =  1.0f;
	this->ortho[ 7] = -1.0f;
	this->ortho[10] = -1.0f;
	this->ortho[15] =  1.0f;

	Instance = this;
}

ShaderManager::~ShaderManager()
{
	Instance = nullptr;
	glUseProgram(0);
	for (const auto &details : this->programs)
		glDeleteProgram(details.program);
	for (const auto shader : this->shaders)
		glDeleteShader(shader);
}

void ShaderManager::initialise()
{
	R_ASSERT(this->active < 0, "ShaderManager is already initialised");

	this->active = 0;
	const Shader::Details &details = this->programs[this->active];
	glUseProgram(details.program);
	set_projection_matrix(details.program, this->ortho);
	glUniform1i(glGetUniformLocation(this->programs[0].program, "texture"), 0);
	glEnableVertexAttribArray(Shader::kAttributeVertex);
	glEnableVertexAttribArray(Shader::kAttributeColor);
	glEnableVertexAttribArray(Shader::kAttributeTexCoord);
}
