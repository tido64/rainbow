// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_COMPONENTS_STATECOMPONENT_H_
#define SCRIPT_COMPONENTS_STATECOMPONENT_H_

#include "Common/Logging.h"
#include "Script/Components/ScriptComponent.h"

namespace rainbow
{
	class StateComponent;

	class IState
	{
	public:
		auto next() const { return next_; }

		virtual void update(StateComponent&, Actor&, unsigned long dt) = 0;

		virtual void on_enter(StateComponent&, Actor&) {}
		virtual void on_exit(StateComponent&, Actor&) {}

		virtual auto to_string() const -> const char* = 0;

	protected:
		IState() : next_(nullptr) {}
		~IState() = default;

	private:
		IState* next_;

		friend StateComponent;
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
			     this);

			auto state = top_;
			while (state)
			{
				LOGD("   | %s (%p)", state->to_string(), state);
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
			state.next_ = top_;
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

			prev->next_ = state.next();
			exit_state(state);
		}

		void set_state(IState& state)
		{
			if (top_)
				top_->on_exit(*this, actor());
			top_ = &state;
			state.on_enter(*this, actor());
		}

		// IScriptComponent details.

		void initialize(Actor& actor) override { actor_ = &actor; }

		void update(unsigned long dt) override
		{
			state()->update(*this, actor(), dt);
		}

	protected:
		auto actor() -> Actor& { return *actor_; }

	private:
		IState* top_ = nullptr;
		Actor* actor_ = nullptr;

		void exit_state(IState& state)
		{
			state.next_ = nullptr;
			state.on_exit(*this, actor());
		}

		IState* state_before(const IState& state) const
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
	};
}

#endif
