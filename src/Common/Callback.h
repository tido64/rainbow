// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_CALLBACK_H_
#define COMMON_CALLBACK_H_

#include <memory>
#include <type_traits>
#include <utility>

namespace rainbow
{
    template <typename T>
    class Callback;

    template <typename R, typename... Args>
    class Callback<R(Args...)>
    {
    public:
        template <typename F>
        constexpr Callback(F&& f) noexcept
            : address_(static_cast<void*>(std::addressof(f)))
        {
            func_ = [](void* address, Args&&... args) -> R {
                return (*reinterpret_cast<std::add_pointer_t<F>>(address))(
                    std::forward<Args>(args)...);
            };
        }

        decltype(auto) operator()(Args... args) const
            noexcept(noexcept(func_(address_, std::forward<Args>(args)...)))
        {
            return func_(address_, std::forward<Args>(args)...);
        }

    private:
        void* address_;
        R (*func_)(void*, Args&&...);
    };
}  // namespace rainbow

#endif
