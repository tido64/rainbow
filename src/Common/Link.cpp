// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Link.h"

#include "Common/Logging.h"

using rainbow::Link;

void Link::append(Link& node)
{
    node.pop();
    node.prev_ = this;
    if (next_ != nullptr)
    {
        next_->prev_ = &node;
        node.next_ = next_;
    }
    next_ = &node;
}

void Link::pop()
{
    if (prev_ != nullptr)
        prev_->next_ = next_;

    if (next_ != nullptr)
    {
        next_->prev_ = prev_;
        next_ = nullptr;
    }
    else if (prev_ != nullptr)
    {
        on_end_link_changed(*prev_);
    }

    prev_ = nullptr;
}

void Link::on_end_link_changed(Link& new_link)
{
    R_ASSERT(new_link.next() == nullptr, "An end link doesn't have a tail");

    if (prev_ == nullptr)
        return;

    prev_->on_end_link_changed(new_link);
}
