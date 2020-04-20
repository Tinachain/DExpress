#pragma once
#ifndef MAIN_THREAD_H_
#define MAIN_THREAD_H_

#include <map>
#include <thread>
#include <list>
#include <mutex>
#include <unordered_map>
#include "cJSON.h"
#include "file_public.h"
#include "file_protocol.h"
#include "udp_manager.h"
#include "multicast_manager.h"
#include "write_log.h"
#include "web_protocol_manager.h"
#include "multicast_client.h"
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
	char *data_;
	int size_;
	server_message_buffer *next_;					
};

//web的消息处理;
struct web_message_buffer
{
	std::string message_type_;
	std::string params_;
	struct evhttp_request *request_;
	web_message_buffer *next_;
};

//发送文件的请求;
enum file_request_state{trs_init, trs_sending};
struct file_request
{
	std::string file_path_;
	std::string file_extra_;
	std::string rename_;
	time_t last_timer_;
	time_t send_timer_;
	file_request_state state_;
};

struct sending_file
{
	time_t sending_timer_;
	std::string file_path_;
	std::string file_extra_;
	std::string rename_;
};

struct file_speed
{
	std::string file_path_;
	int file_size_;
};

struct rudp_config
{
	std::string ip_;
	int port_;
	bool start_;
	bool delay_;
	int delay_interval_;
	int reconnect_;
};

struct hls_config
{
	int concurrency_;
	int max_files_;
	int delete_files_;
	std::string base_;
	int resend_interval_;
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

//public:
//	std::string get_value_string(cJSON *root, std::vector<std::string> child_vector, std::string key);
//	int get_value_int(cJSON *root, std::vector<std::string> child_vector, std::string key);
//	bool get_value_bool(cJSON *root, std::vector<std::string> child_vector, std::string key);

public:	
	thread_state_type current_state_ = tst_init;
	main_thread(void);
	~main_thread(void);
	void init();

public:
	std::thread thread_ptr_;
	void execute();

public:
	server_message_buffer *first_ = nullptr, *last_ = nullptr;
	std::recursive_mutex messasge_lock_;
	void add_queue(header_protocol head_ptr, char *data, int size, int linker_handle);
	void free_queue();

private:
	void business_dispense();

	void hls_sended(server_message_buffer *message);
	void hls_config(server_message_buffer *message);
	void hls_buffer(server_message_buffer *message);

	void rudp_offline(server_message_buffer *message);

private:
	std::string config_string_ = "";
	void send_file_ended(int linker_handle);

private:
	//文件删除管理;
	ustd::files_thread *file_thread_ = nullptr;
	ustd::files_thread *echo_file_thread_ = nullptr;

private:
	time_t last_check_timer_ = time(nullptr);
	void check_file_handle();

private:
	std::string remote_path_ = "";
	std::string extra_path_ = "";
	std::string file_path_ = "";
	std::string file_name_ = "";
	std::string file_rename_ = "";
	int64_t file_size_ = 0;
	int64_t max_id_ = 0;
	int64_t cur_id_ = 0;
	int file_handle_ = -1;
	time_t begin_timer_;
	bool write_file(char *data, int size, int cur_id);
	std::string get_local_path(const std::string &extra_path, const std::string &file_name);
	std::string get_local_file(const std::string &extra_path, const std::string &file_name);

public:
	//Web相关管理;
	web_protocol *web_protocol_ = nullptr;
	web_message_buffer *web_first_ = nullptr, *web_last_ = nullptr;
	std::recursive_mutex web_messasge_lock_;
	void add_web_queue(std::string message_type, std::string params, struct evhttp_request *request);
	void free_web_queue();

private:
	void web_business_dispense();
	void web_dispense(web_message_buffer *message);
	void web_post_message(web_message_buffer *message);

public:
	//RUDP传输相关管理;
	udp_manager *udp_manager_ = nullptr;
	int linker_handle_ = -1;
	std::recursive_mutex send_lock_;
	int send_buffer_to_server(char *data, const int &size);
	int send_buffer_to_server(const unsigned short &message_id, char *data, const int &size);
	int send_buffer_to_server(const unsigned short &message_id, const int &cur_id, char *data, const int &size);

private:
	bool rudp_disconnected_ = true;
	time_t rudp_disconnected_timer_= time(nullptr);
	void check_rudp_disconnect();

private:
	//HLS文件传输相关管理;
	std::recursive_mutex file_lock_;
	std::list<std::shared_ptr<file_request>> file_request_list_;
	time_t check_file_timer_ = time(nullptr);
	void add_file_request(const std::string &file_path, const std::string &file_extra, std::string &rename);
	std::shared_ptr<file_request> get_init_file();
	std::shared_ptr<file_request> find_file_request(const std::string &file_path);
	bool set_file_state(const std::string &file_path, const file_request_state &state);
	void delete_file_request(const std::string &file_path);
	int get_sending_file_count();
	void free_file_request();

private:
	void init_file_request_state();
	void check_file_timer();
	void check_file_request();
	bool read_file(const std::string &file_path, const std::string &extra_path, const std::string &file_name);

public:
	//日志相关管理;
	ustd::log::write_log *write_log_ptr_ = nullptr;
	std::recursive_mutex log_lock_;
	void add_log(const int &log_type, const char *log_text_format, ...);
	void add_on_log(const int &message_type, const std::string &message);

public:
	multicast_manager *multicast_manager_ = nullptr;
	void send_multicast_config(const std::string &group_ip, const int &group_port);

private:
	time_t last_show_timer_= time(nullptr);
	void show_static();

private:
	//速度统计;
	std::recursive_mutex speed_lock_;
	std::list<std::shared_ptr<file_speed>> speed_list_;
	void add_speed(const std::string &file_path, const int &size);
	int get_speed();

private:
	//模拟测试数据;
	time_t last_simulation_timer_= time(nullptr);
	void simulation_hls_message();
	void check_simulation();

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
