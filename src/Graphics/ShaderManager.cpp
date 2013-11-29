// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <memory>

#include "Common/Data.h"
#include "Graphics/ShaderManager.h"

// For platforms not using GLEW.
#ifndef GLAPIENTRY
#	define GLAPIENTRY
#endif

typedef std::unique_ptr<char[]> unique_str;

namespace
{
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
	};
}

ShaderManager *ShaderManager::Instance = nullptr;

int ShaderManager::create_program(const int *shaders, const size_t count)
{
	const GLuint program = glCreateProgram();
	for (size_t i = 0; i < count; ++i)
		glAttachShader(program, this->shaders[shaders[i]]);

	glBindAttribLocation(program, Shader::VERTEX, "vertex");
	glBindAttribLocation(program, Shader::COLOR, "color");
	glBindAttribLocation(program, Shader::TEXCOORD, "texcoord");

	glLinkProgram(program);

	const unique_str &error =
			verify(program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);
	if (error.get())
	{
		R_ERROR("[Rainbow] GLSL: Failed to link program: %s\n", error.get());
		glDeleteProgram(program);
		return Shader::INVALID;
	}

	this->programs.push_back(Shader::Details(program));
	return this->programs.size() - 1;
}

int ShaderManager::create_shader(int type, const char *src)
{
#ifdef GL_ES_VERSION_2_0
	const char *source = src;
#else
	const Data &glsl = Data::load_asset(src);
	if (!glsl)
	{
		R_ASSERT(glsl, "Failed to load shader");
		return Shader::INVALID;
	}
	const char *source = glsl;
#endif

	const GLuint id = glCreateShader(type);
	glShaderSource(id, 1, &source, nullptr);
	glCompileShader(id);

	const unique_str &error =
			verify(id, GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog);
	if (error.get())
	{
		R_ERROR("[Rainbow] GLSL: Failed to compile %s shader: %s\n",
		        (type == GL_VERTEX_SHADER) ? "vertex" : "fragment",
		        error.get());
		glDeleteShader(id);
		return Shader::INVALID;
	}

	this->shaders.push_back(id);
	return this->shaders.size() - 1;
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

	this->active = program;
	const Shader::Details &details = this->programs[this->active];
	glUseProgram(details.program);

	set_projection_matrix(details.program, this->ortho);

	glEnableVertexAttribArray(Shader::VERTEX);
	glEnableVertexAttribArray(Shader::COLOR);

	if (!details.texture0)
		glDisableVertexAttribArray(Shader::TEXCOORD);
	else
		glEnableVertexAttribArray(Shader::TEXCOORD);
}

ShaderManager::ShaderManager(const char *shaders[2]) : active(-1)
{
	R_ASSERT(Instance == nullptr, "There can be only one ShaderManager");
	R_ASSERT(shaders[0] && shaders[1],
	         "No vertex and/or fragment shaders specified");

	const int tmp[] = {
		this->create_shader(Shader::VERTEX_SHADER, shaders[0]),
		this->create_shader(Shader::FRAGMENT_SHADER, shaders[1])
	};
	R_ASSERT(tmp[0] != Shader::INVALID && tmp[1] != Shader::INVALID,
	         "Failed to compile default shader");
	const int pid = this->create_program(tmp, 2);
	R_ASSERT(pid != Shader::INVALID, "Failed to compile default shader");

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

	this->use(0);
	glUniform1i(glGetUniformLocation(this->programs[0].program, "texture"), 0);
}
