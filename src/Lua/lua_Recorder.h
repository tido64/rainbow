#ifndef LUA_RECORDER_H_
#define LUA_RECORDER_H_

#include "ConFuoco/Recorder.h"
#include "Lua/LuaHelper.h"

namespace Rainbow
{
	namespace Lua
	{
		class Recorder
		{
		public:
			static const char class_name[];
			static const Method<Recorder> methods[];

			Recorder(lua_State *);

			int get_average_power(lua_State *);
			int get_low_pass(lua_State *);
			int get_peak_power(lua_State *);

			inline int pause(lua_State *);
			inline int record(lua_State *);
			inline int stop(lua_State *);

			inline int update(lua_State *);

		private:
			ConFuoco::Recorder recorder;
		};

		int Recorder::pause(lua_State *)
		{
			this->recorder.pause();
			return 0;
		}

		int Recorder::record(lua_State *)
		{
			this->recorder.record();
			return 0;
		}

		int Recorder::stop(lua_State *)
		{
			this->recorder.stop();
			return 0;
		}

		int Recorder::update(lua_State *)
		{
			this->recorder.update();
			return 0;
		}
	}
}

#endif
