#ifndef LUA_RANDOM_H_
#define LUA_RANDOM_H_

struct lua_State;

namespace Rainbow
{
	namespace Lua
	{
		class Random
		{
			friend void init(lua_State *);

		public:
			static int random(lua_State *);

		private:
			static void init(lua_State *);

			Random();
			Random(const Random &);
			Random& operator=(const Random &);
		};
	}
}

#endif
