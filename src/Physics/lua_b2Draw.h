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

			int shaders[2];
			shaders[0] = ShaderManager::Instance->create_shader(
					Shader::VERTEX_SHADER, simple2d_vsh);
			shaders[1] = ShaderManager::Instance->create_shader(
					Shader::FRAGMENT_SHADER, simple_fsh);
			this->program = ShaderManager::Instance->create_program(shaders, 2);
		}

		void draw()
		{
			ShaderManager::Instance->use(this->program);
			Renderer::bind_buffer(0);

			for (size_t i = 0; i < num_worlds; ++i)
				if (this->worlds[i])
					this->worlds[i]->DrawDebugData();

			ShaderManager::Instance->reset();
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
