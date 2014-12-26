// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SHADERS_DIFFUSE_H_
#define GRAPHICS_SHADERS_DIFFUSE_H_

#include "Common/Vec3.h"

namespace Rainbow
{
	namespace Shaders
	{
		class Diffuse
		{
		public:
			Diffuse(const bool normal = false);

			void set_cutoff(const float cutoff) const;
			void set_radius(const float radius) const;
			void set_position(const float x, const float y, const float z = 100.0f) const;

			inline unsigned int id() const;

		private:
			int cutoff_;    ///< Maximum distance of the light's influence.
			int radius_;    ///< Light source's radius.
			int position_;  ///< Position of the light source.
			unsigned int program_;
		};

		unsigned int Diffuse::id() const
		{
			return program_;
		}
	}
}

#endif
