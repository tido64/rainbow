// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
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
		virtual void initialize(Actor&) = 0;

		/// <summary>Called every frame update.</summary>
		virtual void update(unsigned long dt) = 0;

	protected:
		~IScriptComponent() = default;
	};

	class DummyScriptComponent final : public IScriptComponent
	{
	public:
		void initialize(Actor&) override {}
		void update(unsigned long) override {}
	};
}

#endif
