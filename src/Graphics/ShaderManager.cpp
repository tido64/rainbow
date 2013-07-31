// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#include "Common/Data.h"
#include "Graphics/ShaderManager.h"

ShaderManager *ShaderManager::Instance = nullptr;

ShaderManager::ShaderManager(const char **shaders, const size_t count)
{
	R_ASSERT(Instance == nullptr, "There can be only one ShaderManager");

	if (!shaders || !count || count > 2)
		return;

	int tmp[] = { Shader::VERTEX_SHADER, Shader::FRAGMENT_SHADER };
	for (size_t i = 0; i < count; ++i)
	{
		tmp[i] = this->create_shader(tmp[i], shaders[i]);
		if (tmp[i] == Shader::INVALID)
			return;
	}
	this->create_program(tmp, count);
	if (!this->programs.size())
		return;
	this->active.id = -1;

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
	for (const auto &program : this->programs)
		glDeleteProgram(program.id);
	for (const auto &shader : this->shaders)
		glDeleteShader(shader);
	Instance = nullptr;
}

int ShaderManager::create_program(const int *shaders, const size_t count)
{
	Program program = { glCreateProgram() };
	for (size_t i = 0; i < count; ++i)
		glAttachShader(program.id, this->shaders[shaders[i]]);

	glBindAttribLocation(program.id, Shader::VERTEX, "vertex");
	glBindAttribLocation(program.id, Shader::COLOR, "color");
	glBindAttribLocation(program.id, Shader::TEXCOORD, "texcoord");

	glLinkProgram(program.id);

	GLint status = 0;
	glGetProgramiv(program.id, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint info_len = 0;
		glGetProgramiv(program.id, GL_INFO_LOG_LENGTH, &info_len);
		if (info_len > 0)
		{
			char *log = new char[info_len + 1];
			glGetProgramInfoLog(program.id, info_len, nullptr, log);
			log[info_len] = '\0';
			R_ERROR("[Rainbow] GLSL: Failed to link program: %s\n", log);
			delete[] log;
		}
		glDeleteProgram(program.id);
		return Shader::INVALID;
	}

	this->programs.push_back(program);
	return this->programs.size() - 1;
}

int ShaderManager::create_shader(int type, const char *src)
{
#ifdef GL_ES_VERSION_2_0
	const char *source = src;
#else
	Data glsl(src);
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

	GLint status = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint info_len = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &info_len);
		if (info_len > 0)
		{
			char *log = new char[info_len + 1];
			glGetShaderInfoLog(id, info_len, nullptr, log);
			log[info_len] = '\0';
			R_ERROR("[Rainbow] GLSL: Failed to compile %s shader: %s\n",
			        (type == GL_VERTEX_SHADER) ? "vertex" : "fragment", log);
			delete[] log;
		}
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
}

void ShaderManager::set_ortho(const float left, const float right, const float bottom, const float top)
{
	this->ortho[0] = 2.0f / (right - left);
	this->ortho[3] = -(right + left) / (right - left);
	this->ortho[5] = 2.0f / (top - bottom);
	this->ortho[7] = -(top + bottom) / (top - bottom);

	int u = glGetUniformLocation(this->active.id, "mvp_matrix");
	R_ASSERT(u >= 0, "Shader is missing an orthographic projection matrix");
	glUniformMatrix4fv(u, 1, GL_FALSE, this->ortho);
}

void ShaderManager::use(const int program)
{
	if (this->programs[program].id == this->active.id)
		return;

	this->active = this->programs[program];
	glUseProgram(this->active.id);

	int u = glGetUniformLocation(this->active.id, "mvp_matrix");
	R_ASSERT(u >= 0, "Shader is missing an orthographic projection matrix");
	glUniformMatrix4fv(u, 1, GL_FALSE, this->ortho);

	glEnableVertexAttribArray(Shader::VERTEX);
	glEnableVertexAttribArray(Shader::COLOR);

	u = glGetUniformLocation(this->active.id, "texture");
	if (u < 0)
		glDisableVertexAttribArray(Shader::TEXCOORD);
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(u, 0);
		glEnableVertexAttribArray(Shader::TEXCOORD);
	}
}
