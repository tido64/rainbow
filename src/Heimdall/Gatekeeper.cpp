#ifdef USE_HEIMDALL

#include "Common/Data.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"
#include "Heimdall/Gatekeeper.h"
#include "Heimdall/Resources.h"
#include "Lua/LuaHelper.h"
#include "Resources/Inconsolata.otf.h"
#include "Resources/NewsCycle-Regular.ttf.h"

#define DataRef(ref) Data(ref, sizeof(ref), Data::kDataReference)

namespace
{
	const char* basename(const char *const path) pure;

	const char* basename(const char *const path)
	{
		const char *basename = path;
		for (const char *c = path; *c; ++c)
		{
			if (*c == '/' || *c == ':' || *c == '\\')
			{
				basename = ++c;
				continue;
			}
		}
		return basename;
	}

	class Library
	{
	public:
		Library(const char *const path);

		inline const char* name() const;

		inline operator bool() const;
		inline operator Data() const;

	private:
		std::unique_ptr<char[]> name_;
		const char *path_;
	};

	Library::Library(const char *const path) : path_(path)
	{
		const char *filename = basename(this->path_);
		size_t length = strlen(filename);
		if (length < 5 || memcmp(filename + length - 4, ".lua", 4) != 0)
		{
			this->path_ = nullptr;
			return;
		}
		length -= 4;
		this->name_.reset(new char[length + 1]);
		strncpy(this->name_.get(), filename, length);
		this->name_[length] = '\0';
	}

	const char* Library::name() const
	{
		return this->name_.get();
	}

	Library::operator bool() const
	{
		return this->path_;
	}

	Library::operator Data() const
	{
	#if defined(RAINBOW_OS_MACOS)
		return Data(File::open(this->path_));
	#elif defined(RAINBOW_OS_WINDOWS)
		return Data::load_asset(this->path_);
	#else
		return Data();
	#endif
	}
}

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
		width(0), height(0), touch_count(0), overlay_node(nullptr),
		monitor(Path::current())
	{
		this->scenegraph.add_child(this->info.node());

		this->overlay_node = this->scenegraph.add_child(&this->overlay);
		this->overlay_node->enabled = false;
		this->overlay_node->add_child(this->info.button());

		this->monitor.set_callback([this](const char *path) {
			char *file = new char[strlen(path) + 1];
			strcpy(file, path);
			this->queue.push(file);
		});

		this->touch_canceled();
	}

	Gatekeeper::~Gatekeeper()
	{
		Resources::ConsoleFont = nullptr;
		Resources::UIFont = nullptr;
	}

	void Gatekeeper::init(const Data &script, const int width, const int height)
	{
		this->set_video(width, height);
		this->director.reset(new Rainbow::Director());
		this->director->init(script, this->width, this->height);
		Input::Instance->subscribe(this, Input::TOUCH_EVENTS);
	}

	void Gatekeeper::set_video(const int width, const int height)
	{
		this->width = width;
		this->height = height;

		const unsigned int pt = this->height / 64;
		this->console_font = new FontAtlas(DataRef(Inconsolata_otf), pt);
		this->ui_font = new FontAtlas(DataRef(NewsCycle_Regular_ttf), (pt << 1) + (pt >> 1));
		Resources::ConsoleFont = this->console_font.get();
		Resources::UIFont = this->ui_font.get();

		this->overlay.setup(width, height);
		const float y = this->height - this->console_font->height();
		Vec2f position(this->width / 128,
		               y - this->console_font->height() - this->ui_font->height());
		this->info.set_button(position);
		position.y = y;
		this->info.set_console(position);
	}

	void Gatekeeper::update(const unsigned long dt)
	{
		const char *file = nullptr;
		while (this->queue.pop(file))
		{
			std::unique_ptr<const char[]> path(file);
			Library library(path.get());
			if (!library)
				continue;

			R_DEBUG("[Rainbow] Reloading '%s'...\n", library.name());
			Rainbow::Lua::reload(this->director->state(), library, library.name());
		}
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
