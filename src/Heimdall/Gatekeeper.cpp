#ifdef USE_HEIMDALL

#include "Common/Chrono.h"
#include "Common/Data.h"
#include "Graphics/TextureManager.h"
#include "Heimdall/Gatekeeper.h"
#include "Resources/Inconsolata.otf.h"

namespace Heimdall
{
	const float kFrames = 10.0f;
	const float kThreshold = 1000.0f / 60.0f * kFrames;
	//const char kControlFormat[] = "\u009b \u00bb";
	const char kInfoFormat[] = "FPS: %.1f\nVMEM: %.2f MBs (peaked at %.2f MBs)";

	Gatekeeper::Gatekeeper() :
		time(0.0f), width(0), height(0), director(nullptr), info_node(nullptr)
	{
		sprintf(this->info_text, kInfoFormat, 60.0f, 0.0f, 0.0f);
		this->info_label.set_color(Colorb(0xff, 0x00, 0x00));
		this->info_label.set_text(this->info_text);
		this->info_node = new SceneGraph::Node();
		this->info_node->add_child(&this->info_label);

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
		this->info_label.set_font(this->font.raw_ptr());
		this->info_label.set_position(this->width / 128, this->height - this->font->get_height());
	}

	void Gatekeeper::update(const unsigned long t)
	{
		this->director->update(t);

		this->time += Chrono::Instance().diff();
		if (this->time >= kThreshold)
		{
			double used, peak;
			TextureManager::Instance().get_usage(used, peak, peak);

			sprintf(this->info_text, kInfoFormat, 1000.0f / (this->time / kFrames), used, peak);
			this->info_label.set_text(this->info_text);

			this->time = 0.0f;
		}

		this->scenegraph.update();
	}
}

#endif
