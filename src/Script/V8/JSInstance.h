// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <memory>

#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <v8.h>
#ifdef __GNUC__
#    pragma GCC diagnostic pop
#endif

#include "Math/Vec2.h"

namespace v8
{
    class Platform;
}

namespace rainbow
{
    class Data;

    class JSInstance
    {
    public:
        JSInstance();
        ~JSInstance();

        void init(const Vec2i& screen_size) { init_impl(screen_size); }
        void update(uint64_t dt) { update_impl(dt); }

    private:
        v8::Isolate* isolate_ = nullptr;
        v8::Eternal<v8::Context> context_;
        v8::Eternal<v8::Function> update_;
        v8::Eternal<v8::Function> init_;
        std::unique_ptr<v8::ArrayBuffer::Allocator> allocator_;
        std::unique_ptr<v8::Platform> platform_;

        void init_impl(const Vec2i& screen_size);
        void update_impl(uint64_t);
        void on_memory_warning_impl();
    };
}  // namespace rainbow
