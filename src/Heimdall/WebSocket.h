// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_WEBSOCKET_H_
#define HEIMDALL_WEBSOCKET_H_

#include "Platform/Macros.h"
#if defined(USE_HEIMDALL) && defined(RAINBOW_SDL)

#include <future>
#include <mutex>
#include <string>

struct libwebsocket;
struct libwebsocket_context;

namespace heimdall
{
	class Gatekeeper;

	class WebSocket
	{
	public:
		WebSocket();
		~WebSocket();

		void update(Gatekeeper &);

		void on_output_connection(libwebsocket *wsi, void *user);
		bool on_output_write(libwebsocket *wsi, void *user);
		void on_scene_graph_connection(libwebsocket *wsi, void *user);
		bool on_scene_graph_write(libwebsocket *wsi, void *user);

	private:
		std::mutex scene_graph_mutex_;
		std::mutex output_mutex_;
		std::string scene_graph_;
		std::string output_back_;
		unsigned int state_;
		int output_write_state_;
		std::string output_front_;
		int output_connections_;
		int scene_graph_connections_;
		libwebsocket_context *context_;
		std::future<void> thread_;

		void queue_line(const char *line);
	};
}

#endif  // USE_HEIMDALL && RAINBOW_SDL
#endif  // HEIMDALL_WEBSOCKET_H_
