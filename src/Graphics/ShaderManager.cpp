// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

// TODO: The Visual C++ compiler doesn't handle function pointers as template
// parameters very well so we pass them as method parameters instead. This may
// or may not be fixed in VS2013.
#ifdef _MSC_VER
#	define WORKAROUND_FUNCPTR_IN_TEMPLATES
#	include <functional>
#endif

#include <memory>

#include "Common/Data.h"
#include "Graphics/ShaderManager.h"

namespace
{
#ifdef WORKAROUND_FUNCPTR_IN_TEMPLATES
	std::unique_ptr<char[]> get_info_log(
			std::function<void(GLuint, GLenum, GLint*)> glGetiv,
			std::function<void(GLuint, GLsizei, GLsizei*, GLchar*)> glGetInfoLog,
			const GLuint id)
#else
	template<void (*glGetiv)(GLuint, GLenum, GLint*),
	         void (*glGetInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*)>
	std::unique_ptr<char[]> get_info_log(const GLuint id)
#endif
	{
		GLint info_len = 0;
		glGetiv(id, GL_INFO_LOG_LENGTH, &info_len);
		if (info_len == 0)
			return std::unique_ptr<char[]>();
		char *log = new char[info_len];
		glGetInfoLog(id, info_len, nullptr, log);
		log[info_len] = '\0';
		return std::unique_ptr<char[]>(log);
	}

	void set_projection_matrix(const GLuint program, const GLfloat *ortho)
	{
		const int location = glGetUniformLocation(program, "mvp_matrix");
		R_ASSERT(location >= 0, "Shader is missing a projection matrix");
		glUniformMatrix4fv(location, 1, GL_FALSE, ortho);
	}
}

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
		const std::unique_ptr<char[]> &log =
	#ifdef WORKAROUND_FUNCPTR_IN_TEMPLATES
				get_info_log(glGetProgramiv, glGetProgramInfoLog, program.id);
	#else
				get_info_log<glGetProgramiv, glGetProgramInfoLog>(program.id);
	#endif
		R_ERROR("[Rainbow] GLSL: Failed to link program: %s\n", log.get());
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

	GLint status = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		const std::unique_ptr<char[]> &log =
	#ifdef WORKAROUND_FUNCPTR_IN_TEMPLATES
				get_info_log(glGetShaderiv, glGetShaderInfoLog, id);
	#else
				get_info_log<glGetShaderiv, glGetShaderInfoLog>(id);
	#endif
		R_ERROR("[Rainbow] GLSL: Failed to compile %s shader: %s\n",
		        (type == GL_VERTEX_SHADER) ? "vertex" : "fragment", log.get());
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

void ShaderManager::set_projection(const float left, const float right, const float bottom, const float top)
{
	this->ortho[0] = 2.0f / (right - left);
	this->ortho[3] = -(right + left) / (right - left);
	this->ortho[5] = 2.0f / (top - bottom);
	this->ortho[7] = -(top + bottom) / (top - bottom);
	set_projection_matrix(this->active.id, this->ortho);
}

void ShaderManager::use(const int program)
{
	if (this->programs[program].id == this->active.id)
		return;

	this->active = this->programs[program];
	glUseProgram(this->active.id);

	set_projection_matrix(this->active.id, this->ortho);

	glEnableVertexAttribArray(Shader::VERTEX);
	glEnableVertexAttribArray(Shader::COLOR);

	const int location = glGetUniformLocation(this->active.id, "texture");
	if (location < 0)
		glDisableVertexAttribArray(Shader::TEXCOORD);
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(location, 0);
		glEnableVertexAttribArray(Shader::TEXCOORD);
	}
}
