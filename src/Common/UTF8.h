// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_UTF8_H_
#define COMMON_UTF8_H_

#include <cstdint>

namespace rainbow
{
    const uint8_t kUTF8Accept = 0;
    const uint8_t kUTF8Reject = 0xf;

    uint8_t utf8_decode_step(uint8_t state, uint8_t octet, uint32_t* cpp);

    /// <summary>
    ///   Applies the given function <paramref name="f"/> to each valid UTF-8
    ///   code point in <paramref name="str"/>.
    /// </summary>
    template <typename F>
    void for_each_utf8(const char* str, F&& f)
    {
        uint32_t code_point;
        uint8_t state = kUTF8Accept;
        for (; *str; ++str)
        {
            state = utf8_decode_step(state, *str, &code_point);
            if (state == kUTF8Accept)
                f(code_point);
            else if (state == kUTF8Reject)
                break;
        }
    }
}

#endif
