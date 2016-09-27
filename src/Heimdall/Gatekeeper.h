// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_GATEKEEPER_H_
#define HEIMDALL_GATEKEEPER_H_

#if USE_LUA_SCRIPT
#   include <mutex>
#   include <queue>
#endif  // USE_LUA_SCRIPT

#include "Director.h"
#include "Heimdall/ChangeMonitor.h"
#include "Heimdall/Overlay.h"
#include "Heimdall/OverlayActivator.h"

struct Pointer;

namespace heimdall
{
    /// <summary>Overlay for debugging options.</summary>
    class Gatekeeper final
    {
    public:
        Gatekeeper();

        bool active() const { return director_.active(); }
        auto error() const { return director_.error(); }
        auto input() -> Input& { return director_.input(); }
        auto terminated() const { return director_.terminated(); }

        void draw()
        {
            director_.draw();
            scenegraph_.draw();
        }

        void init(const Vec2i& screen);
        void terminate() { director_.terminate(); }
        void terminate(const char* error) { director_.terminate(error); }
        void update(unsigned long dt);

        void on_focus_gained() { director_.on_focus_gained(); }
        void on_focus_lost() { director_.on_focus_lost(); }
        void on_memory_warning() { director_.on_memory_warning(); }

    private:
        rainbow::Director director_;
#if USE_LUA_SCRIPT
        std::queue<std::unique_ptr<char[]>> changed_files_;
#endif  // USE_LUA_SCRIPT
        Overlay overlay_;
        OverlayActivator overlay_activator_;
        rainbow::GroupNode scenegraph_;
#if USE_LUA_SCRIPT
        std::mutex changed_files_mutex_;
        ChangeMonitor monitor_;
#endif  // USE_LUA_SCRIPT
    };
}

using Director = heimdall::Gatekeeper;

#endif  // HEIMDALL_GATEKEEPER_H_
