// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_COMPONENTS_STATECOMPONENT_H_
#define SCRIPT_COMPONENTS_STATECOMPONENT_H_

#include "Common/Logging.h"
#include "Common/Passkey.h"
#include "Script/Components/ScriptComponent.h"

namespace rainbow
{
    class StateComponent;

    class IState
    {
    public:
        auto next() const { return next_; }

        void set_next(IState* next, const Passkey<StateComponent>&)
        {
            next_ = next;
        }

        void update(StateComponent& component, Actor& actor, uint64_t dt)
        {
            update_impl(component, actor, dt);
        }

        void on_enter(StateComponent& component, Actor& actor)
        {
            on_enter_impl(component, actor);
        }

        void on_exit(StateComponent& component, Actor& actor)
        {
            on_exit_impl(component, actor);
        }

        auto to_string() const -> const char* { return to_string_impl(); }

    protected:
        IState() : next_(nullptr) {}
        ~IState() = default;

    private:
        IState* next_;

        virtual void update_impl(StateComponent&, Actor&, uint64_t dt) = 0;
        virtual void on_enter_impl(StateComponent&, Actor&) {}
        virtual void on_exit_impl(StateComponent&, Actor&) {}
        virtual auto to_string_impl() const -> const char* = 0;
    };

    class StateComponent : public IScriptComponent
    {
    public:
        auto state() const { return top_; }

        bool contains(const IState& state) const
        {
            return &state == top_ || state_before(state) != nullptr;
        }

        void dump() const
        {
            LOGD("<StateComponent:%p>\n"
                 "  Top\n"
                 "   ↑",
                 static_cast<const void*>(this));

            auto state = top_;
            while (state)
            {
                LOGD("   | %s (%p)",
                     state->to_string(),
                     static_cast<const void*>(state));
                state = state->next();
            }

            LOGD("   ⊥");
        }

        void pop_state()
        {
            if (!top_)
                return;

            auto& state = *top_;
            top_ = state.next();
            exit_state(state);
        }

        void push_state(IState& state)
        {
            state.set_next(top_, {});
            top_ = &state;
            state.on_enter(*this, actor());
        }

        void remove_state(IState& state)
        {
            if (&state == top_)
            {
                pop_state();
                return;
            }

            auto prev = state_before(state);
            if (!prev)
                return;

            prev->set_next(state.next(), {});
            exit_state(state);
        }

        void set_state(IState& state)
        {
            if (top_)
                top_->on_exit(*this, actor());
            top_ = &state;
            state.on_enter(*this, actor());
        }

    protected:
        auto actor() -> Actor& { return *actor_; }

    private:
        IState* top_ = nullptr;
        Actor* actor_ = nullptr;

        void exit_state(IState& state)
        {
            state.set_next(nullptr, {});
            state.on_exit(*this, actor());
        }

        auto state_before(const IState& state) const -> IState*
        {
            auto i = top_;
            while (i)
            {
                if (i->next() == &state)
                    return i;

                i = i->next();
            }
            return nullptr;
        }

        // IScriptComponent details.

        void initialize_impl(Actor& actor) override { actor_ = &actor; }

        void update_impl(uint64_t dt) override
        {
            state()->update(*this, actor(), dt);
        }
    };
}

#endif
