#ifdef USE_HEIMDALL

#include "Common/Chrono.h"
#include "Common/Data.h"
#include "Graphics/TextureManager.h"
#include "Heimdall/Gatekeeper.h"
#include "Resources/Inconsolata.otf.h"

namespace Heimdall
{
	Gatekeeper::Gatekeeper() :
		paused(false), width(0), height(0), time_i(10), director(nullptr),
		info_node(nullptr)
	{
		this->fps_label.set_color(Colorb(0xff, 0x00, 0x00));
		this->fps_label.set_text("FPS: 60.0");
		this->tex_label.set_color(Colorb(0xff, 0x00, 0x00));

		this->info_node = new SceneGraph::Node();
		this->info_node->add_child(&this->fps_label);
		this->info_node->add_child(&this->tex_label);
		this->scenegraph.add_child(this->info_node);
	}

	Gatekeeper::~Gatekeeper()
	{
		delete this->director;
	}

	void Gatekeeper::init(const Data &script)
	{
		delete this->director;
		this->director = new Rainbow::Director();
		this->director->set_video(this->width, this->height);
		this->director->init(script);
	}

	void Gatekeeper::set_video(const int width, const int height)
	{
		this->width = width;
		this->height = height;

		const unsigned int pt = this->height / 64;

		this->font = new FontAtlas(pt);
		{
			Data type;
			type.copy(Inconsolata_otf, sizeof(Inconsolata_otf));
			this->font->load(type);
		}
		this->fps_label.set_font(this->font.raw_ptr());
		this->tex_label.set_font(this->font.raw_ptr());

		const int x = this->width / 128;
		const int y = pt + (pt >> 1);
		this->fps_label.set_position(x, this->height - y);
		this->tex_label.set_position(x, this->height - y - y);
	}

	void Gatekeeper::update(const unsigned long t)
	{
		if (!this->paused)
			this->director->update(t);

		// Update frames/second
		this->time[--this->time_i] = Chrono::Instance().diff();
		if (!this->time_i)
		{
			float fps = 0.0f;
			for (; this->time_i < 10; ++this->time_i)
				fps += this->time[this->time_i];
			sprintf(this->fps_text, "FPS: %.1f", 10000.0f / fps);
			this->fps_label.set_text(this->fps_text);
			this->time_i = 10;
		}

		// Update texture memory usage
		if (this->info_node->enabled)
		{
			double used, peak;
			TextureManager::Instance().get_usage(used, peak, peak);
			sprintf(this->tex_text, "VMEM: %.2f MBs (peaked at %.2f MBs)", used, peak);
			this->tex_label.set_text(this->tex_text);
		}

		this->scenegraph.update();
	}
}

#endif
