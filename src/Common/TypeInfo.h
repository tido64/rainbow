// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
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
        auto value() const { return id_; }

        friend bool operator!=(type_id_t a, type_id_t b)
        {
            return !(a.id_ == b.id_);
        }

        friend bool operator==(type_id_t a, type_id_t b)
        {
            return a.id_ == b.id_;
        }

    private:
        template <typename T>
        static auto id()
        {
            static constexpr T* const id{};
            return &id;
        }

        const void* id_;

        constexpr type_id_t(const void* p) : id_(p) {}

        template <typename T>
        friend auto type_id() -> type_id_t;
    };

    /// <summary>Returns a unique id for the specified type.</summary>
    /// <remarks>Ids may not be the same from session to session.</remarks>
    template <typename T>
    auto type_id() -> type_id_t
    {
        return type_id_t::id<std::decay_t<T>>();
    }
}

#endif
