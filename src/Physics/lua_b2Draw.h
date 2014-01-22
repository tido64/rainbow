// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <Graphics/Renderer.h>
#include "Graphics/ShaderManager.h"

#ifdef GL_ES_VERSION_2_0
#	include "Graphics/Shaders/Shaders.h"
#else
#	define simple2d_vsh  "Shaders/Simple2D.vsh"
#	define simple_fsh    "Shaders/Simple.fsh"
#endif

namespace b2
{
	class DebugData
	{
	public:
		DebugData() : program(0)
		{
			memset(this->worlds, 0, sizeof(this->worlds));

			Shader::ShaderParams shaders[] = {
				{ Shader::kTypeVertex, 0, simple2d_vsh },
				{ Shader::kTypeFragment, 0, simple_fsh },
				{ Shader::kTypeInvalid, 0, nullptr }
			};
			const Shader::AttributeParams attributes[] = {
				{ Shader::kAttributeVertex, "vertex" },
				{ Shader::kAttributeColor, "color" },
				{ Shader::kAttributeNone, nullptr }
			};
			this->program =
			    ShaderManager::Instance->compile(shaders, attributes);
			Shader::Details &details =
			    ShaderManager::Instance->get_program(this->program);
			details.texture0 = false;
		}

		void draw()
		{
			ShaderManager::Context context;
			ShaderManager::Instance->use(this->program);

			for (size_t i = 0; i < num_worlds; ++i)
				if (this->worlds[i])
					this->worlds[i]->DrawDebugData();
		}

		void erase(b2World *world)
		{
			for (size_t i = 0; i < num_worlds; ++i)
			{
				if (this->worlds[i] == world)
				{
					this->worlds[i] = nullptr;
					break;
				}
			}
		}

		void push_back(b2World *world)
		{
			for (size_t i = 0; i < num_worlds; ++i)
			{
				if (!this->worlds[i])
				{
					this->worlds[i] = world;
					return;
				}
			}
		}

	private:
		static const size_t num_worlds = 16;

		int program;
		b2World *worlds[num_worlds];
	} *g_debug_data;

	void DrawDebugData()
	{
		g_debug_data->draw();
	}
}
