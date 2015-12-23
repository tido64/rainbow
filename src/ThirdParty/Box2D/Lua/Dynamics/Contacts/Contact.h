// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_DYNAMICS_CONTACTS_CONTACT_H_
#define THIRDPARTY_BOX2D_LUA_DYNAMICS_CONTACTS_CONTACT_H_

#include "ThirdParty/Box2D/Lua/Helper.h"

class b2Contact;

NS_B2_LUA_BEGIN
{
	class Contact : public rainbow::lua::Bind<Contact>
	{
		friend Bind;

	public:
		explicit Contact(lua_State*);

		b2Contact* get() const { return contact_; }
		void reset(b2Contact* contact) { contact_ = contact; }

	private:
		static int GetManifold(lua_State*);
		static int GetWorldManifold(lua_State*);
		static int IsTouching(lua_State*);
		static int SetEnabled(lua_State*);
		static int IsEnabled(lua_State*);
		static int GetNext(lua_State*);
		static int GetFixtureA(lua_State*);
		static int GetChildIndexA(lua_State*);
		static int GetFixtureB(lua_State*);
		static int GetChildIndexB(lua_State*);
		static int SetFriction(lua_State*);
		static int GetFriction(lua_State*);
		static int ResetFriction(lua_State*);
		static int SetRestitution(lua_State*);
		static int GetRestitution(lua_State*);
		static int ResetRestitution(lua_State*);
		static int SetTangentSpeed(lua_State*);
		static int GetTangentSpeed(lua_State*);

		b2Contact* contact_;
	};
} NS_B2_LUA_END

#endif
