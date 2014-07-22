// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_B2DRAW_H_
#define THIRDPARTY_BOX2D_LUA_B2DRAW_H_

#include "Graphics/Renderer.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Shaders.h"

namespace b2
{
	class DebugData
	{
	public:
		DebugData() : program_(0)
		{
			std::fill_n(worlds_, kNumWorlds, nullptr);

			Shader::ShaderParams shaders[] = {
				{ Shader::kTypeVertex, 0, Rainbow::Shaders::kSimple2Dv },
				{ Shader::kTypeFragment, 0, Rainbow::Shaders::kSimplef },
				{ Shader::kTypeInvalid, 0, nullptr }
			};
			const Shader::AttributeParams attributes[] = {
				{ Shader::kAttributeVertex, "vertex" },
				{ Shader::kAttributeColor, "color" },
				{ Shader::kAttributeNone, nullptr }
			};
			program_ = ShaderManager::Instance->compile(shaders, attributes);
			Shader::Details &details =
			    ShaderManager::Instance->get_program(program_);
			details.texture0 = false;
		}

		void draw()
		{
			ShaderManager::Context context;
			ShaderManager::Instance->use(program_);
			std::for_each(worlds_, worlds_ + kNumWorlds, [](b2World *world) {
				if (world)
					world->DrawDebugData();
			});
		}

		void erase(b2World *world)
		{
			for (int i = 0; i < kNumWorlds; ++i)
			{
				if (worlds_[i] == world)
				{
					worlds_[i] = nullptr;
					return;
				}
			}
		}

		void push_back(b2World *world)
		{
			for (int i = 0; i < kNumWorlds; ++i)
			{
				if (!worlds_[i])
				{
					worlds_[i] = world;
					return;
				}
			}
		}

	private:
		static const int kNumWorlds = 16;

		int program_;
		b2World *worlds_[kNumWorlds];
	} *g_debug_data;

	void DrawDebugData()
	{
		g_debug_data->draw();
	}
}

#endif
