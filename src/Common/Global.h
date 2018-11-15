// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_GLOBAL_H_
#define COMMON_GLOBAL_H_

#include "Common/Logging.h"
#include "Common/NonCopyable.h"

namespace rainbow
{
    template <typename T>
    class Global : private NonCopyable<Global<T>>
    {
    public:
        static auto Get() { return s_instance; }

    protected:
        Global() = default;
        ~Global() { s_instance = nullptr; }

        bool is_global() const
        {
            return s_instance == static_cast<const T*>(this);
        }

        void make_global()
        {
            R_ASSERT(s_instance == nullptr, "An instance already exists");
            s_instance = static_cast<T*>(this);
        }

    private:
        static T* s_instance;
    };

    template <typename T>
    T* Global<T>::s_instance = nullptr;
}  // namespace rainbow

#endif
