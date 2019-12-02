// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_GATEKEEPER_H_
#define HEIMDALL_GATEKEEPER_H_

#include "Director.h"
#include "Heimdall/Overlay.h"
#include "Heimdall/OverlayActivator.h"

namespace heimdall
{
    /// <summary>Overlay for debugging options.</summary>
    class Gatekeeper final
    {
    public:
        Gatekeeper() : overlay_(director_), overlay_activator_(&overlay_) {}

        void init(const rainbow::Vec2i& screen);

        [[nodiscard]] auto active() const { return director_.active(); }
        [[nodiscard]] auto error() const { return director_.error(); }

        auto graphics_context() -> rainbow::graphics::Context&
        {
            return director_.graphics_context();
        }

        auto input() -> rainbow::Input& { return director_.input(); }
        [[nodiscard]] auto terminated() const { return director_.terminated(); }

        void draw()
        {
            director_.draw();
            overlay_.draw();
        }

        void show_diagnostic_tools() { overlay_.enable(); }

        void terminate() { director_.terminate(); }
        void terminate(std::error_code error) { director_.terminate(error); }
        void update(uint64_t dt);

        void on_focus_gained() { director_.on_focus_gained(); }
        void on_focus_lost() { director_.on_focus_lost(); }
        void on_memory_warning() { director_.on_memory_warning(); }

    private:
        rainbow::Director director_;
        Overlay overlay_;
        OverlayActivator overlay_activator_;
    };
}  // namespace heimdall

#ifdef USE_HEIMDALL
using Director = heimdall::Gatekeeper;
#endif  // USE_HEIMDALL

#endif  // HEIMDALL_GATEKEEPER_H_
