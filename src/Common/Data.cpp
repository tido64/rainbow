// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Data.h"

#include <cstring>

#include "Common/Logging.h"

using rainbow::Data;
using rainbow::czstring;

Data::~Data()
{
    if (ownership_ != Ownership::Owner)
        return;

    operator delete(data_);
}
