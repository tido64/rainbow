// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Dynamics/World.h"

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#include <Box2D/Dynamics/b2Body.h>
#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

#include "ThirdParty/Box2D/DebugDraw.h"
#include "ThirdParty/Box2D/Lua/Dynamics/Body.h"
#include "ThirdParty/Box2D/Lua/Dynamics/Contacts/Contact.h"
#include "ThirdParty/Box2D/StableWorld.h"

namespace b2
{
    extern std::unique_ptr<b2::DebugDraw> g_debug_draw;

    int g_contact = LUA_NOREF;

    void on_contact(lua_State* L,
                    int listener,
                    const char* event,
                    b2Contact* contact)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, listener);
        lua_getfield(L, -1, event);
        if (!lua_isfunction(L, -1))
        {
            lua_pop(L, 2);
            return;
        }
        lua_insert(L, -2);

        lua_rawgeti(L, LUA_REGISTRYINDEX, g_contact);
        auto self = rainbow::lua::touserdata<lua::Contact>(L, -1);
        self->reset(contact);

        rainbow::lua::call(L, 2, 0, 0, "Failed to evaluate '%s'", event);
    }
}

NS_B2_LUA_BEGIN
{
    constexpr bool World::is_constructible;

    const char World::class_name[] = "World";

    const luaL_Reg World::functions[]{
        {"GetPTM",                  &World::GetPTM},
        {"SetPTM",                  &World::SetPTM},
        {"SetDestructionListener",  &World::SetDestructionListener},
        {"SetContactFilter",        &World::SetContactFilter},
        {"SetContactListener",      &World::SetContactListener},
        {"SetDebugDraw",            &World::SetDebugDraw},
        {"CreateBody",              &World::CreateBody},
        {"DestroyBody",             &World::DestroyBody},
        {"CreateJoint",             &World::CreateJoint},
        {"DestroyJoint",            &World::DestroyJoint},
        {"Step",                    &World::Step},
        {"QueryAABB",               &World::QueryAABB},
        {"RayCast",                 &World::RayCast},
        {"GetBodyList",             &World::GetBodyList},
        {"GetJointList",            &World::GetJointList},
        {"GetContactList",          &World::GetContactList},
        {"SetAllowSleeping",        &World::SetAllowSleeping},
        {"GetAllowSleeping",        &World::GetAllowSleeping},
        {"SetWarmStarting",         &World::SetWarmStarting},
        {"GetWarmStarting",         &World::GetWarmStarting},
        {"SetContinuousPhysics",    &World::SetContinuousPhysics},
        {"GetContinuousPhysics",    &World::GetContinuousPhysics},
        {"SetSubStepping",          &World::SetSubStepping},
        {"GetSubStepping",          &World::GetSubStepping},
        {"GetProxyCount",           &World::GetProxyCount},
        {"GetBodyCount",            &World::GetBodyCount},
        {"GetJointCount",           &World::GetJointCount},
        {"GetContactCount",         &World::GetContactCount},
        {"GetTreeHeight",           &World::GetTreeHeight},
        {"GetTreeBalance",          &World::GetTreeBalance},
        {"GetTreeQuality",          &World::GetTreeQuality},
        {"SetGravity",              &World::SetGravity},
        {"GetGravity",              &World::GetGravity},
        {"IsLocked",                &World::IsLocked},
        {"ShiftOrigin",             &World::ShiftOrigin},
        {"GetContactManager",       &World::GetContactManager},
        {"GetProfile",              &World::GetProfile},
        {"Dump",                    &World::Dump},
        {nullptr,                   nullptr}};

    World::World(lua_State* L) : contact_listener_(LUA_NOREF), state_(L)
    {
        rainbow::lua::checkargs<rainbow::lua::nil_or<lua_Number>,
                                rainbow::lua::nil_or<lua_Number>>(L);

        switch (lua_gettop(L))
        {
            case 0:
                world_ = std::make_unique<StableWorld>();
                break;
            case 1:
                world_ = std::make_unique<StableWorld>(lua_tonumber(L, 1));
                break;
            case 2:
                world_ = std::make_unique<StableWorld>(
                    lua_tonumber(L, 1), lua_tonumber(L, 2));
                break;
            default:
                luaL_argerror(L, 3, "0-2 parameters expected");
                break;
        }
    }

    World::~World()
    {
        if (contact_listener_ != LUA_NOREF)
            luaL_unref(state_, LUA_REGISTRYINDEX, contact_listener_);
    }

    void World::BeginContact(b2Contact* contact)
    {
        if (contact_listener_ == LUA_NOREF)
            return;

        on_contact(state_, contact_listener_, "BeginContact", contact);
    }

    void World::EndContact(b2Contact* contact)
    {
        if (contact_listener_ == LUA_NOREF)
            return;

        on_contact(state_, contact_listener_, "EndContact", contact);
    }

    void World::PreSolve(b2Contact* contact, const b2Manifold*)
    {
        if (contact_listener_ == LUA_NOREF)
            return;

        on_contact(state_, contact_listener_, "PreSolve", contact);
    }

    void World::PostSolve(b2Contact* contact, const b2ContactImpulse*)
    {
        if (contact_listener_ == LUA_NOREF)
            return;

        on_contact(state_, contact_listener_, "PostSolve", contact);
    }

    int World::GetPTM(lua_State* L)
    {
        return get1f(L, [](const StableWorld* world) {
            return world->GetPTM();
        });
    }

    int World::SetPTM(lua_State* L)
    {
        return set1f(L, [](StableWorld* world, float ptm) {
            world->SetPTM(ptm);
        });
    }

    int World::SetDestructionListener(lua_State*) { return -1; }

    int World::SetContactFilter(lua_State*) { return -1; }

    int World::SetContactListener(lua_State* L)
    {
        rainbow::lua::checkargs<World, rainbow::lua::nil_or<void*>>(L);

        World* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        if (self->contact_listener_ != LUA_NOREF)
            luaL_unref(L, LUA_REGISTRYINDEX, self->contact_listener_);
        if (lua_isnil(L, 2))
        {
            self->contact_listener_ = LUA_NOREF;
            return 0;
        }
        if (g_contact == LUA_NOREF)
        {
            rainbow::lua::alloc<Contact>(L);
            g_contact = luaL_ref(L, LUA_REGISTRYINDEX);
        }
        lua_settop(L, 2);
        self->contact_listener_ = luaL_ref(L, LUA_REGISTRYINDEX);
        self->get()->SetContactListener(self);
        return 0;
    }

    int World::SetDebugDraw(lua_State* L)
    {
        return set1b(L, [](StableWorld* world, bool enable) {
            world->SetDebugDraw(enable ? g_debug_draw.get() : nullptr);
        });
    }

    int World::CreateBody(lua_State* L)
    {
        rainbow::lua::checkargs<World, void*>(L);

        World* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        const b2BodyDef& bd = GetBodyDef(L);
        lua_pushlightuserdata(L, self->get()->CreateBody(&bd));
        return rainbow::lua::alloc<Body>(L);
    }

    int World::DestroyBody(lua_State* L)
    {
        rainbow::lua::checkargs<World, Body>(L);

        World* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->get()->DestroyBody(rainbow::lua::touserdata<Body>(L, 2)->get());
        return 0;
    }

    int World::CreateJoint(lua_State*) { return -1; }

    int World::DestroyJoint(lua_State*) { return -1; }

    int World::Step(lua_State* L)
    {
        rainbow::lua::checkargs<World,
                                lua_Number,
                                rainbow::lua::nil_or<lua_Number>,
                                rainbow::lua::nil_or<lua_Number>>(L);

        World* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->get()->Step(
            lua_tonumber(L, 2),
            static_cast<int32>(rainbow::lua::optinteger(L, 3, 8)),
            static_cast<int32>(rainbow::lua::optinteger(L, 4, 3)));
        return 0;
    }

    int World::QueryAABB(lua_State*) { return -1; }

    int World::RayCast(lua_State*) { return -1; }

    int World::GetBodyList(lua_State* L)
    {
        return get1ud<Body>(L, [](StableWorld* world) {
            return world->GetBodyList();
        });
    }

    int World::GetJointList(lua_State*) { return -1; }

    int World::GetContactList(lua_State*) { return -1; }

    int World::SetAllowSleeping(lua_State* L)
    {
        return set1b(L, [](StableWorld* world, bool allow) {
            world->SetAllowSleeping(allow);
        });
    }

    int World::GetAllowSleeping(lua_State* L)
    {
        return get1b(L, [](const StableWorld* world) {
            return world->GetAllowSleeping();
        });
    }

    int World::SetWarmStarting(lua_State* L)
    {
        return set1b(L, [](StableWorld* world, bool warm) {
            world->SetWarmStarting(warm);
        });
    }

    int World::GetWarmStarting(lua_State* L)
    {
        return get1b(L, [](const StableWorld* world) {
            return world->GetWarmStarting();
        });
    }

    int World::SetContinuousPhysics(lua_State* L)
    {
        return set1b(L, [](StableWorld* world, bool continuous) {
            world->SetContinuousPhysics(continuous);
        });
    }

    int World::GetContinuousPhysics(lua_State* L)
    {
        return get1b(L, [](const StableWorld* world) {
            return world->GetContinuousPhysics();
        });
    }

    int World::SetSubStepping(lua_State* L)
    {
        return set1b(L, [](StableWorld* world, bool sub_stepping) {
            world->SetSubStepping(sub_stepping);
        });
    }

    int World::GetSubStepping(lua_State* L)
    {
        return get1b(L, [](const StableWorld* world) {
            return world->GetSubStepping();
        });
    }

    int World::GetProxyCount(lua_State* L)
    {
        return get1i(L, [](const StableWorld* world) {
            return world->GetProxyCount();
        });
    }

    int World::GetBodyCount(lua_State* L)
    {
        return get1i(L, [](const StableWorld* world) {
            return world->GetBodyCount();
        });
    }

    int World::GetJointCount(lua_State* L)
    {
        return get1i(L, [](const StableWorld* world) {
            return world->GetJointCount();
        });
    }

    int World::GetContactCount(lua_State* L)
    {
        return get1i(L, [](const StableWorld* world) {
            return world->GetContactCount();
        });
    }

    int World::GetTreeHeight(lua_State* L)
    {
        return get1i(L, [](const StableWorld* world) {
            return world->GetTreeHeight();
        });
    }

    int World::GetTreeBalance(lua_State* L)
    {
        return get1i(L, [](const StableWorld* world) {
            return world->GetTreeBalance();
        });
    }

    int World::GetTreeQuality(lua_State* L)
    {
        return get1f(L, [](const StableWorld* world) {
            return world->GetTreeQuality();
        });
    }

    int World::SetGravity(lua_State* L)
    {
        return set2f(L, [](StableWorld* world, float gx, float gy) {
            world->SetGravity(b2Vec2(gx, gy));
        });
    }

    int World::GetGravity(lua_State* L)
    {
        return get1fv(L, [](const StableWorld* world) {
            return world->GetGravity();
        });
    }

    int World::IsLocked(lua_State* L)
    {
        return get1b(L, [](const StableWorld* world) {
            return world->IsLocked();
        });
    }

    int World::ShiftOrigin(lua_State* L)
    {
        return set2f(L, [](StableWorld* world, float x0, float y0) {
            world->ShiftOrigin(b2Vec2(x0, y0));
        });
    }

    int World::GetContactManager(lua_State*) { return -1; }

    int World::GetProfile(lua_State*) { return -1; }

    int World::Dump(lua_State* L)
    {
        World* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->get()->Dump();
        return 0;
    }
} NS_B2_LUA_END
