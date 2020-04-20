/*
 * web_protocol.h
 *
 *  Created on: 2017年5月17日
 *      Author: fxh7622
 */

#pragma once

#ifndef WEB_PROTOCOL_H_
#define WEB_PROTOCOL_H_

#include <memory>
#include <thread>
#include <mutex>
#include <map>

//加载EVENT;
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

//web消息处理
class web_protocol;
struct web_linker_information
{
	web_protocol *parent_;
	int linker_handle_;
	int user_id_;
	std::string ip_;
	time_t active_timer_;
	struct evhttp_request *req_;
	struct evbuffer *buf_;
};

const int WEB_ERROR 	= 	1;
const int WEB_INFO	=	2;
const int WEB_DEBUG	=	3;

typedef std::function<void (const int &message_type, const std::string &message)> ON_CHECK_MESSAGE;
typedef std::function<void (std::string message_type, std::string params, struct evhttp_request *request)> ON_ADD_QUEUE;

enum thread_state{ts_init, ts_runing, ts_stoping, ts_stoped };

class web_protocol
{
public:
	ON_CHECK_MESSAGE on_check_message_;
	ON_ADD_QUEUE on_add_queue_;
public:
	int listen_port_ = 8081;
	std::thread thread_ptr_;
	web_protocol();
	~web_protocol(void);
	void init(const int &listen_port = 8081);

public:
	std::recursive_mutex message_lock_;
	std::map<std::string, int> path_message_id_map_;
	int get_message_id(const std::string &path);
	bool add_message_id(const std::string &path, const int &message_id);
	void free_message_id();

public:
	struct event_base *base;
	static void generic_handler(struct evhttp_request *req, void *arg);

public:
	void execute();
	void check_message(int message_type, const char *context, ...);
};

#endif /* WEB_PROTOCOL_H_ */
