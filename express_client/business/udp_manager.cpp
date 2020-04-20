
#include <thread>
#include <mutex>

#include "udp_manager.h"
#include "rudp_public.h"
#include "main_thread.h"
#include "file_protocol.h"
#include "path.h"
#include "cJSON.h"

udp_manager::udp_manager(const std::string &ip, const int &port, const bool &delay, const int &delay_interval)
{
	udp_ip_ = ip;
	udp_port_ = port;
	delay_ = delay;
	delay_interval_ = delay_interval;
}

udp_manager::~udp_manager(void)
{
}

int udp_manager::connect_server()
{
	int time_out = 5 * 1000;
	rudp_linker *linker_ptr = rudp_manager_ptr_->begin_client(udp_ip_, udp_port_, 0, delay_, delay_interval_);
	if (nullptr != linker_ptr)
	{
		const int timer_space = 200;
		int flag_count = 0;
		int connect_count = time_out / timer_space;
		while(1)
		{
			//延时;
			ustd::rudp_public::sleep_delay(timer_space);
			flag_count++;

			//状态判断
			if (OPEN == linker_ptr->get_rudp_state())
				return linker_ptr->get_linker_handle();

			//链接失败
			if (flag_count >= connect_count)
				return ERROR_HANDLE;
		}
	}
	return ERROR_HANDLE;
}

void udp_manager::init()
{
	//创建UDP对象;
	rudp_manager_ptr_ = new rudp_manager(main_thread::get_instance()->json_config_.log_path_);
	rudp_manager_ptr_->init();
	rudp_manager_ptr_->on_connect = std::bind(&udp_manager::rudp_on_connect, this, std::placeholders::_1, std::placeholders::_2);
	rudp_manager_ptr_->on_disconnect = std::bind(&udp_manager::rudp_on_disconnected, this, std::placeholders::_1);
	rudp_manager_ptr_->on_error = std::bind(&udp_manager::rudp_on_error, this, std::placeholders::_1, std::placeholders::_2);
	rudp_manager_ptr_->on_read = std::bind(&udp_manager::rudp_on_recv, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

int udp_manager::connect()
{
	linker_handle_ = connect_server();
	if (ERROR_HANDLE == linker_handle_)
	{
		main_thread::get_instance()->add_log(LOG_TYPE_ERROR, "begin_client Failed Udp_Ip=%s Udp_Port=%d", udp_ip_.c_str(), udp_port_);
		return linker_handle_;
	}
	else
	{
		main_thread::get_instance()->add_log(LOG_TYPE_INFO, "begin_client Success Udp_Ip=%s Udp_Port=%d", udp_ip_.c_str(), udp_port_);
		return linker_handle_;
	}
}

bool udp_manager::uninit()
{
	//uninit_rudp_server();
	return true;
}

bool udp_manager::rudp_on_recv(char* data, int size, int linker_handle)
{
	header_protocol *header_ptr = (header_protocol *)(data);
	if(nullptr == header_ptr)
		return false;

	main_thread::get_instance()->add_queue(*header_ptr, (char*)(data + sizeof(header_protocol)), size - sizeof(header_protocol), linker_handle);
	return true;
}

void udp_manager::rudp_on_connect(const struct sockaddr_in &addr, const int &linker_handle)
{
	//main_thread::get_instance()->add_log(LOG_TYPE_INFO, "New Linker IP %s", ip.c_str());
}

void udp_manager::rudp_on_disconnected(const int &linker_handle)
{
	main_thread *main_thread_ = main_thread::get_instance();

	header_protocol header_ptr;
	memset(&header_ptr, 0, sizeof(header_ptr));
	header_ptr.protocol_id_ = CLOSE_LINKER;
	main_thread_->add_queue(header_ptr, nullptr, 0, linker_handle);

	main_thread::get_instance()->add_log(LOG_TYPE_INFO, "Linker Disconnect linker_handle %d", linker_handle);
}

void udp_manager::rudp_on_error(const int &error, const int &linker_handle)
{
	main_thread *main_thread_ = main_thread::get_instance();

	header_protocol header_ptr;
	memset(&header_ptr, 0, sizeof(header_ptr));
	header_ptr.protocol_id_ = CLOSE_LINKER;
	main_thread_->add_queue(header_ptr, nullptr, 0, linker_handle);
}
