// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_IO.h"

#include "Common/Data.h"
#include "FileSystem/File.h"
#include "Lua/LuaHelper.h"
#include "Lua/LuaSyntax.h"

using rainbow::Data;
using rainbow::File;
using rainbow::FileType;
using rainbow::WriteableFile;
using rainbow::czstring;

namespace
{
    int load(lua_State* L)
    {
        // rainbow.io.load(filename)
        rainbow::lua::checkargs<char*>(L);

        const Data& data = File::read(lua_tostring(L, 1), FileType::UserFile);
        if (!data)
            lua_pushnil(L);
        else
            lua_pushlstring(L, data.as<char*>(), data.size());
        return 1;
    }

    int save(lua_State* L)
    {
        // rainbow.io.save(filename, data, size)
        rainbow::lua::checkargs<char*, char*, lua_Number>(L);

        const auto size = static_cast<size_t>(lua_tointeger(L, 3));
        const Data data(lua_tostring(L, 2), size, Data::Ownership::Reference);
        czstring path = lua_tostring(L, 1);
        rainbow::lua::push(L, WriteableFile::write(path, data) == size);
        return 1;
    }
}

void rainbow::lua::io::init(lua_State* L)
{
    lua_pushliteral(L, "io");
    lua_createtable(L, 0, 2);
    luaR_rawsetcfunction(L, "load", &::load);
    luaR_rawsetcfunction(L, "save", &::save);
    lua_rawset(L, -3);
}
