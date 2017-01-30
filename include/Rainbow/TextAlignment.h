// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef RAINBOW_TEXTALIGNMENT_H_
#define RAINBOW_TEXTALIGNMENT_H_

namespace rainbow
{
    enum class TextAlignment
    {
        Left,
        Right,
        Center
    };

    inline auto text_alignment_from_char(char c)
    {
        switch (c)
        {
            case 'c':
                return TextAlignment::Center;
            case 'r':
                return TextAlignment::Right;
            default:
                return TextAlignment::Left;
        }
    }
}

#endif
