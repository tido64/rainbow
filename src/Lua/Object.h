// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_OBJECT_H_
#define LUA_OBJECT_H_

#include "Lua/LuaHelper.h"
#include "Lua/LuaSyntax.h"
#include "Math/Vec2.h"

#define LUA_REG_OBJECT(Class, name)                                            \
    constexpr bool Class::is_constructible;                                    \
    const char Class::class_name[] = name;                                     \
    const luaL_Reg Class::functions[]
#define LUA_REG_OBJECT_END() luaL_Reg{}
#define LUA_REG_OBJECT_FUNC(Class, func) luaL_Reg{#func, &Class::func}
#define LUA_REG_OBJECT_PROPS(constructible)                                    \
    static constexpr bool is_constructible = constructible;                    \
    static const char class_name[];                                            \
    static const luaL_Reg functions[];

namespace rainbow { namespace lua
{
    template <typename T>
    class Object
    {
    protected:
        template <typename F, typename... Args>
        static auto get1b(lua_State* L, F&& get, Args&&... args) -> int
        {
            return with_self(L, [&get, &args...](T* self, lua_State* L) {
                lua_pushboolean(
                    L, get(self->get(), std::forward<Args>(args)...));
                return 1;
            });
        }

#ifdef USE_PHYSICS
        template <typename F>
        static auto set1b(lua_State* L, F&& set) -> int
        {
            checkargs<T, bool>(L);

            return with_self(L, [&set](T* self, lua_State* L) {
                set(self->get(), lua_toboolean(L, 2));
                return 0;
            });
        }
#endif  // USE_PHYSICS

        template <typename F>
        static auto get1f(lua_State* L, F&& get) -> int
        {
            return with_self(L, [&get](T* self, lua_State* L) {
                lua_pushnumber(L, get(self->get()));
                return 1;
            });
        }

        template <typename F>
        static auto set1f(lua_State* L, F&& set) -> int
        {
            checkargs<T, lua_Number>(L);

            return with_self(L, [&set](T* self, lua_State* L) {
                set(self->get(), lua_tonumber(L, 2));
                return 0;
            });
        }

        template <typename F, typename... Args>
        static auto get1fv(lua_State* L, F&& get, Args&&... args) -> int
        {
            return with_self(L, [&get, &args...](T* self, lua_State* L) {
                const auto& v = get(self->get(), std::forward<Args>(args)...);
                lua_pushnumber(L, v.x);
                lua_pushnumber(L, v.y);
                return 2;
            });
        }

        template <typename F>
        static auto set1fv(lua_State* L, F&& set) -> int
        {
            checkargs<T, lua_Number, lua_Number>(L);

            return with_self(L, [&set](T* self, lua_State* L) {
                set(self->get(), Vec2f(lua_tonumber(L, 2), lua_tonumber(L, 3)));
                return 0;
            });
        }

        template <typename F>
        static auto set2f(lua_State* L, F&& set) -> int
        {
            checkargs<T, lua_Number, lua_Number>(L);

            return with_self(L, [&set](T* self, lua_State* L) {
                set(self->get(), lua_tonumber(L, 2), lua_tonumber(L, 3));
                return 0;
            });
        }

        template <typename F>
        static auto get1i(lua_State* L, F&& get) -> int
        {
            return with_self(L, [&get](T* self, lua_State* L) {
                lua_pushinteger(L, get(self->get()));
                return 1;
            });
        }

        template <typename F>
        static auto set1i(lua_State* L, F&& set) -> int
        {
            checkargs<T, lua_Number>(L);

            return with_self(L, [&set](T* self, lua_State* L) {
                set(self->get(), lua_tointeger(L, 2));
                return 0;
            });
        }

        template <typename U, typename F>
        static auto set1ud(lua_State* L, F&& set) -> int
        {
            checkargs<T, U>(L);

            return with_self(L, [&set](T* self, lua_State* L) {
                set(self->get(), touserdata<U>(L, 2)->get());
                return 0;
            });
        }

#ifdef USE_PHYSICS
        template <typename U, typename F>
        static auto get1ud(lua_State* L, F&& get) -> int
        {
            return with_self(L, [&get](T* self, lua_State* L) {
                void* p = get(self->get());
                if (!p)
                    return 0;

                lua_pushlightuserdata(L, p);
                return lua::alloc<U>(L);
            });
        }
#endif  // USE_PHYSICS

        template <typename F>
        static auto with_self(lua_State* L, F&& action) -> int
        {
            T* self = touserdata<T>(L, 1);
            return self == nullptr ? 0 : action(self, L);
        }

        ~Object() {}
    };
}}  // namespace rainbow::lua

#endif
