// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_DYNAMICS_CONTACTS_CONTACT_H_
#define THIRDPARTY_BOX2D_LUA_DYNAMICS_CONTACTS_CONTACT_H_

#include "ThirdParty/Box2D/Lua/Helper.h"

class b2Contact;

namespace b2 { namespace lua
{
    class Contact : private rainbow::lua::Object<Contact>
    {
    public:
        LUA_REG_OBJECT_PROPS(false)

        explicit Contact(lua_State*);

        auto get() const { return contact_; }
        void reset(b2Contact* contact) { contact_ = contact; }

    private:
        static auto GetManifold(lua_State*) -> int;
        static auto GetWorldManifold(lua_State*) -> int;
        static auto IsTouching(lua_State*) -> int;
        static auto SetEnabled(lua_State*) -> int;
        static auto IsEnabled(lua_State*) -> int;
        static auto GetNext(lua_State*) -> int;
        static auto GetFixtureA(lua_State*) -> int;
        static auto GetChildIndexA(lua_State*) -> int;
        static auto GetFixtureB(lua_State*) -> int;
        static auto GetChildIndexB(lua_State*) -> int;
        static auto SetFriction(lua_State*) -> int;
        static auto GetFriction(lua_State*) -> int;
        static auto ResetFriction(lua_State*) -> int;
        static auto SetRestitution(lua_State*) -> int;
        static auto GetRestitution(lua_State*) -> int;
        static auto ResetRestitution(lua_State*) -> int;
        static auto SetTangentSpeed(lua_State*) -> int;
        static auto GetTangentSpeed(lua_State*) -> int;

        b2Contact* contact_;
    };
}}  // namespace b2::lua

#endif
