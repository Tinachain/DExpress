/*
 * multicast_manager.cpp
 *
 *  Created on: 2019年1月2日
 *      Author: fxh7622
 */

#include "multicast_manager.h"
#include "main_thread.h"

multicast_manager::multicast_manager(const std::string &group_ip, const int &group_port)
{
	group_ip_ = group_ip;
	group_port_ = group_port;
}

multicast_manager::~multicast_manager(void)
{
	if(nullptr != multicast_client_ptr_)
	{
		delete multicast_client_ptr_;
		multicast_client_ptr_ = nullptr;
	}
}

bool multicast_manager::init()
{
	//创建组播对象;
	multicast_client_ptr_ = new multicast_client();
	multicast_client_ptr_->multicast_on_error = std::bind(&multicast_manager::multicast_on_error, this, std::placeholders::_1);
	multicast_client_ptr_->multicast_on_recv = std::bind(&multicast_manager::multicast_on_recv, this, std::placeholders::_1, std::placeholders::_2);
	multicast_client_ptr_->log_ptr_ = log_ptr_;

	return multicast_client_ptr_->begin_multicast(group_ip_, group_port_);
}

void multicast_manager::multicast_on_recv(char *data, const int &size)
{
	main_thread *main_thread_ = main_thread::get_instance();
	main_thread_->send_buffer_to_server(MULTICAST_BUFFER, data, size);
}

void multicast_manager::multicast_on_error(const int &error)
{
	/*main_thread *main_thread_ = main_thread::get_instance();
	header_protocol header_ptr;
	memset(&header_ptr, 0, sizeof(header_ptr));
	header_ptr.protocol_id_ = CLOSE_LINKER;
	main_thread_->add_queue(header_ptr, nullptr, 0, linker_handle);*/
}

void multicast_manager::add_log(const int log_type, const char *context, ...)
{
	const int array_length = 1024 * 10;
	char log_text[array_length];
	memset(log_text, 0x00, array_length);

	va_list arg_ptr;
	va_start(arg_ptr, context);
	int result = vsprintf(log_text, context, arg_ptr);
	va_end(arg_ptr);
	if (result <= 0)
		return;

	if (result > array_length)
		return;

	if(nullptr != log_ptr_)
	{
		log_ptr_->write_log3(log_type, log_text);
	}
}


