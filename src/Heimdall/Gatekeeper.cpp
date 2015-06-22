// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Heimdall/Gatekeeper.h"

#include "Common/Data.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"
#include "Input/Pointer.h"
#include "Resources/Inconsolata.otf.h"
#include "Resources/NewsCycle-Regular.ttf.h"

#if !defined(USE_LUA_SCRIPT) || USE_LUA_SCRIPT
#include "Lua/LuaHelper.h"
#include "Lua/LuaScript.h"

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

		const char* name() const { return name_.get(); }

		inline Data open() const;

		explicit operator bool() const { return path_; }

	private:
		std::unique_ptr<char[]> name_;
		const char *path_;
	};
}
#endif  // USE_LUA_SCRIPT

namespace heimdall
{
	Gatekeeper::Gatekeeper()
	    : overlay_activator_(&overlay_)
#if !defined(USE_LUA_SCRIPT) || USE_LUA_SCRIPT
	    , monitor_(Path::current())
#endif  // USE_LUA_SCRIPT
	{}

	void Gatekeeper::init(const Vec2i &screen)
	{
		pre_init(screen);
		Director::init(screen);
		if (terminated())
			return;
		post_init();
	}

	void Gatekeeper::update(const unsigned long dt)
	{
#if !defined(USE_LUA_SCRIPT) || USE_LUA_SCRIPT
		lua_State *L = static_cast<LuaScript*>(script())->state();
		while (!changed_files_.empty())
		{
			const Library library(changed_files_.front().get());
			{
				std::lock_guard<std::mutex> lock(changed_files_mutex_);
				changed_files_.pop();
			}

			if (!library)
				continue;

			LOGI("Reloading '%s'...", library.name());
			rainbow::lua::reload(L, library.open(), library.name());
		}
#endif  // USE_LUA_SCRIPT

		Director::update(dt);

		if (!overlay_.is_visible())
			overlay_activator_.update(dt);
		info_->update(dt);
		scenegraph_.update(dt);
	}

	void Gatekeeper::post_init()
	{
#if !defined(USE_LUA_SCRIPT) || USE_LUA_SCRIPT
		monitor_.set_callback([this](const char *path) {
			auto file = new char[strlen(path) + 1];
			strcpy(file, path);
			std::lock_guard<std::mutex> lock(changed_files_mutex_);
			changed_files_.emplace(file);
		});
#endif  // USE_LUA_SCRIPT
	}

	void Gatekeeper::pre_init(const Vec2i &screen)
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

		input().subscribe(this);
		input().subscribe(&overlay_activator_);
	}

	bool Gatekeeper::on_pointer_began_impl(const unsigned int count,
	                                       const Pointer *pointers)
	{
		std::for_each(pointers, pointers + count, [this](const Pointer &p) {
			if (info_->button().hit_test(Vec2i(p.x, p.y)))
				pressed_[p.hash] = &info_->button();
		});
		return overlay_.is_visible();
	}

	bool Gatekeeper::on_pointer_canceled_impl()
	{
		pressed_.clear();
		return overlay_.is_visible();
	}

	bool Gatekeeper::on_pointer_ended_impl(const unsigned int count,
	                                       const Pointer *pointers)
	{
		if (overlay_.is_visible() && !overlay_activator_.is_activated())
		{
			if (!pressed_.empty())
			{
				std::for_each(pointers,
				              pointers + count,
				              [this](const Pointer &p) {
					auto button = pressed_.find(p.hash);
					if (button != pressed_.end())
					{
						if (button->second->hit_test(Vec2i(p.x, p.y)))
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

	bool Gatekeeper::on_pointer_moved_impl(const unsigned int, const Pointer *)
	{
		return overlay_.is_visible();
	}
}

#if !defined(USE_LUA_SCRIPT) || USE_LUA_SCRIPT
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

Data Library::open() const
{
#if defined(RAINBOW_OS_MACOS)
	return Data(File::open(path_));
#elif defined(RAINBOW_OS_WINDOWS)
	return Data::load_asset(path_);
#else
	return Data();
#endif  // RAINBOW_OS_MACOS
}
#endif  // USE_LUA_SCRIPT

#endif  // USE_HEIMDALL
