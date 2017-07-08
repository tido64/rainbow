// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef YGGDRASIL_LUAGENERATOR_H_
#define YGGDRASIL_LUAGENERATOR_H_

#include <iostream>
#include <sstream>

#include "ClangHelper.h"

namespace lua
{
    auto pushvalue(std::ostream& os, CXTypeKind type_kind, const char* name)
    {
        switch (type_kind)
        {
            case CXType_Bool:
                os << "lua_pushboolean(L, " << name << ");\n";
                break;
            case CXType_UInt:
            case CXType_ULong:
                os << "lua_pushinteger(L, static_cast<lua_Integer>(" << name << ")));\n";
                break;
            case CXType_Int:
            case CXType_Long:
                os << "lua_pushinteger(L, " << name << ");\n";
                break;
            case CXType_Float:
            case CXType_Double:
                os << "lua_pushnumber(L, " << name << ");\n";
                break;
            case CXType_LValueReference:
            case CXType_Record:
                os << "rainbow::lua::push(L, " << name << ");\n";
                break;
            default:
                os << "// TODO: rainbow::lua::push(L, " << name << ");\n";
                std::cout << "Unhandled type in pushvalue: " << Spelling(type_kind) << "\n";
                break;
        }
    }

    auto tovalue(std::ostream& os, CXCursor cursor, int index)
    {
        auto cursor_kind =
            clang_getCanonicalType(clang_getCursorType(cursor)).kind;
        switch (cursor_kind)
        {
            case CXType_Bool:
                os << "rainbow::lua::toboolean(L, " << index << ")";
                break;
            case CXType_UInt:
            case CXType_ULong:
                os << "static_cast<uint32_t>(rainbow::lua::tointeger(L, " << index << "))";
                break;
            case CXType_ULongLong:
                os << "static_cast<uint64_t>(rainbow::lua::tointeger(L, " << index << "))";
                break;
            case CXType_Int:
            case CXType_Long:
                os << "rainbow::lua::tointeger(L, " << index << ")";
                break;
            case CXType_Float:
            case CXType_Double:
                os << "rainbow::lua::tonumber(L, " << index << ")";
                break;
            case CXType_LValueReference:
            case CXType_Record:
                os << "rainbow::lua::tovalue<" << decayed_cursor_type(cursor) << ">(L, " << index << ")";
                break;
            default:
                os << "// TODO: rainbow::lua::tovalue<" << decayed_cursor_type(cursor) << ">(L, " << index << ")";
                std::cout << "Unhandled type in tovalue: " << Spelling(cursor_kind) << "\n";
                break;
        }
    }

    auto generate_body(const std::vector<Class>& models)
    {
        std::ostringstream os;
        os << R"(
namespace rainbow { namespace lua
{
    void push(lua_State* L, const SpriteRef& ref)
    {
        lua_createtable(L, 0, 2);
        lua_pushlightuserdata(L, ref.batch());
        lua_setfield(L, -2, "spritebatch");
        lua_pushinteger(L, ref.index());
        lua_setfield(L, -2, "index");
    }

    void push(lua_State* L, const Vec2f& v)
    {
        lua_createtable(L, 0, 2);
        lua_pushnumber(L, v.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, v.y);
        lua_setfield(L, -2, "y");
    }

    template <typename T>
    auto touserdata(lua_State* L, int index) -> T*
    {
        return static_cast<T*>(lua_touserdata(L, index));
    }

    template <>
    auto touserdata(lua_State* L, int index) -> Sprite*
    {
        lua_getfield(L, index, "spritebatch");
        auto sprite_batch = static_cast<SpriteBatch*>(lua_touserdata(L, -1));
        lua_getfield(L, index, "index");
        auto i = lua_tointeger(L, -1);
        auto sprite = &sprite_batch->at(i);
        lua_pop(2);
        return sprite;
    }
}}  // namespace rainbow::lua
)";

        for (auto&& model : models)
        {
            for (auto&& method : model.methods())
            {
                os << "\nauto " << method.entry_point_name() << "(lua_State* L) -> int\n"
                   << "{\n"
                   << "    auto self = rainbow::lua::touserdata<" << model.name() << ">(L, 1);\n";

                char fallback = 'a';
                int index = 1;
                for (auto&& arg : method.arguments())
                {
                    os << "    const auto ";
                    if (arg.has_name())
                        os << arg.name();
                    else
                        os << fallback++;
                    os << " = ";
                    tovalue(std::ref(os), arg.cursor(), ++index);
                    os << ";\n";
                }

                os << "    ";

                std::string result_name;
                const auto result_type = method.result().type().kind;
                if (result_type != CXType_Void)
                {
                    result_name = method.infer_result_name();
                    os << "const auto " << result_name << " = ";
                }

                fallback = 'a';
                os << "self->" << method.name() << '(';
                method.join_arguments(
                    std::ref(os),
                    [&fallback](std::ostream& os, const Argument& arg) mutable {
                        if (arg.has_name())
                            os << arg.name();
                        else
                            os << fallback++;
                    });
                os << ");\n";

                int num_results = 0;
                if (result_type != CXType_Void)
                {
                    num_results = 1;
                    os << "    ";
                    pushvalue(std::ref(os), result_type, result_name.c_str());
                }
                os << "    return " << num_results << ";\n}\n";
            }
        }

        return os;
    }

/*
    auto visit_children_body(CXCursor cursor,
                             CXCursor parent,
                             CXClientData client_data) -> CXChildVisitResult
    {
        auto& stream = *static_cast<std::ostringstream*>(client_data);
        switch (clang_getCursorKind(cursor))
        {
            case CXCursor_Constructor:
                if (Spelling name(cursor); name.c_str() != nullptr)
                {
                    stream << "auto " << root_namespace(cursor) << '_' << to_lower(name) << '(';
                    join_arguments(
                        stream, cursor, [](std::ostream& stream, CXCursor arg) {
                            stream << Spelling(clang_getCursorType(arg)) << ' '
                                   << Spelling(arg);
                        });
                    stream << ") -> " << name << ";\n";
                }
                break;

            default:
                break;
        }

        return CXChildVisit_Continue;
    }
*/
}  // namespace lua

#endif
