#pragma once

#include <map>
#include <thread>
#include <mutex>
#include <list>
#include <vector>
#include <stdint.h>

#include "file_protocol.h"
#include "rudp_socket.h"

struct send_message_buffer
{
	unsigned short message_id_;
	char *data_;
	int size_;
	int linker_handle_;
	send_message_buffer *next_;					
};

class udp_manager
{
public:
	udp_manager(const std::string &ip, const int &port, const bool &delay, const int &delay_interval);
	~udp_manager(void);

public:
	int linker_handle_ = ERROR_HANDLE;
	std::string udp_ip_ = "127.0.0.1";
	int udp_port_ = 9999;
	bool delay_ = false;
	int delay_interval_ = 2000;

	void init();
	bool uninit();
	int connect_server();
	void load_json();

public:
	int connect();

public:
	rudp_manager *rudp_manager_ptr_ = nullptr;
	void rudp_on_connect(const struct sockaddr_in &addr, const int &linker_handle);
	bool rudp_on_recv(char* data, int size, int linker_handle);
	void rudp_on_disconnected(const int &linker_handle);
	void rudp_on_error(const int &error, const int &linker_handle);
};
