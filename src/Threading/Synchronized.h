// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THREADING_SYNCHRONIZED_H_
#define THREADING_SYNCHRONIZED_H_

#include <mutex>
#include <utility>

#include "Common/NonCopyable.h"

namespace rainbow
{
    template <typename T>
    class Synchronized
    {
        class LockedPtr : private NonCopyable<LockedPtr>
        {
        public:
            LockedPtr(std::mutex& mutex, T& value) : lock_(mutex), value_(value)
            {
            }

            LockedPtr(LockedPtr&&) = delete;
            auto operator=(LockedPtr&&) ->LockedPtr& = delete;

            template <typename F>
            void invoke_unlocked(F&& func)
            {
                lock_.unlock();
                func(*this);
                lock_.lock();
            }

            auto operator->() -> T* { return &value_; }
            auto operator*() -> T& { return value_; }

        private:
            std::unique_lock<std::mutex> lock_;
            T& value_;
        };

    public:
        using value_type = T;

        template <typename... Args>
        Synchronized(Args&&... args) : value_(std::forward<Args>(args)...)
        {
        }

        template <typename F>
        auto invoke(F&& func)
        {
            LockedPtr ptr{mutex_, value_};
            return func(ptr);
        }

        auto operator->() -> LockedPtr { return {mutex_, value_}; }

    private:
        std::mutex mutex_;
        value_type value_;
    };
}

#endif
