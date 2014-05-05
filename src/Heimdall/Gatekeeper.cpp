// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Heimdall/Gatekeeper.h"

#include "Common/Data.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"
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
		explicit Library(const char *const path);

		inline const char* name() const;

		inline Data open() const;

		inline explicit operator bool() const;

	private:
		std::unique_ptr<char[]> name_;
		const char *path_;
	};
}

namespace Heimdall
{
	Gatekeeper::Gatekeeper()
	    : touch_count(0), touch_held(0), overlay_node(nullptr),
	      monitor(Path::current())
	{
		if (this->terminated())
			return;

		this->info.reset(new DebugInfo());
		this->scenegraph.add_child(this->info->node());

		this->overlay_node = this->scenegraph.add_child(&this->overlay);
		this->overlay_node->enabled = false;
		this->overlay_node->add_child(this->info->button());

		this->monitor.set_callback([this](const char *path) {
			char *file = new char[strlen(path) + 1];
			strcpy(file, path);
			std::lock_guard<std::mutex> lock(this->changed_files_mutex);
			this->changed_files = this->changed_files.push_front(file);
		});

		this->touch_canceled();
	}

	void Gatekeeper::init(const Data &main, const Vec2i &screen)
	{
		this->overlay.setup(screen);

		const unsigned int pt = screen.height / 64;
		FontAtlas *console_font = new FontAtlas(DataRef(Inconsolata_otf), pt);
		FontAtlas *ui_font = new FontAtlas(DataRef(NewsCycle_Regular_ttf),
		                                   (pt << 1) + (pt >> 1));
		const float y = screen.height - console_font->height();
		Vec2f position(screen.width / 128,
		               y - console_font->height() - ui_font->height());
		this->info->set_button(position, ui_font);
		position.y = y;
		this->info->set_console(position, console_font);

		Director::init(main, screen);
		Input::Instance->subscribe(this, Input::Events::Touch);

		lua_State *L = this->state();
		this->reload = [L](const char *file) {
			std::unique_ptr<const char[]> path(file);
			Library library(path.get());
			if (!library)
				return;

			R_DEBUG("[Rainbow] Reloading '%s'...\n", library.name());
			Rainbow::Lua::reload(L, library.open(), library.name());
		};
	}

	void Gatekeeper::update(const unsigned long dt)
	{
		// Reload changed files.
		{
			decltype(this->changed_files) files;
			{
				std::lock_guard<std::mutex> lock(this->changed_files_mutex);
				files = std::move(this->changed_files);
			}
			for_each(files, this->reload);
		}

		Director::update(dt);

		if (!this->overlay_node->enabled && this->touch_count == 2)
		{
			this->touch_held += dt;
			if (this->touch_held > 2000)
				this->toggle_overlay();
		}
		this->info->update(dt);
		this->scenegraph.update(dt);
	}

	void Gatekeeper::touch_began_impl(const Touch *const touches,
	                                  const size_t count)
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

	void Gatekeeper::touch_ended_impl(const Touch *const touches,
	                                  const size_t count)
	{
		if (this->overlay_node->enabled && !this->touch_count)
		{
			if (!this->info->on_touch(touches, count))
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
}

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

Data Library::open() const
{
#if defined(RAINBOW_OS_MACOS)
	return Data(File::open(this->path_));
#elif defined(RAINBOW_OS_WINDOWS)
	return Data::load_asset(this->path_);
#else
	return Data();
#endif
}

Library::operator bool() const
{
	return this->path_;
}

#endif
