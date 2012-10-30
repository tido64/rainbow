#ifndef HEIMDALL_GATEKEEPER_H_
#define HEIMDALL_GATEKEEPER_H_

#ifdef USE_HEIMDALL

#include "Director.h"
#include "Common/Vector.h"
#include "Graphics/Label.h"
#include "Graphics/SpriteBatch.h"

namespace Heimdall
{
	/// Overlay for debugging options.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Gatekeeper : public NonCopyable<Gatekeeper>
	{
	public:
		Gatekeeper();
		~Gatekeeper();

		inline void draw();

		void init(const Data &);
		void set_video(const int width, const int height);

		inline void toggle_info();

		void update(const unsigned long t = 0);

		inline void on_memory_warning();

	private:
		float time;
		int width;
		int height;

		Rainbow::Director *director;
		SmartPtr<FontAtlas> font;

		SceneGraph::Node *info_node;
		SceneGraph::Node scenegraph;

		Label info_label;

		char info_text[128];
	};

	void Gatekeeper::draw()
	{
		this->director->draw();
		this->scenegraph.draw();
	}

	void Gatekeeper::toggle_info()
	{
		this->info_node->enabled = !this->info_node->enabled;
	}

	void Gatekeeper::on_memory_warning()
	{
		this->director->on_memory_warning();
	}
}

typedef Heimdall::Gatekeeper Director;

#endif  // USE_HEIMDALL
#endif  // HEIMDALL_GATEKEEPER_H_
