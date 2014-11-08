// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/WebSocket.h"
#if defined(USE_HEIMDALL) && defined(RAINBOW_SDL)

#include <algorithm>
#include <cstring>
#include <functional>

#include <libwebsockets.h>

#include "Heimdall/Gatekeeper.h"
#include "Resources/Rainbow.html.h"

namespace
{
	const int kListeningPort = 9000;
	const int kPollTimeOut = 50;

	const unsigned int kOutputHasUpdates      = 1u << 0;
	const unsigned int kSceneGraphHasUpdates  = 1u << 1;

	const size_t kSendBufferPadding =
	    LWS_SEND_BUFFER_PRE_PADDING + LWS_SEND_BUFFER_POST_PADDING;

	template<size_t N>
	unsigned int accumulate(const char *const (&strings)[N],
	                        std::unique_ptr<char[]> &result)
	{
		size_t lengths[N];
		size_t size = 0;
		for (unsigned int i = 0; i < N; ++i)
		{
			lengths[i] = strlen(strings[i]);
			size += lengths[i];
		}
		result.reset(new char[size]);
		char *buf = result.get();
		for (unsigned int i = 0; i < N; ++i)
		{
			std::copy_n(strings[i], lengths[i], buf);
			buf += lengths[i];
		}
		R_ASSERT(size < 9999999, "kHeaderSize needs to be accommodated");
		return static_cast<unsigned int>(size);
	}

	size_t get_written(void *user)
	{
		return *static_cast<size_t*>(user);
	}

	void set_written(void *user, const size_t written)
	{
		*static_cast<size_t*>(user) = written;
	}

	heimdall::WebSocket* web_socket(libwebsocket_context *context)
	{
		return static_cast<heimdall::WebSocket*>(
		    libwebsocket_context_user(context));
	}

	size_t write_socket(libwebsocket *wsi, unsigned char *buf, const size_t len)
	{
		size_t written = 0;
		while (written < len && !lws_send_pipe_choked(wsi))
		{
			const int status = libwebsocket_write(
			    wsi, buf + written, len - written, LWS_WRITE_TEXT);
			if (status <= 0)
				break;
			written += status;
		}
		return written;
	}

	bool write_socket(libwebsocket *wsi, void *user, std::string &str)
	{
		size_t written = get_written(user);
		written += write_socket(
		    wsi,
		    reinterpret_cast<unsigned char*>(const_cast<char*>(
		        str.data() + LWS_SEND_BUFFER_PRE_PADDING + written)),
		    str.length() - kSendBufferPadding - written);
		const bool completed = written == str.length() - kSendBufferPadding;
		set_written(user, !completed ? written : 0);
		return completed;
	}

	int on_connection(libwebsocket *wsi, void *user)
	{
		if (!wsi)
			return -1;
		set_written(user, 0);
		return 1;
	}

	template<typename F, typename G>
	int on_request(libwebsocket_context *context,
	               libwebsocket *wsi,
	               libwebsocket_callback_reasons reason,
	               void *user,
	               F&& on_connection,
	               G&& on_write)
	{
		switch (reason)
		{
			case LWS_CALLBACK_ESTABLISHED:
				(web_socket(context)->*on_connection)(wsi, user);
				libwebsocket_callback_on_writable(context, wsi);
				break;
			case LWS_CALLBACK_CLOSED:
				(web_socket(context)->*on_connection)(nullptr, nullptr);
				break;
			case LWS_CALLBACK_SERVER_WRITEABLE:
				if (!(web_socket(context)->*on_write)(wsi, user))
					libwebsocket_callback_on_writable(context, wsi);
				break;
			default:
				break;
		}
		return 0;
	}

	int on_http_request(libwebsocket_context *context,
	                    libwebsocket *wsi,
	                    libwebsocket_callback_reasons reason,
	                    void *,
	                    void *in,
	                    size_t)
	{
		static std::unique_ptr<char[]> html;
		static const unsigned int kContentLength =
		    accumulate(rainbow_html, html);

		switch (reason)
		{
			case LWS_CALLBACK_HTTP: {
				const char *url = static_cast<const char*>(in);
				if (url[0] == '/' && url[1] == '\0')
				{
					const char kResponseHeader[] =
					    "HTTP/1.0 200 OK\r\n"
					    "Server: Rainbow\r\n"
					    "Content-Type: text/html\r\n"
					    "Content-Length: %u\r\n\r\n";
					const size_t kHeaderSize = sizeof(kResponseHeader) + 5;

					char buffer[LWS_SEND_BUFFER_PRE_PADDING + kHeaderSize +
					            LWS_SEND_BUFFER_POST_PADDING];
					char *content = buffer + LWS_SEND_BUFFER_PRE_PADDING;
					libwebsocket_write(
					    wsi,
					    reinterpret_cast<unsigned char*>(content),
					    snprintf(content,
					             kHeaderSize,
					             kResponseHeader,
					             kContentLength),
					    LWS_WRITE_HTTP);

					libwebsocket_callback_on_writable(context, wsi);
				}
				else
				{
					libwebsockets_return_http_status(
					    context, wsi, HTTP_STATUS_NOT_FOUND, nullptr);
				}
				break;
			}
			case LWS_CALLBACK_HTTP_WRITEABLE:
				libwebsocket_write(
				    wsi,
				    reinterpret_cast<unsigned char*>(html.get()),
				    kContentLength,
				    LWS_WRITE_HTTP);
				return 1;
			default:
				break;
		}
		return 0;
	}

