// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_PASSKEY_H_
#define COMMON_PASSKEY_H_

#include "Common/NonCopyable.h"

namespace rainbow
{
    template <typename T>
    class Passkey : private NonCopyable<Passkey<T>>
    {
        Passkey() = default;

        friend T;
    };
}  // namespace rainbow

#endif
