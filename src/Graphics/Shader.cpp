// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Common/RainbowAssert.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Shader.h"

Shader::Shader(const unsigned int type, const char *shader) :
	name(0), type(type)
{
	const GLuint id = glCreateShader(type);
	glShaderSource(id, 1, &shader, nullptr);
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
			fprintf(stderr, "[Rainbow] GLSL: Failed to compile %s shader: %s\n", (type == GL_VERTEX_SHADER) ? "vertex" : "fragment", log);
			delete[] log;
		}
		glDeleteShader(id);
	}
	else
		this->name = id;
}

Shader::~Shader()
{
	if (this->name)
		glDeleteShader(this->name);
}

Shader::operator bool() const
{
	return this->name;
}
