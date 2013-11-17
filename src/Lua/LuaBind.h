// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUABIND_H_
#define LUA_LUABIND_H_

#include "Common/SharedPtr.h"

struct lua_State;

namespace Rainbow
{
	namespace Lua
	{
		enum BindType
		{
			kBindTypeDerived,  ///< Binding object is derived from class.
			kBindTypeStrong,   ///< Binding object has a strong reference to class instance.
			kBindTypeWeak      ///< Binding object has a weak reference to class instance.
		};

		template<class L, class T, BindType type>
		class Bind;

		template<class T>
		struct Method
		{
			const char *const name;
			int (T::*lua_CFunction)(lua_State *);
		};

		template<class L, class T>
		class Bind<L, T, kBindTypeDerived>
		{
		public:
			static const char class_name[];
			static const Method<L> methods[];

			T* get()
			{
				return static_cast<T*>(static_cast<L*>(this));
			}

		protected:
			~Bind() = default;
		};

		template<class L, class T>
		class Bind<L, T, kBindTypeStrong>
		{
		public:
			static const char class_name[];
			static const Method<L> methods[];

			Bind() = default;

			T* get() const
			{
				return this->ptr.get();
			}

		protected:
			SharedPtr<T> ptr;

			~Bind() = default;
		};

		template<class L, class T>
		class Bind<L, T, kBindTypeWeak>
		{
		public:
			static const char class_name[];
			static const Method<L> methods[];

			Bind() : ptr(nullptr) { }
			Bind(T *ptr) : ptr(ptr) { }

			T* get() const
			{
				return this->ptr;
			}

		protected:
			T *ptr;

			~Bind() = default;
		};
	}
}

#endif
