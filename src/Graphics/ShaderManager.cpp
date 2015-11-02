// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/ShaderManager.h"

#include <memory>

#include "Common/Data.h"
#include "FileSystem/Path.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shaders.h"

// For platforms not using GLEW.
#ifndef GLAPIENTRY
#define GLAPIENTRY
#endif

namespace
{
	using String = std::unique_ptr<char[]>;

	const Shader::AttributeParams kAttributeDefaultParams[] = {
	    {Shader::kAttributeVertex, "vertex"},
	    {Shader::kAttributeColor, "color"},
	    {Shader::kAttributeTexCoord, "texcoord"},
	    {Shader::kAttributeNone, nullptr}};

	void set_projection_matrix(const Shader::Details& details,
	                           const std::array<float, 16>& ortho)
	{
		R_ASSERT(details.mvp_matrix >= 0,
		         "Shader is missing a projection matrix");
		glUniformMatrix4fv(details.mvp_matrix, 1, GL_FALSE, ortho.data());
	}

	template<typename F, typename G>
	String verify(const GLuint id,
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
				return {};
			char* log = new char[info_len];
			glGetInfoLog(id, info_len, nullptr, log);
			return String(log);
		}
		return {};
	}

	unsigned int compile_shader(const Shader::Params& shader)
	{
		const GLuint id = glCreateShader(shader.type);
		if (Path(shader.source).is_file())
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
			R_ABORT(
			    "GLSL: Failed to compile %s shader: %s",
			    (shader.type == Shader::kTypeVertex ? "vertex" : "fragment"),
			    error.get());
			glDeleteShader(id);
			return ShaderManager::kInvalidProgram;
		}

		return id;
	}

	unsigned int link_program(const Shader::Params* shaders,
	                          const Shader::AttributeParams* attributes)
	{
		const GLuint program = glCreateProgram();
		for (auto shader = shaders; shader->type != Shader::kTypeInvalid;
		     ++shader)
			glAttachShader(program, shader->id);
		for (auto attrib = attributes; attrib->name; ++attrib)
			glBindAttribLocation(program, attrib->index, attrib->name);
		glLinkProgram(program);

		const String& error = verify(
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

ShaderManager::~ShaderManager()
{
	for (const auto& details : programs_)
		glDeleteProgram(details.program);
	for (const auto shader : shaders_)
		glDeleteShader(shader);
}

unsigned int ShaderManager::compile(Shader::Params* shaders,
                                    const Shader::AttributeParams* attributes)
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
	set_projection_matrix(get_program(), renderer_->projection());
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

void ShaderManager::use(const unsigned int program)
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

ShaderManager::ShaderManager(Renderer* renderer)
    : current_(kInvalidProgram), renderer_(renderer) {}

bool ShaderManager::init()
{
	Shader::Params shaders[] = {
	    {Shader::kTypeVertex, 0, rainbow::shaders::kFixed2Dv,
	     rainbow::shaders::integrated::kFixed2Dv},
	    {Shader::kTypeFragment, 0, rainbow::shaders::kFixed2Df,
	     rainbow::shaders::integrated::kFixed2Df},
	    {Shader::kTypeInvalid, 0, nullptr, nullptr}};
	const unsigned int pid = compile(shaders, nullptr);
	if (pid == kInvalidProgram)
	{
		R_ABORT("Failed to compile default shader");
		return false;
	}
	make_global();
	return true;
}
