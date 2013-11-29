// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/ShaderManager.h"
#include "Graphics/Shaders.h"
#include "Graphics/Shaders/Diffuse.h"

namespace
{
	Shader::ShaderParams sDiffuseLight2Df {
		Shader::kTypeFragment, 0, Rainbow::Shaders::kDiffuseLight2Df
	};
	Shader::ShaderParams sDiffuseLightNormalf {
		Shader::kTypeFragment, 0, Rainbow::Shaders::kDiffuseLightNormalf
	};
	Shader::ShaderParams sNormalMappedf {
		Shader::kTypeVertex, 0, Rainbow::Shaders::kNormalMappedv
	};
}

namespace Rainbow
{
	namespace Shaders
	{
		Diffuse::Diffuse(const bool normal) :
			cutoff(0), radius(0), position(0), program(-1)
		{
			if (normal)
			{
				Shader::ShaderParams shaders[] = {
					sNormalMappedf,
					sDiffuseLightNormalf,
					{ Shader::kTypeInvalid, 0, nullptr }
				};
				const Shader::AttributeParams attributes[] = {
					{ Shader::kAttributeVertex, "vertex" },
					{ Shader::kAttributeColor, "color" },
					{ Shader::kAttributeTexCoord, "texcoord" },
					{ Shader::kAttributeNormal, "normal" },
					{ Shader::kAttributeNone, nullptr }
				};
				this->program =
				    ShaderManager::Instance->compile(shaders, attributes);
			}
			else
			{
				Shader::ShaderParams shaders[] = {
					{ Shader::kTypeVertex, 0, nullptr },  // kFixed2Dv
					sDiffuseLight2Df,
					{ Shader::kTypeInvalid, 0, nullptr }
				};
				this->program =
				    ShaderManager::Instance->compile(shaders, nullptr);
			}
			if (this->program < 0)
				return;

			ShaderManager::Context context;
			ShaderManager::Instance->use(this->program);
			Shader::Details &details =
			    ShaderManager::Instance->get_program(this->program);
			this->cutoff = glGetUniformLocation(details.program, "cutoff");
			this->radius = glGetUniformLocation(details.program, "radius");
			this->position = glGetUniformLocation(details.program, "light");
			glUniform1i(glGetUniformLocation(details.program, "texture"), 0);
			if (normal)
			{
				glUniform1i(glGetUniformLocation(details.program, "normal"), 1);
				details.texture1 = true;
			}

			R_ASSERT(glGetError() == GL_NO_ERROR,
			         "Failed to load diffuse shader");
		}

		void Diffuse::set_cutoff(const float cutoff) const
		{
			ShaderManager::Context context;
			ShaderManager::Instance->use(this->program);
			glUniform1f(this->cutoff, cutoff);
		}

		void Diffuse::set_radius(const float radius) const
		{
			ShaderManager::Context context;
			ShaderManager::Instance->use(this->program);
			glUniform1f(this->radius, radius);
		}

		void Diffuse::set_position(const float x, const float y, const float z) const
		{
			ShaderManager::Context context;
			ShaderManager::Instance->use(this->program);
			glUniform3f(this->position, x, y, z);
		}
	}
}
