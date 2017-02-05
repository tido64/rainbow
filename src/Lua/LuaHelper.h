// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUAHELPER_H_
#define LUA_LUAHELPER_H_

#include <new>
#include <utility>

#include <lua.hpp>

#include "Common/Constraints.h"
#include "Common/NonCopyable.h"
#include "Common/String.h"
#include "Lua/LuaMacros.h"

namespace rainbow { class Data; }

namespace std
{
    template <>
    struct default_delete<lua_State>
    {
        void operator()(lua_State* L) const { lua_close(L); }
    };
}

NS_RAINBOW_LUA_BEGIN
{
    template <typename T>
    class Bind;

    template <typename T>
    using LuaBindable = std::enable_if_t<std::is_base_of<Bind<T>, T>::value>;

    /// <summary>
    ///   The equivalent of <see cref="std::weak_ptr"/> for Lua objects.
    /// </summary>
    class WeakRef : private NonCopyable<WeakRef>
    {
    public:
        static int RegistryIndex;

        WeakRef() : state_(nullptr), ref_(LUA_NOREF) {}
        WeakRef(lua_State*);
        ~WeakRef();

        void get() const;
        void reset(lua_State* L = nullptr);

        explicit operator bool() const { return ref_ >= 0; }

    private:
        lua_State* state_;
        int ref_;
    };

    /// <summary>
    ///   Outputs information about the error, and dumps the stack if
    ///   applicable.
    /// </summary>
    void error(lua_State* L, int result);

    /// <summary>
    ///   Returns the pointer returned from <see cref="luaL_checkudata"/> or
    ///   <see cref="lua_touserdata"/>, depending on whether <c>NDEBUG</c> is
    ///   defined.
    /// </summary>
    template <typename T, typename = LuaBindable<T>>
    auto touserdata(lua_State* L, int n) -> T*
    {
#ifndef NDEBUG
        return static_cast<T*>(luaL_checkudata(L, n, T::class_name));
#else
        return static_cast<T*>(lua_touserdata(L, n));
#endif
    }

    /// <summary>
    ///   Returns the string representing a Lua wrapped object. The format of
    ///   the string is "<type name>: <address>".
    /// <//summary>
    template <typename T, typename = LuaBindable<T>>
    auto tostring(lua_State* L)
    {
        lua_pushfstring(L, "%s: %p", T::class_name, touserdata<T>(L, 1));
        return 1;
    }

    /// <summary>Creates a Lua wrapped object.</summary>
    template <typename T, typename... Args>
    auto alloc(lua_State* L, Args&&... args) -> int
    {
        static_assert(
            std::is_base_of<Bind<T>, T>::value, "T must be a subclass of Bind");

        void* data = lua_newuserdata(L, sizeof(T));
        luaL_setmetatable(L, T::class_name);
        // Stash the userdata so we can return it later.
        const int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        new (data) T(L, std::forward<Args>(args)...);
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
        return 1;
    }

    template <typename... Args>
    auto call(lua_State* L,
              int nargs,
              int nresults,
              int errfunc,
              const char* err,
              Args&&... args) -> int
    {
        const int result = lua_pcall(L, nargs, nresults, errfunc);
        if (errfunc)
            lua_remove(L, 1);
        if (result != LUA_OK)
        {
            error(L, result);
            return luaL_error(L, err, std::forward<Args>(args)...);
        }
        return LUA_OK;
    }

    template <typename T, typename = LuaBindable<T>>
    auto dealloc(lua_State* L)
    {
        touserdata<T>(L, 1)->~T();
        return 0;
    }

    /// <summary>
    ///   Returns <c>true</c> if the value at the given index is a boolean, and
    ///   <c>false</c> otherwise.
    /// </summary>
    inline auto isboolean(lua_State* L, int index)
    {
        return lua_isboolean(L, index);
    }

    /// <summary>
    ///   Returns <c>true</c> if the value at the given index is a number or a
    ///   string convertible to a number, and <c>false</c> otherwise.
    /// </summary>
    inline auto isnumber(lua_State* L, int index)
    {
        return lua_isnumber(L, index) != 0;
    }

    /// <summary>
    ///   Returns <c>true</c> if the value at the given index is a string or a
    ///   number (which is always convertible to a string), and <c>false</c>
    ///   otherwise.
    /// </summary>
    inline auto isstring(lua_State* L, int index)
    {
        return lua_isstring(L, index) != 0;
    }

    /// <summary>
    ///   Returns <c>true</c> if the value at the given index is a table, and
    ///   <c>false</c> otherwise.
    /// </summary>
    inline auto istable(lua_State* L, int index)
    {
        return lua_istable(L, index);
    }

    /// <summary>
    ///   Returns <c>true</c> if the value at the given index is a userdata
    ///   (either full or light), and <c>false</c> otherwise.
    /// </summary>
    inline auto isuserdata(lua_State* L, int index)
    {
        return lua_isuserdata(L, index) != 0;
    }

    /// <summary>Custom Lua package loader.</summary>
    auto load(lua_State* L) -> int;

    /// <summary>Loads buffer as a Lua chunk.</summary>
    /// <param name="L">Lua state.</param>
    /// <param name="chunk">Buffer to load.</param>
    /// <param name="name">
    ///   Name of the chunk. Used for debug information.
    /// </param>
    /// <param name="exec">
    ///   For internal use only! Whether to execute the loaded chunk. Only used
    ///   by Lua package loaders.
    /// </param>
    /// <returns>Number of successfully loaded chunks.</returns>
    auto load(lua_State* L,
              const Data& chunk,
              const char* name,
              bool exec = true) -> int;

    /// <summary>Creates a new Lua state.</summary>
    inline auto newstate()
    {
        return std::unique_ptr<lua_State>{luaL_newstate()};
    }

    /// <summary>
    ///   Returns the value returned from <see cref="luaL_optinteger"/> but
    ///   without the extra type check if <c>NDEBUG</c> is defined.
    /// </summary>
    inline auto optinteger(lua_State* L, int n, lua_Integer def)
    {
#ifndef NDEBUG
        return luaL_optinteger(L, n, def);
#else
        return luaL_opt(L, lua_tointeger, n, def);
#endif
    }

    /// <summary>
    ///   Returns the value returned from <see cref="luaL_optnumber"/> but
    ///   without the extra type check if <c>NDEBUG</c> is defined.
    /// </summary>
    inline auto optnumber(lua_State* L, int n, lua_Number def)
    {
#ifndef NDEBUG
        return luaL_optnumber(L, n, def);
#else
        return luaL_opt(L, lua_tonumber, n, def);
#endif
    }

    template <typename T>
    void push(lua_State* L, T value);

    /// <summary>Pushes a collectable pointer on the stack.</summary>
    /// <remarks>
    ///   Wraps pointer in a table so that one can attach an arbitrary metatable
    ///   and have the garbage collector clean it up. Also sets the
    ///   <c>__type</c> field for type checking.
    /// </remarks>
    /// <param name="L">Lua state.</param>
    /// <param name="ptr">The pointer to push on the stack.</param>
    /// <param name="name">Name of the pointer type.</param>
    void pushpointer(lua_State* L, void* ptr, const char* name);

    /// <summary>
    ///   Does the equivalent of <c>t[field] = value</c>, where <c>t</c> is the
    ///   table at the top of the stack, without metamethods.
    /// </summary>
    /// <param name="L">Lua state.</param>
    /// <param name="field">Name of the index.</param>
    /// <param name="length">Length of <paramref name="field"/>.</param>
    /// <param name="value">The value to assign.</param>
    template <typename T>
    void rawset(lua_State* L, const char* field, size_t length, T value)
    {
        lua_pushlstring(L, field, length);
        push<T>(L, value);
        lua_rawset(L, -3);
    }

    /// <summary>Simple C++-wrapper, loosely based on LunaWrapper.</summary>
    /// <remarks>
    ///   <para>
    ///     Wraps a C++ object and makes its methods available in the namespace
    ///     on top of the stack.
    ///   </para>
    ///   References
    ///   <list type="bullet">
    ///     <item>http://www.lua.org/manual/5.2/</item>
    ///     <item>http://lua-users.org/wiki/LunaWrapper</item>
    ///   </list>
    /// </remarks>
    template <typename T, typename = LuaBindable<T>>
    void reg(lua_State* L)
    {
        if (T::is_constructible)
        {
            lua_pushstring(L, T::class_name);
            lua_pushcclosure(L, &alloc<T>, 0);
            lua_rawset(L, -3);
        }
        luaL_newmetatable(L, T::class_name);  // metatable = {}
        luaL_setfuncs(L, T::functions, 0);
        luaR_rawsetcfunction(L, "__gc", &dealloc<T>);
        luaR_rawsetcfunction(L, "__tostring", &tostring<T>);
        lua_pushliteral(L, "__index");
        lua_pushvalue(L, -2);
        lua_rawset(L, -3);  // metatable.__index = metatable
        lua_pushliteral(L, "__metatable");
        lua_createtable(L, 0, 0);
        lua_rawset(L, -3);  // metatable.__metatable = {}
        lua_pop(L, 1);
    }

    /// <summary>Reloads a previously loaded Lua chunk.</summary>
    /// <param name="L">Lua state.</param>
    /// <param name="chunk">Buffer to load.</param>
    /// <param name="name">
    ///   Name of the chunk. Used for debug information.
    /// </param>
    /// <returns>Number of successfully reloaded chunks.</returns>
    auto reload(lua_State* L, const Data& chunk, const char* name) -> int;

    /// <summary>
    ///   Replaces the table at index <paramref name="n"/> with its userdata if
    ///   one exists.
    /// </summary>
    void replacetable(lua_State* L, int n);

    /// <summary>Sets debugging hook.</summary>
    void sethook(lua_State* L,
                 int mask = LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE);

    /// <summary>
    ///   Converts the Lua value at the given acceptable index to a boolean
    ///   value.
    /// </summary>
    inline auto toboolean(lua_State* L, int index)
    {
        return lua_toboolean(L, index) != 0;
    }

    /// <summary>
    ///   Returns the value returned from <see cref="luaL_checkinteger"/> or
    ///   <see cref="lua_tointeger"/>, depending on whether <c>NDEBUG</c> is
    ///   defined.
    /// </summary>
    inline auto tointeger(lua_State* L, int n)
    {
#ifndef NDEBUG
        return luaL_checkinteger(L, n);
#else
        return lua_tointeger(L, n);
#endif
    }

    /// <summary>
    ///   Returns the value returned from <see cref="luaL_checknumber"/> or
    ///   <see cref="lua_tonumber"/>, depending on whether <c>NDEBUG</c> is
    ///   defined.
    /// </summary>
    inline auto tonumber(lua_State* L, int n)
    {
#ifndef NDEBUG
        return luaL_checknumber(L, n);
#else
        return lua_tonumber(L, n);
#endif
    }

    /// <summary>Returns the pointer on top of the stack.</summary>
    /// <remarks>
    ///   Unwraps the pointer while checking for nil value and type. This method
    ///   may return a nullptr.
    ///   <seealso cref="pushpointer"/>
    /// </remarks>
    /// <param name="L">Lua state.</param>
    /// <param name="name">Name of the pointer type to return.</param>
    /// <returns>
    ///   The pointer on the top of the stack if valid, else <c>nullptr</c>.
    /// </returns>
    auto topointer(lua_State* L, const char* name) -> void*;

    template <typename T>
    auto tryuserdata(lua_State* L, int index)
    {
        return static_cast<T*>(luaL_testudata(L, index, T::class_name));
    }
} NS_RAINBOW_LUA_END

#endif
