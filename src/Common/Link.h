// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_LINK_H_
#define COMMON_LINK_H_

#include "Common/Logging.h"

namespace rainbow
{
    template <typename T>
    class Link
    {
    public:
        auto next() const { return next_; }
        auto prev() const { return prev_; }

        void append(T& node)
        {
            node.pop();
            node.prev_ = static_cast<T*>(this);
            if (next_ != nullptr) {
                next_->prev_ = &node;
                node.next_ = next_;
            }
            next_ = &node;
        }

        void pop()
        {
            if (prev_ != nullptr)
                prev_->next_ = next_;

            if (next_ != nullptr) {
                next_->prev_ = prev_;
                next_ = nullptr;
            } else if (prev_ != nullptr) {
                prev_->on_end_link_changed(*prev_);
            }

            prev_ = nullptr;
        }

        template <typename F>
        friend auto for_each(T* node, F&& f) -> bool
        {
            while (node != nullptr) {
                if (f(*node))
                    return true;

                node = node->next();
            }
            return false;
        }

    protected:
        ~Link() { pop(); }

    private:
        T* next_ = nullptr;
        T* prev_ = nullptr;

        virtual void on_end_link_changed(T& new_link)
        {
            R_ASSERT(new_link.next() == nullptr,  //
                     "An end link doesn't have a tail");

            if (prev_ == nullptr)
                return;

            prev_->on_end_link_changed(new_link);
        }
    };
}  // namespace rainbow

#endif