	int on_output_request(libwebsocket_context *context,
	                      libwebsocket *wsi,
	                      libwebsocket_callback_reasons reason,
	                      void *user,
	                      void *,
	                      size_t)
	{
		return on_request(context,
		                  wsi,
		                  reason,
		                  user,
		                  &heimdall::WebSocket::on_output_connection,
		                  &heimdall::WebSocket::on_output_write);
	}

	int on_scene_graph_request(libwebsocket_context *context,
	                           libwebsocket *wsi,
	                           libwebsocket_callback_reasons reason,
	                           void *user,
	                           void *,
	                           size_t)
	{
		return on_request(context,
		                  wsi,
		                  reason,
		                  user,
		                  &heimdall::WebSocket::on_scene_graph_connection,
		                  &heimdall::WebSocket::on_scene_graph_write);
	}

	libwebsocket_protocols g_websocket_protocols[] = {
	    {"http-only", on_http_request, 0, 0, 0, nullptr, 0},
	    {"rainbow-output", on_output_request, sizeof(size_t), 0, 0, nullptr, 0},
	    {"rainbow-scenegraph",
	     on_scene_graph_request,
	     sizeof(size_t),
	     0,
	     0,
	     nullptr,
	     0},
	    {nullptr, nullptr, 0, 0, 0, nullptr, 0},
	};

	const libwebsocket_protocols* output_protocol()
	{
		return g_websocket_protocols + 1;
	}

	const libwebsocket_protocols* scene_graph_protocol()
	{
		return g_websocket_protocols + 2;
	}
}

namespace heimdall
{
	std::function<void(const char*)> write_external_log = [](const char *) {};

	WebSocket::WebSocket()
	    : state_(0), output_write_state_(0), output_connections_(0),
	      scene_graph_connections_(0)
	{
		scene_graph_.assign(
		    LWS_SEND_BUFFER_PRE_PADDING + LWS_SEND_BUFFER_POST_PADDING, ' ');
		output_back_.assign(LWS_SEND_BUFFER_PRE_PADDING, ' ');
		lws_context_creation_info info;
		memset(&info, 0, sizeof(info));
		info.port = kListeningPort;
		info.protocols = g_websocket_protocols;
		info.gid = -1;
		info.uid = -1;
		info.user = this;
		context_ = libwebsocket_create_context(&info);
		thread_ = std::async(std::launch::async, [this] {
			lwsl_notice("Serving http://127.0.0.1:%i/\n", kListeningPort);
			while (context_)
			{
				if (output_write_state_ == 0 &&
				    (state_ & kOutputHasUpdates) != 0)
				{
					state_ &= ~kOutputHasUpdates;
					libwebsocket_callback_on_writable_all_protocol(
					    output_protocol());
				}
				if (scene_graph_connections_ > 0 &&
				    (state_ & kSceneGraphHasUpdates) != 0)
				{
					state_ &= ~kSceneGraphHasUpdates;
					libwebsocket_callback_on_writable_all_protocol(
					    scene_graph_protocol());
				}
				libwebsocket_service(context_, kPollTimeOut);
				if (output_write_state_ == output_connections_)
					output_write_state_ = 0;
			}
		});
		write_external_log = [this](const char *line) { queue_line(line); };
	}

	WebSocket::~WebSocket()
	{
		libwebsocket_context *context = context_;
		context_ = nullptr;
		thread_.get();
		libwebsocket_context_destroy(context);
	}

	void WebSocket::update(Gatekeeper &gk)
	{
		if (gk.scenegraph().has_changes())
		{
			scene_graph_mutex_.lock();
			scene_graph_.assign(LWS_SEND_BUFFER_PRE_PADDING, ' ');
			gk.scenegraph().to_string(scene_graph_)
			               .append(LWS_SEND_BUFFER_POST_PADDING, ' ');
			scene_graph_mutex_.unlock();
			gk.scenegraph().reset_state();
			state_ |= kSceneGraphHasUpdates;
		}
	}

	void WebSocket::on_output_connection(libwebsocket *wsi, void *user)
	{
		output_connections_ += on_connection(wsi, user);
	}

	bool WebSocket::on_output_write(libwebsocket *wsi, void *user)
	{
		if (output_write_state_ == 0)
		{
			output_mutex_.lock();
			std::swap(output_back_, output_front_);
			output_back_.assign(LWS_SEND_BUFFER_PRE_PADDING, ' ');
			output_mutex_.unlock();
			output_front_.append(LWS_SEND_BUFFER_POST_PADDING, ' ');
		}
		const bool success = write_socket(wsi, user, output_front_);
		output_write_state_ += success;
		return success;
	}

	void WebSocket::on_scene_graph_connection(libwebsocket *wsi, void *user)
	{
		scene_graph_connections_ += on_connection(wsi, user);
	}

	bool WebSocket::on_scene_graph_write(libwebsocket *wsi, void *user)
	{
		std::lock_guard<std::mutex> lock(scene_graph_mutex_);
		return write_socket(wsi, user, scene_graph_);
	}

	void WebSocket::queue_line(const char *line)
	{
		std::lock_guard<std::mutex> lock(output_mutex_);
		output_back_ += line;
		state_ |= kOutputHasUpdates;
	}
}

#endif
