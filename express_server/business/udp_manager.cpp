
#include <thread>
#include <mutex>

#include "udp_manager.h"
#include "rudp_public.h"
#include "main_thread.h"

#include "ini.h"
#include "path.h"
#include "file_protocol.h"

udp_manager::udp_manager(const int &port, const bool &delay, const int &delay_interval)
{
	udp_port_ = port;
	delay_ = delay;
	delay_interval_ = delay_interval;
}

udp_manager::~udp_manager(void)
{
}

bool udp_manager::init()
{
	//创建并启动RUDP管理;
	rudp_manager_ptr_ = new rudp_manager();
	rudp_manager_ptr_->init();
	rudp_manager_ptr_->on_checkip = std::bind(&udp_manager::rudp_on_check_ip, this, std::placeholders::_1);
	rudp_manager_ptr_->on_disconnect = std::bind(&udp_manager::rudp_on_disconnected, this, std::placeholders::_1, std::placeholders::_2);
	rudp_manager_ptr_->on_error = std::bind(&udp_manager::rudp_on_error, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	rudp_manager_ptr_->on_read = std::bind(&udp_manager::rudp_on_read, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	if (rudp_manager_ptr_->begin_server(udp_port_, delay_, delay_interval_))
	{
		main_thread::get_instance()->add_log(LOG_TYPE_INFO, "RUDP Start udp_port=%d delay=%d delay_interval=%d", udp_port_, delay_, delay_interval_);
		return true;
	}
	else
	{
		main_thread::get_instance()->add_log(LOG_TYPE_ERROR, "RUDP Start Failed udp_port=%d delay=%d delay_interval=%d", udp_port_, delay_, delay_interval_);
		return false;
	}
}

bool udp_manager::uninit()
{
	//uninit_rudp_server();
	return true;
}

bool udp_manager::rudp_on_read(char* data, int size, int linker_handle, struct sockaddr_in addr)
{
	std::string remote_ip = ustd::rudp_public::get_remote_ip(addr);
	header_protocol *header_ptr = (header_protocol *)(data);
	if(nullptr == header_ptr)
		return false;

	main_thread::get_instance()->add_queue(*header_ptr, (char*)(data + sizeof(header_protocol)), size - sizeof(header_protocol), linker_handle, remote_ip);
	return true;
}

bool udp_manager::rudp_on_check_ip(struct sockaddr_in addr)
{
	std::string remote_ip = ustd::rudp_public::get_remote_ip(addr);
	int remote_port = ustd::rudp_public::get_remote_port(addr);
	main_thread::get_instance()->add_log(LOG_TYPE_DEBUG, "New Linker IP %s Port=%d", remote_ip.c_str(), remote_port);
	return true;
}

void udp_manager::rudp_on_disconnected(const int &linker_handle, struct sockaddr_in addr)
{
	std::string remote_ip = ustd::rudp_public::get_remote_ip(addr);

	header_protocol header_ptr;
	memset(&header_ptr, 0, sizeof(header_ptr));
	header_ptr.protocol_id_ = CLOSE_LINKER;
	main_thread::get_instance()->add_queue(header_ptr, nullptr, 0, linker_handle, remote_ip);
	main_thread::get_instance()->add_log(LOG_TYPE_DEBUG, "Linker Disconnect linker_handle %d", linker_handle);
}

void udp_manager::rudp_on_error(const int &error, const int &linker_handle, struct sockaddr_in addr)
{
	std::string remote_ip = ustd::rudp_public::get_remote_ip(addr);

	header_protocol header_ptr;
	memset(&header_ptr, 0, sizeof(header_ptr));
	header_ptr.protocol_id_ = CLOSE_LINKER;
	main_thread::get_instance()->add_queue(header_ptr, nullptr, 0, linker_handle, remote_ip);
	main_thread::get_instance()->add_log(LOG_TYPE_ERROR, "RUDP ERROR error_id=%d linker_handle=%d ip=%s", error, linker_handle, remote_ip.c_str());
}
