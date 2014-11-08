// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Heimdall/Gatekeeper.h"

#include <cstring>

#include "Common/Data.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"
#include "Input/Touch.h"
#include "Lua/LuaHelper.h"
#include "Resources/Inconsolata.otf.h"
#include "Resources/NewsCycle-Regular.ttf.h"

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

namespace heimdall
{
	Gatekeeper::Gatekeeper()
	    : overlay_activator_(&overlay_), monitor_(Path::current()) {}

	void Gatekeeper::init(const Data &main, const Vec2i &screen)
	{
		info_.reset(new DebugInfo());
		scenegraph_.add_child(info_->node());

		overlay_.init(scenegraph_, screen);

		const unsigned int pt = screen.y / 64;
		auto console_font = make_shared<FontAtlas>(
		    Data::from_bytes(Inconsolata_otf), pt);
		auto ui_font = make_shared<FontAtlas>(
		    Data::from_bytes(NewsCycle_Regular_ttf), (pt << 1) + (pt >> 1));
		const float y = screen.y - console_font->height();
		Vec2f position(screen.x / 128,
		               y - console_font->height() - ui_font->height());
		info_->init_button(position, std::move(ui_font));
		position.y = y;
		info_->init_console(position, std::move(console_font));
		overlay_.add_child(info_->button().drawable());

		Director::init(main, screen);
		if (terminated())
			return;

		input().subscribe(this);
		input().subscribe(&overlay_activator_);

		monitor_.set_callback([this](const char *path) {
			std::unique_ptr<char[]> file(new char[strlen(path) + 1]);
			strcpy(file.get(), path);
			std::lock_guard<std::mutex> lock(changed_files_mutex_);
			changed_files_ = changed_files_.push_front(std::move(file));
		});
	}

	void Gatekeeper::update(const unsigned long dt)
	{
		if (!changed_files_.empty())
		{
			decltype(changed_files_) files;
			{
				std::lock_guard<std::mutex> lock(changed_files_mutex_);
				files = std::move(changed_files_);
			}
			lua_State *L = state();
			for_each(files, [L](const std::unique_ptr<char[]> &file) {
				Library library(file.get());
				if (!library)
					return;

				LOGI("Reloading '%s'...", library.name());
				rainbow::lua::reload(L, library.open(), library.name());
			});
		}

		Director::update(dt);

		if (!overlay_.is_visible())
			overlay_activator_.update(dt);
		info_->update(dt);
		scenegraph_.update(dt);
	}

	bool Gatekeeper::on_touch_began_impl(const Touch *const touches,
	                                     const size_t count)
	{
		std::for_each(touches, touches + count, [this](const Touch &t) {
			if (info_->button().hit_test(Vec2i(t.x, t.y)))
				pressed_[t.hash] = &info_->button();
		});
		return overlay_.is_visible();
	}

	bool Gatekeeper::on_touch_canceled_impl()
	{
		pressed_.clear();
		return overlay_.is_visible();
	}

	bool Gatekeeper::on_touch_ended_impl(const Touch *const touches,
	                                     const size_t count)
	{
		if (overlay_.is_visible() && !overlay_activator_.is_activated())
		{
			if (!pressed_.empty())
			{
				std::for_each(touches, touches + count, [this](const Touch &t) {
					auto button = pressed_.find(t.hash);
					if (button != pressed_.end())
					{
						if (button->second->hit_test(Vec2i(t.x, t.y)))
							button->second->press();
						pressed_.erase(button);
					}
				});
			}
			else
				overlay_.hide();
			return true;
		}
		else if (!pressed_.empty())
			pressed_.clear();
		return false;
	}

	bool Gatekeeper::on_touch_moved_impl(const Touch *const, const size_t)
	{
		return overlay_.is_visible();
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
