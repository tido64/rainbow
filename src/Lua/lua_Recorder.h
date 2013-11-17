// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_RECORDER_H_
#define LUA_RECORDER_H_

#include "ConFuoco/Recorder.h"
#include "Lua/LuaBind.h"

namespace Rainbow
{
	namespace Lua
	{
		class Recorder :
			public ConFuoco::Recorder,
			public Bind<Recorder, ConFuoco::Recorder, kBindTypeDerived>
		{
			friend class Bind<Recorder, ConFuoco::Recorder, kBindTypeDerived>;

		public:
			Recorder(lua_State *);

		private:
			int get_average_power(lua_State *);
			int get_low_pass(lua_State *);
			int get_peak_power(lua_State *);

			int pause(lua_State *);
			int record(lua_State *);
			int stop(lua_State *);

			int update(lua_State *);
		};
	}
}

#endif
