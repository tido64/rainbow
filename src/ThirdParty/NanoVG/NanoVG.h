// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_NANOVG_NANOVG_H_
#define THIRDPARTY_NANOVG_NANOVG_H_

#include <functional>
#include <utility>
#include <vector>

#ifdef __GNUC__
#	pragma GCC diagnostic push
#	ifdef __clang__
#		pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#		pragma clang diagnostic ignored "-Wnested-anon-types"
#	else
#		pragma GCC diagnostic ignored "-pedantic"
#	endif
#endif
#include <nanovg.h>
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif

#include "Graphics/Drawable.h"

namespace nvg
{
	class Context
	{
	public:
		Context();
		~Context();

		void draw();

		template<typename F>
		void enqueue(F&& cmd)
		{
			commands_.emplace_back(std::forward<F>(cmd));
		}

	private:
		std::vector<std::function<void(NVGcontext*)>> commands_;
		NVGcontext *context_;
	};

	class Drawable final : public ::Drawable
	{
	public:
		Drawable(Context &nvg) : nvg_(&nvg) {}

	private:
		Context *nvg_;

		void draw_impl() override { nvg_->draw(); }
		void update_impl(const unsigned long) override {}
	};
}

#endif
