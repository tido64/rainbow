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

		inline void pause();
		inline void step();

		inline void toggle_info();

		void update(const unsigned long t = 0);

		inline void on_memory_warning();

	private:
		bool paused;
		int width;
		int height;
		size_t time_i;

		Rainbow::Director *director;
		SmartPtr<FontAtlas> font;

		SceneGraph::Node *info_node;
		SceneGraph::Node scenegraph;

		char fps_text[64];
		char tex_text[64];
		Label fps_label;
		Label tex_label;

		unsigned long time[10];
	};

	void Gatekeeper::draw()
	{
		this->director->draw();
		this->scenegraph.draw();
	}

	void Gatekeeper::pause()
	{
		this->paused = !this->paused;
	}

	void Gatekeeper::toggle_info()
	{
		this->info_node->enabled = !this->info_node->enabled;
	}

	void Gatekeeper::step()
	{
		this->update(1000 / 60);
		this->paused = true;
	}

	void Gatekeeper::on_memory_warning()
	{
		this->director->on_memory_warning();
	}
}

typedef Heimdall::Gatekeeper Director;

#endif  // USE_HEIMDALL
#endif  // HEIMDALL_GATEKEEPER_H_
