// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COLLISION_SAT_H_
#define COLLISION_SAT_H_

namespace rainbow
{
    struct Quad;

    auto overlaps(const Quad& a, float ar, const Quad& b, float br) -> bool;
}  // namespace rainbow

#endif
