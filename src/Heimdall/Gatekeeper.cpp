#ifdef USE_HEIMDALL

#include "Common/Data.h"
#include "Heimdall/Gatekeeper.h"
#include "Heimdall/Resources.h"
#include "Resources/Inconsolata.otf.h"
#include "Resources/NewsCycle-Regular.ttf.h"

namespace Heimdall
{
	namespace Resources
	{
		const Colorb kColorDisabled(0xff, 0xff, 0xff, 0xb0);
		const Colorb kColorEnabled;
		FontAtlas *ConsoleFont = nullptr;
		FontAtlas *UIFont = nullptr;
	}

	Gatekeeper::Gatekeeper() :
		width(0), height(0), touch_count(0), director(nullptr),
		overlay_node(nullptr)
	{
		this->scenegraph.add_child(this->info.get_node());

		this->overlay_node = this->scenegraph.add_child(&this->overlay);
		this->overlay_node->enabled = false;
		this->overlay_node->add_child(this->info.get_button());

		this->touch_canceled();
	}

	Gatekeeper::~Gatekeeper()
	{
		Resources::ConsoleFont = nullptr;
		Resources::UIFont = nullptr;
		delete this->director;
	}

	void Gatekeeper::init(const Data &script, const int width, const int height)
	{
		this->set_video(width, height);
		delete this->director;
		this->director = new Rainbow::Director();
		this->director->init(script, this->width, this->height);
		Input::Instance->subscribe(this, Input::TOUCH_EVENTS);
	}

	void Gatekeeper::set_video(const int width, const int height)
	{
		this->width = width;
		this->height = height;

		const unsigned int pt = this->height / 64;

		this->console_font = new FontAtlas(
				Data(Inconsolata_otf, sizeof(Inconsolata_otf), Data::kDataReference),
				pt);
		this->ui_font = new FontAtlas(
				Data(NewsCycle_Regular_ttf, sizeof(NewsCycle_Regular_ttf), Data::kDataReference),
				(pt << 1) + (pt >> 1));
		Resources::ConsoleFont = this->console_font.get();
		Resources::UIFont = this->ui_font.get();

		this->overlay.setup(width, height);
		Vec2f position(
			this->width / 128,
			this->height - this->console_font->get_height() * 2 - this->ui_font->get_height());
		this->info.set_button(position);
		position.y = this->height - this->console_font->get_height();
		this->info.set_console(position);
	}

	void Gatekeeper::update(const unsigned long dt)
	{
		//if (!this->overlay_node->enabled)
			this->director->update(dt);
		if (!this->overlay_node->enabled && this->touch_count == 2)
		{
			this->touch_held += dt;
			if (this->touch_held > 2000)
				this->toggle_overlay();
		}
		this->info.update(dt);
		this->scenegraph.update(dt);
	}

	void Gatekeeper::touch_began_impl(const Touch *const touches, const size_t count)
	{
		if (this->overlay_node->enabled)
			return;

		size_t i = 0;
		switch (this->touch_count)
		{
			case 0:
				this->touches[0] = touches[0];
				++this->touch_count;
				++i;
			case 1:
				if (i < count)
				{
					this->touches[1] = touches[i];
					++this->touch_count;
					this->touch_held = 0;
				}
				break;
			default:
				break;
		}
	}

	void Gatekeeper::touch_canceled_impl()
	{
		this->touch_count = 0;
		this->touches[0].hash = -1;
		this->touches[1].hash = -1;
	}

	void Gatekeeper::touch_ended_impl(const Touch *const touches, const size_t count)
	{
		if (this->overlay_node->enabled && !this->touch_count)
		{
			if (!this->info.on_touch(touches, count))
				this->toggle_overlay();
			return;
		}

		for (size_t i = 0; i < count; ++i)
		{
			if (touches[i] == this->touches[0])
			{
				this->touches[0] = this->touches[1];
				this->touches[1].hash = -1;
				--this->touch_count;
			}
			else if (touches[i] == this->touches[1])
			{
				this->touches[1].hash = -1;
				--this->touch_count;
			}
		}
	}

	void Gatekeeper::touch_moved_impl(const Touch *const, const size_t) { }
}

#endif
