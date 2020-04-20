#pragma once

#include <map>
#include <thread>
#include <mutex>
#include <list>
#include <vector>
#include <stdint.h>
#include <string>
#include <functional>
#include "../../header/file_public.h"
#include "../../header/file_protocol.h"
#include "rudp_socket.h"
#include "path.h"

#pragma pack(push, 1)

class udp_manager
{
public:
	udp_manager(const int &port, const bool &skip_frames, const int &skip_interval);
	~udp_manager(void);
public:
	int udp_port_ = 9999;
	bool delay_ = false;
	int delay_interval_ = 2000;
	void load_json();
public:
	bool init();
	bool uninit();

public:
	rudp_manager *rudp_manager_ptr_ = nullptr;

public:
	bool rudp_on_read(char* data, int size, int linker_handle, struct sockaddr_in addr);
	bool rudp_on_check_ip(struct sockaddr_in addr);
	void rudp_on_disconnected(const int &linker_handle, struct sockaddr_in addr);
	void rudp_on_error(const int &error, const int &linker_handle, struct sockaddr_in addr);
};
