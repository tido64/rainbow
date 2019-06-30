// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_DEFER_H_
#define COMMON_DEFER_H_

#include <utility>

#include "Common/NonCopyable.h"

namespace rainbow
{
    template <typename F>
    class Defer : private NonCopyable<Defer<F>>
    {
    public:
        ~Defer() noexcept { f_(); }

    private:
        F f_;

        Defer(F&& f) noexcept : f_(std::forward<F>(f)) {}

        Defer(Defer&&) = delete;
        auto operator=(Defer &&) -> Defer& = delete;

        template <typename G>
        friend auto defer(G&& g) noexcept -> Defer<G>;
    };

    template <typename F>
    auto defer(F&& f) noexcept -> Defer<F>
    {
        return Defer<F>(std::forward<F>(f));
    }
}  // namespace rainbow

#endif
