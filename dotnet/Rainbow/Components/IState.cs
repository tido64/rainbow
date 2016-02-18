// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Components
{
    public interface IState
    {
        void Update(object sender, ulong dt);

        void OnEnter(object sender, IState oldState);

        void OnExit(object sender, IState newState);
    }
}
