// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_COMPONENTS_SCRIPTCOMPONENT_H_
#define SCRIPT_COMPONENTS_SCRIPTCOMPONENT_H_

namespace rainbow
{
    class Actor;

    /// <summary>Component for scripting <see cref="Actor"/>s.</summary>
    class IScriptComponent
    {
    public:
        /// <summary>Called on component initialisation.</summary>
        void initialize(Actor& actor) { initialize_impl(actor); }

        /// <summary>Called every frame update.</summary>
        void update(uint64_t dt) { update_impl(dt); }

    protected:
        ~IScriptComponent() = default;

    private:
        virtual void initialize_impl(Actor&) = 0;
        virtual void update_impl(uint64_t dt) = 0;
    };

    class DummyScriptComponent final : public IScriptComponent
    {
    private:
        void initialize_impl(Actor&) override {}
        void update_impl(uint64_t) override {}
    };
}

#endif
