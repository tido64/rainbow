// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_NANOVG_NANOVG_H_
#define THIRDPARTY_NANOVG_NANOVG_H_

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   ifdef __clang__
#       pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#       pragma clang diagnostic ignored "-Wnested-anon-types"
#   else
#       pragma GCC diagnostic ignored "-pedantic"
#   endif
#endif
#include <nanovg.h>
#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

#include "Graphics/Drawable.h"

namespace nvg
{
    class Canvas : public IDrawable
    {
    public:
        Canvas();
        ~Canvas() override;

    protected:
        auto context() const { return context_; }

    private:
        NVGcontext* context_;

        void draw_impl() final;

        virtual void paint_impl() = 0;
    };
}

#endif
