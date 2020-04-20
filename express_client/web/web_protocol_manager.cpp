/*
 * web_protocol.cpp
 *
 *  Created on: 2017年5月17日
 *      Author: fxh7622
 */


#include <mutex>
#include <time.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

//包含event2头;
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

#include "string_list.h"
#include "web_protocol_manager.h"

web_protocol::web_protocol()
{
}

void web_protocol::init(const int &listen_port)
{
	listen_port_ = listen_port;
	thread_ptr_ = std::thread(&web_protocol::execute, this);
	thread_ptr_.detach();
}

web_protocol::~web_protocol(void)
{
	free_message_id();
}

void web_protocol::generic_handler(struct evhttp_request *request, void *arg)
{
	//有链接请求过来;
	web_protocol* web_protocol_mananger_ptr = static_cast<web_protocol*>(arg);

	evhttp_add_header(evhttp_request_get_output_headers(request), "Access-Control-Allow-Origin", "*");
	evhttp_add_header(evhttp_request_get_output_headers(request), "Access-Control-Allow-Methods", "GET,POST,OPTIONS");
	evhttp_add_header(evhttp_request_get_output_headers(request), "Access-Control-Allow-Headers", "x-requested-with,content-type");

	//得到command的类型;
	evhttp_cmd_type cmd_type = evhttp_request_get_command(request);
	switch(cmd_type)
	{
		case EVHTTP_REQ_POST:	//POST类型
		{
			struct evbuffer *buf = evhttp_request_get_input_buffer(request);
			while(1)
			{
				int length = evbuffer_get_length(buf);
				char *buffer = new char[length];
				memset(buffer, 0, length);
				int ret = evbuffer_remove(buf, buffer, length);
				if (ret != length)
				{
					web_protocol_mananger_ptr->check_message(WEB_ERROR, "evbuffer_remove length != ret error=%d", errno);
				}

				if(web_protocol_mananger_ptr->on_add_queue_ != nullptr)
				{
					//投递web请求给主线程;
					std::string params_string(buffer, length);
					web_protocol_mananger_ptr->on_add_queue_("POST", params_string, request);
					evhttp_send_reply(request, HTTP_OK, "OK", buf);
				}
				else
				{
					//没有办法投递到主线程，直接返回错误;
					evhttp_send_reply(request, HTTP_NOTFOUND, "OK", buf);
				}
				//evbuffer_free(buf); //测试
				delete[] buffer;
				return;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void web_protocol::execute()
{
	check_message(WEB_INFO, "event_base_new port=%d", listen_port_);
	base = event_base_new();
	if (!base)
	{
		check_message(WEB_ERROR, "event_base_new error=%d port=%d", errno, listen_port_);
		return;
	}

	//创建一个HTTP的对象;
	struct evhttp * http_server = evhttp_new(base);
	if(!http_server)
	{
		check_message(WEB_ERROR, "evhttp_new error=%d port=%d", errno, listen_port_);
		return;
	}
	//绑定地址;
	short http_port = listen_port_;
	int ret = evhttp_bind_socket(http_server, "0.0.0.0", http_port);
	if(ret != 0)
	{
		check_message(WEB_ERROR, "evhttp_bind_socket error=%d port=%d", errno, listen_port_);
		return;
	}
	//注册事件;
	evhttp_set_gencb(http_server, generic_handler, this);

	// 开工;
	check_message(WEB_DEBUG, "event_base_dispatch port=%d", listen_port_);
	event_base_dispatch(base);

	check_message(WEB_DEBUG, "event_base_dispatch free error=%d port=%d", errno, listen_port_);
	event_base_free(base);
}

void web_protocol::check_message(int message_type, const char *context, ...)
{
	//得到信息;
	char message_text[8 * 1024];
	memset(message_text, 0x00, 8 * 1024);
	va_list arg_ptr;
	va_start(arg_ptr, context);
	vsprintf(message_text, context, arg_ptr);
	va_end(arg_ptr);

	if(on_check_message_ != nullptr)
		on_check_message_(message_type, message_text);
}

int web_protocol::get_message_id(const std::string &path)
{
	std::lock_guard<std::recursive_mutex> message_gurad(message_lock_);
	std::map<std::string, int>::iterator iter = path_message_id_map_.find(path);
	if(iter != path_message_id_map_.end())
	{
		return iter->second;
	}
	return -1;
}

bool web_protocol::add_message_id(const std::string &path, const int &message_id)
{
	std::lock_guard<std::recursive_mutex> message_gurad(message_lock_);
	path_message_id_map_.insert(std::make_pair(path, message_id));
	return true;
}

void web_protocol::free_message_id()
{
	std::lock_guard<std::recursive_mutex> message_gurad(message_lock_);
	path_message_id_map_.clear();
}
