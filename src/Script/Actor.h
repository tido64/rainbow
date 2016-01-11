// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_ACTOR_H_
#define SCRIPT_ACTOR_H_

#include "Script/Components/AnimationComponent.h"
#include "Script/Components/RenderComponent.h"
#include "Script/Components/ScriptComponent.h"

namespace rainbow
{
	class Actor
	{
	public:
		virtual ~Actor() = default;

		virtual auto animation_component() -> IAnimationComponent&
		{
			R_ABORT("Attempted access to a non-existing animation component.");
			UNREACHABLE();
			return *static_cast<IAnimationComponent*>(nullptr);
		}

		auto is_enabled() const { return render_component().is_enabled(); }
		auto is_mirrored() const { return render_component().is_mirrored(); }
		auto node() const { return render_component().node(); }
		auto position() const { return render_component().position(); }

		/// <summary>Sets actor position.</summary>
		virtual void set_position(const Vec2f& p)
		{
			render_component().set_position(p);
		}

		/// <summary>Sets local angle of rotation (in radian).</summary>
		void set_local_rotation(float r)
		{
			render_component().set_local_rotation(r);
		}

		/// <summary>Sets uniform local scaling factor.</summary>
		/// <param name="f">Scaling factor for both axes.</param>
		void set_local_scale(float f) { render_component().set_local_scale(f); }

		/// <summary>Sets non-uniform local scaling factor.</summary>
		/// <param name="f">Scaling factors for x- and y-axis.</param>
		void set_local_scale(const Vec2f& scale)
		{
			render_component().set_local_scale(scale);
		}

		/// <summary>Disables actor.</summary>
		void disable()
		{
			render_component().disable();
			on_disabled();
		}

		/// <summary>Called on actor destruction.</summary>
		void dispose() { on_unload(); }

		/// <summary>Enables actor.</summary>
		void enable()
		{
			render_component().enable();
			on_enabled();
		}

		/// <summary>Hides actor if it is currently shown.</summary>
		void hide() { render_component().hide(); }

		/// <summary>Called on actor creation.</summary>
		void initialize()
		{
			initialize_components();
			on_initialized();
		}

		/// <summary>Mirrors actor.</summary>
		void mirror() { render_component().mirror(); }

		/// <summary>Moves actor by (x,y).</summary>
		virtual void move(const Vec2f& delta)
		{
			render_component().move(delta);
		}

		/// <summary>Shows actor if it is currently hidden.</summary>
		void show() { render_component().show(); }

		/// <summary>Called every frame update.</summary>
		void update(unsigned long dt)
		{
			if (!is_enabled())
				return;

			script_component().update(dt);
		}

	protected:
		auto render_component() -> RenderComponent&
		{
			return render_component_;
		}

		auto render_component() const -> const RenderComponent&
		{
			return render_component_;
		}

		virtual auto script_component() -> IScriptComponent&
		{
			static DummyScriptComponent dummy;
			return dummy;
		}

		void initialize_components()
		{
			initialize_render_component();
			node()->add_child(render_component().batch());
			initialize_animation_component(*node());
			script_component().initialize(*this);
		}

		/// <summary>Called when all components have been initialised.</summary>
		virtual void on_initialized() {}

	private:
		RenderComponent render_component_;

		/// <summary>
		///   Called when the animation component should be initialised.
		/// </summary>
		virtual void initialize_animation_component(SceneNode&) {}

		/// <summary>
		///   Called when the render component should be initialised.
		/// </summary>
		virtual void initialize_render_component() = 0;

		/// <summary>Called when actor has been disabled.</summary>
		virtual void on_disabled() {}

		/// <summary>Called when actor has been enabled.</summary>
		virtual void on_enabled() {}

		/// <summary>Called when actor is being disposed.</summary>
		virtual void on_unload() {}
	};
}

#endif
