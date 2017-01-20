// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_LINK_H_
#define COMMON_LINK_H_

#include "Common/Constraints.h"

namespace rainbow
{
    class Link
    {
    public:
        auto next() const { return next_; }
        auto prev() const { return prev_; }

        void append(Link& node);
        void pop();

        template <typename T, typename F, typename = EnableIfBaseOf<Link, T>>
        friend auto for_each(T* node, F&& f) -> bool
        {
            while (node != nullptr)
            {
                if (f(*node))
                    return true;

                node = node->next();
            }
            return false;
        }

    protected:
        ~Link() { pop(); }

    private:
        Link* next_ = nullptr;
        Link* prev_ = nullptr;

        virtual void on_end_link_changed(Link& new_link);
    };
}

#endif
