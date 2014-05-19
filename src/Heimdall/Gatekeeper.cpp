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
				basename = ++c;
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
	    : touch_count_(0), touch_held_(0), overlay_node_(nullptr),
	      monitor_(Path::current())
	{
		if (terminated())
			return;

		info_.reset(new DebugInfo());
		scenegraph_.add_child(info_->node());

		overlay_node_ = scenegraph_.add_child(&overlay_);
		overlay_node_->enabled = false;
		overlay_node_->add_child(info_->button());

		monitor_.set_callback([this](const char *path) {
			char *file = new char[strlen(path) + 1];
			strcpy(file, path);
			std::lock_guard<std::mutex> lock(changed_files_mutex_);
			changed_files_ = changed_files_.push_front(file);
		});

		touch_canceled();
	}

	void Gatekeeper::init(const Data &main, const Vec2i &screen)
	{
		overlay_.setup(screen);

		const unsigned int pt = screen.height / 64;
		FontAtlas *console_font = new FontAtlas(DataRef(Inconsolata_otf), pt);
		FontAtlas *ui_font = new FontAtlas(DataRef(NewsCycle_Regular_ttf),
		                                   (pt << 1) + (pt >> 1));
		const float y = screen.height - console_font->height();
		Vec2f position(screen.width / 128,
		               y - console_font->height() - ui_font->height());
		info_->set_button(position, ui_font);
		position.y = y;
		info_->set_console(position, console_font);

		Director::init(main, screen);
		Input::Instance->subscribe(this, Input::Events::Touch);
	}

	void Gatekeeper::update(const unsigned long dt)
	{
		// Reload changed files.
		{
			decltype(changed_files_) files;
			{
				std::lock_guard<std::mutex> lock(changed_files_mutex_);
				files = std::move(changed_files_);
			}
			lua_State *L = state();
			for_each(files, [L](const char *file) {
				std::unique_ptr<const char[]> path(file);
				Library library(path.get());
				if (!library)
					return;

				R_DEBUG("[Rainbow] Reloading '%s'...\n", library.name());
				Rainbow::Lua::reload(L, library.open(), library.name());
			});
		}

		Director::update(dt);

		if (!overlay_node_->enabled && touch_count_ == 2)
		{
			touch_held_ += dt;
			if (touch_held_ > 2000)
				toggle_overlay();
		}
		info_->update(dt);
		scenegraph_.update(dt);
	}

	void Gatekeeper::touch_began_impl(const Touch *const touches,
	                                  const size_t count)
	{
		if (overlay_node_->enabled)
			return;

		size_t i = 0;
		switch (touch_count_)
		{
			case 0:
				touches_[0] = touches[0];
				++touch_count_;
				++i;
			case 1:
				if (i < count)
				{
					touches_[1] = touches[i];
					++touch_count_;
					touch_held_ = 0;
				}
				break;
			default:
				break;
		}
	}

	void Gatekeeper::touch_canceled_impl()
	{
		touch_count_ = 0;
		touches_[0].hash = -1;
		touches_[1].hash = -1;
	}

	void Gatekeeper::touch_ended_impl(const Touch *const touches,
	                                  const size_t count)
	{
		if (overlay_node_->enabled && !touch_count_)
		{
			if (!info_->on_touch(touches, count))
				toggle_overlay();
			return;
		}

		for (size_t i = 0; i < count; ++i)
		{
			if (touches[i] == touches_[0])
			{
				touches_[0] = touches_[1];
				touches_[1].hash = -1;
				--touch_count_;
			}
			else if (touches[i] == touches_[1])
			{
				touches_[1].hash = -1;
				--touch_count_;
			}
		}
	}
}

Library::Library(const char *const path) : path_(path)
{
	const char *filename = basename(path_);
	size_t length = strlen(filename);
	if (length < 5 || memcmp(filename + length - 4, ".lua", 4) != 0)
	{
		path_ = nullptr;
		return;
	}
	length -= 4;
	name_.reset(new char[length + 1]);
	strncpy(name_.get(), filename, length);
	name_[length] = '\0';
}

const char* Library::name() const
{
	return name_.get();
}

Data Library::open() const
{
#if defined(RAINBOW_OS_MACOS)
	return Data(File::open(path_));
#elif defined(RAINBOW_OS_WINDOWS)
	return Data::load_asset(path_);
#else
	return Data();
#endif
}

Library::operator bool() const
{
	return path_;
}

#endif
