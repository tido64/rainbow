// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_TYPEINFO_H_
#define COMMON_TYPEINFO_H_

#include <type_traits>

namespace rainbow
{
	class type_id_t
	{
	public:
		friend bool operator!=(const type_id_t a, const type_id_t b)
		{
			return !(a.id_ == b.id_);
		}

		friend bool operator==(const type_id_t a, const type_id_t b)
		{
			return a.id_ == b.id_;
		}

	private:
		template <typename T>
		static type_id_t id()
		{
			static constexpr T* const id{};
			return &id;
		}

		const void* id_;

		constexpr type_id_t(const void* p) : id_(p) {}

		template <typename T>
		friend type_id_t type_id();
	};

	/// <summary>Returns a unique id for the specified type.</summary>
	/// <remarks>Ids may not be the same from session to session.</remarks>
	template <typename T>
	type_id_t type_id()
	{
		return type_id_t::id<typename std::decay<T>::type>();
	}
}

#endif
