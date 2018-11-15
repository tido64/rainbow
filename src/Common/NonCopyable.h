// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_NONCOPYABLE_H_
#define COMMON_NONCOPYABLE_H_

namespace rainbow
{
    /// <summary>
    ///   Prevents objects of a class from being copy-constructed or assigned.
    /// </summary>
    template <typename T>
    class NonCopyable
    {
    public:
        NonCopyable(const NonCopyable&) = delete;
        auto operator=(const NonCopyable&) -> NonCopyable& = delete;

    protected:
        NonCopyable() = default;
        ~NonCopyable() = default;
    };
}  // namespace rainbow

#endif
