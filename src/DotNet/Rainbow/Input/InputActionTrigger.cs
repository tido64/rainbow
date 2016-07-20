// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Input
{
    public struct InputActionTrigger
    {
        public ControllerAxis ControllerAxis { get; set; }

        public ControllerButton ControllerButton { get; set; }

        public VirtualKey VirtualKey { get; set; }
    }
}
