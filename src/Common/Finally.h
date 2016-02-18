// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_FINALLY_H_
#define COMMON_FINALLY_H_

#include <utility>

#include "Common/NonCopyable.h"

namespace rainbow
{
    namespace detail
    {
        template <typename F>
        class FinalAct : private NonCopyable<FinalAct<F>>
        {
        public:
            explicit FinalAct(F func)
                : final_act_(std::move(func)), should_invoke_(true)
            {
            }

            FinalAct(FinalAct&&) = delete;

            ~FinalAct()
            {
                if (should_invoke_)
                    final_act_();
            }

            void cancel() { should_invoke_ = false; }

            auto operator=(FinalAct&& other) noexcept -> FinalAct&
            {
                final_act_ = std::move(other.final_act_);
                should_invoke_ = other.should_invoke_;
                other.cancel();
                return *this;
            }

        private:
            F final_act_;
            bool should_invoke_;
        };
    }

    template <typename F>
    auto finally(F&& f) noexcept
    {
        return detail::FinalAct<F>(std::forward<F>(f));
    }
}

#endif
