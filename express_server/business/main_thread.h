#pragma once
#ifndef MAIN_THREAD_H_
#define MAIN_THREAD_H_

#include <map>
#include <thread>
#include <list>
#include <mutex>
#include <unordered_map>

#include "../../header/file_public.h"
#include "../../header/file_protocol.h"
#include "udp_manager.h"
#include "multicast_server.h"
#include "write_log.h"
#include "files.h"

struct log_record
{
	int level_;
	char log_char_[1024];
};

struct server_message_buffer
{
	unsigned short message_id_;
	int linker_handle_;
	char ip_[15];
	char *data_;
	int size_;
	server_message_buffer *next_;					
};

struct flow_linker
{
	time_t begin_timer_;
	time_t last_timer_;
	int64_t flow_;
};
struct hls_linker
{
	std::string remote_path_;
	std::string extra_path_;
	std::string file_rename_;
	int64_t max_id_;
	std::string file_path_;
	std::string config_string_;
	time_t begin_timer_;
	time_t last_timer_;
	std::shared_ptr<ustd::files_thread> file_thread_;
};
struct multicast_linker
{
	std::string group_ip_;
	int group_port_;
	std::shared_ptr<multicast_server> multicast_thread_;
};
struct linker_file
{
	int linker_handle_;
	int protocol_;
	flow_linker *flow_;
	hls_linker *hls_;
	multicast_linker *multicast_;
};
typedef std::map<int, std::shared_ptr<linker_file>> linker_file_map;

struct rudp_config
{
	int port_;
	bool delay_;
	int delay_interval_;
};
struct hls_config
{
	int max_files_;
	int delete_files_;
	std::string base_;
	bool echo_;
};
struct multicast_config
{
	std::string group_ip_;
	int group_port_;
};
struct json_config
{
	int web_port_;
	std::string log_path_;
	rudp_config rudp_config_;
	int protocol_;
	hls_config hls_;
	multicast_config multicast_;
	bool tested_;
};

class main_thread
{

public:
	json_config json_config_;
	void init_json();
	void load_json();

public:	
	thread_state_type current_state_ = tst_init;
	main_thread(void);
	~main_thread(void);
	void init();

public:
	udp_manager *udp_manager_ = nullptr;
	ustd::log::write_log *write_log_ptr_ = nullptr;
	std::thread thread_ptr_;
	void execute();

private:
	time_t last_check_timer_ = time(nullptr);
	void check_file_handle();

private:
	std::recursive_mutex linker_file_lock_;
	linker_file_map linker_file_map_;
	void add_linker_file(const int &linker_handle, const std::string &config_string, const std::string &remote_path, const std::string &extra_path, const std::string &file_rename, const int64_t &max_id, const std::string &file_path);
	void add_linker_file(const int &linker_handle, const std::string &group_ip, const int &group_port);
	void add_flow(const int &linker_handle, const uint64 &flow);
	std::shared_ptr<linker_file> find_linker_file(const int &linker_handle);
	void delete_linker_file(const int &linker_handle);

private:
	time_t last_static_timer_ = time(nullptr);
	void check_static();

private:
	void business_dispense();
	void hls_config(server_message_buffer *message_buffer);
	void hls_buffer(server_message_buffer *message_buffer);
	void hls_sended(server_message_buffer *message_buffer);

	void multicast_config(server_message_buffer *message_buffer);
	void multicast_buffer(server_message_buffer *message_buffer);

	void linker_offline(server_message_buffer *message_buffer);

private:
	void send_file_ended(int linker_handle);

private:
	uint64 total_flow_ = 0;

private:
	bool write_file(char *data, int size, int cur_id, std::shared_ptr<linker_file> file_ptr);
	std::string get_local_path(const std::string &extra_path, const std::string &file_name);
	std::string get_local_file(const std::string &extra_path, const std::string &file_name);

public:	
	server_message_buffer *first_ = nullptr, *last_ = nullptr;
	std::recursive_mutex messasge_lock_;
	void add_queue(header_protocol head_ptr, char *data, int size, int linker_handle, std::string ip);
	void free_queue();

public:
	std::recursive_mutex log_lock_;
	void add_log(const int &log_type, const char *log_text_format, ...);
	void add_on_log(const int &message_type, const std::string &message);

public:
	std::recursive_mutex send_lock_;
	int send_buffer_to_server(char *data, const int &size, const int &linker_handle);
	int send_buffer_to_server(const unsigned short &message_id, char *data, const int &size, const int &linker_handle);
	int send_buffer_to_server(const unsigned short &message_id, const int &cur_id, char *data, const int &size, const int &linker_handle);

public:
	static main_thread *get_instance()
	{
		static main_thread *m_pInstance = nullptr;
		if (m_pInstance == nullptr)  
		{
			m_pInstance = new main_thread();
		}
		return m_pInstance; 
	}
};
#endif  // MAIN_THREAD_H_
