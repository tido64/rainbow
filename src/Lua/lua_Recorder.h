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
