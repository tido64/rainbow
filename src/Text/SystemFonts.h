// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef TEXT_SYSTEMFONTS_H_
#define TEXT_SYSTEMFONTS_H_

#include "Common/Data.h"

namespace rainbow::text
{
    auto monospace_font() -> Data;
    auto monospace_font_path() -> czstring;
}

#endif
