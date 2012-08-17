// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include <cstdio>

#include "Common/Debug.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Pipeline.h"
#include "Graphics/Shader.h"

Pipeline::Pipeline() : program(0), vertex(0), fragment(0)
{
	this->program = glCreateProgram();
}

Pipeline::~Pipeline()
{
	glDeleteProgram(this->program);
}

void Pipeline::attach_shader(const Shader *shader)
{
	unsigned int &m_shader
		= (shader->type == GL_VERTEX_SHADER) ? this->vertex : this->fragment;
	if (m_shader)
		glDetachShader(this->program, m_shader);
	m_shader = shader->name;
	glAttachShader(this->program, m_shader);
}

void Pipeline::use() const
{
	glUseProgram(this->program);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(this->program, "texture"), 0);

	glEnableVertexAttribArray(COLOR_LOCATION);
	glEnableVertexAttribArray(TEXCOORD_LOCATION);
	glEnableVertexAttribArray(VERTEX_LOCATION);
}

bool Pipeline::link()
{
	glBindAttribLocation(this->program, COLOR_LOCATION, "color");
	glBindAttribLocation(this->program, TEXCOORD_LOCATION, "texcoord");
	glBindAttribLocation(this->program, VERTEX_LOCATION, "vertex");

	glLinkProgram(this->program);

	GLint status = 0;
	glGetProgramiv(this->program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint info_len = 0;
		glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &info_len);
		if (info_len > 0)
		{
			char *log = new char[info_len + 1];
			glGetProgramInfoLog(this->program, info_len, nullptr, log);
			log[info_len] = '\0';
			fprintf(stderr, "[Rainbow] GLSL: Failed to link program: %s\n", log);
			delete[] log;
		}
		return false;
	}

	return glGetError() == GL_NO_ERROR;
}
