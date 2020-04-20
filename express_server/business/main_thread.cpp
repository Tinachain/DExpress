
#include "main_thread.h"
#include "path.h"
#include <time.h>
#include <fstream>
#include <string.h>
#include "file_protocol.h"
#include "ini.h"
#include "write_log.h"
#include "udp_manager.h"
#include "json.h"
#include "string_list.h"
#include "fcntl.h"
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include "json.h"

main_thread::main_thread()
{
	init_json();
	load_json();
	write_log_ptr_ = new ustd::log::write_log();
	write_log_ptr_->init("RUDP File Server Manager", json_config_.log_path_.c_str(), 1);
}

void main_thread::init_json()
{
	json_config_.log_path_ = "hole_log";

	//rudp的配置;
	json_config_.rudp_config_.port_ = 9999;
	json_config_.rudp_config_.delay_ = false;
	json_config_.rudp_config_.delay_interval_ = 2000;

	json_config_.protocol_ = 0;

	//HLS协议;
	json_config_.hls_.base_ = "/home/fxh7622/channellist";
	json_config_.hls_.max_files_ = 200;
	json_config_.hls_.delete_files_ = 100;
	json_config_.hls_.echo_ = false;

	//组播协议;
	json_config_.multicast_.group_ip_ = "";
	json_config_.multicast_.group_port_ = 0;
}

void main_thread::load_json()
{
	std::string json_path = ustd::path::get_app_path() + "/config.json";
	if (!ustd::path::is_file_exist(json_path))
	{
		return;
	}
	printf("path=%s", json_path.c_str());

	//
	long long file_size = ustd::path::get_file_size(json_path);
	int readhandle = open(json_path.c_str(), O_RDONLY);
	if(-1 == readhandle)
	{
		return;
	}

	char *file_buffer = new char[file_size];
	memset(file_buffer, 0, file_size);
	int read_len = read(readhandle, file_buffer, file_size);
	if(read_len != file_size)
	{
		delete[] file_buffer;
		file_size = 0;
		close(readhandle);
		return;
	}
	close(readhandle);

	std::string params_string(file_buffer, file_size);
	add_log(LOG_TYPE_INFO, "\n %s\n", params_string.c_str());

	cJSON *root = cJSON_Parse(params_string.c_str());
	if(nullptr == root)
	{
		return;
	}

	std::vector<std::string> tmp_vector;
	tmp_vector.push_back("log");
	json_config_.log_path_ = ustd::json::get_value_string(root, tmp_vector, "path");

	tmp_vector.clear();
	tmp_vector.push_back("rudp");
	json_config_.rudp_config_.port_ = ustd::json::get_value_int(root, tmp_vector, "port");
	json_config_.rudp_config_.delay_ = ustd::json::get_value_bool(root, tmp_vector, "delay");
	json_config_.rudp_config_.delay_interval_ = ustd::json::get_value_int(root, tmp_vector, "delay_interval");

	tmp_vector.clear();
	json_config_.protocol_ = ustd::json::get_value_int(root, tmp_vector, "protocol");

	tmp_vector.clear();
	tmp_vector.push_back("hls");
	json_config_.hls_.echo_ = ustd::json::get_value_bool(root, tmp_vector, "echo");

	tmp_vector.push_back("files");
	json_config_.hls_.max_files_ = ustd::json::get_value_int(root, tmp_vector, "max");
	json_config_.hls_.delete_files_ = ustd::json::get_value_int(root, tmp_vector, "delete");

	tmp_vector.clear();
	tmp_vector.push_back("hls");
	tmp_vector.push_back("file");
	json_config_.hls_.base_ = ustd::json::get_value_string(root, tmp_vector, "base");

	tmp_vector.clear();
	tmp_vector.push_back("multicast");
	json_config_.multicast_.group_ip_ = ustd::json::get_value_string(root, tmp_vector, "group_ip");
	json_config_.multicast_.group_port_ = ustd::json::get_value_int(root, tmp_vector, "group_port");

	cJSON_Delete(root);
}

void main_thread::init()
{
	add_log(LOG_TYPE_INFO, "Create Main Thread");
	thread_ptr_ = std::thread(&main_thread::execute, this);
	thread_ptr_.detach();
}

main_thread::~main_thread(void)
{
	if(current_state_ == tst_runing)
	{
		current_state_ = tst_stoping;
		time_t last_timer = time(nullptr);
		int timer_interval = 0;
		while((timer_interval <= 2))
		{
			time_t current_timer = time(nullptr);
			timer_interval = static_cast<int>(difftime(current_timer, last_timer));
			if(current_state_ == tst_stoped)
			{
				break;
			}
			ustd::rudp_public::sleep_delay(50);
		}
	}
	if(nullptr != udp_manager_)
	{
		udp_manager_->uninit();
		delete udp_manager_;
		udp_manager_ = nullptr;
	}
	free_queue();
}

void main_thread::add_queue(header_protocol head_ptr, char *data, int size, int linker_handle, std::string ip)
{
	server_message_buffer *buffer_ptr = new server_message_buffer();
	buffer_ptr->message_id_ = head_ptr.protocol_id_;
	buffer_ptr->linker_handle_ = linker_handle;
	buffer_ptr->next_ = nullptr;
	buffer_ptr->size_ = size;
	buffer_ptr->data_ = nullptr;
	memcpy(buffer_ptr->ip_, ip.c_str(), ip.length());
	if (size > 0)
	{
		buffer_ptr->data_ = new char[size];
		memcpy(buffer_ptr->data_, data, size);
	}	

	{
		std::lock_guard<std::recursive_mutex> gurad(messasge_lock_);

		if(first_ != nullptr)
			last_->next_ = buffer_ptr;
		else
			first_ = buffer_ptr;

		last_ = buffer_ptr;
	}
}

void main_thread::free_queue()
{
	server_message_buffer *next_ptr = nullptr;
	while(first_ != nullptr)
	{
		next_ptr = first_->next_;
		if(first_->data_ != nullptr)
		{
			delete[] first_->data_;
		}
		delete first_;
		first_ = next_ptr;
	}
	first_ = nullptr;
	last_ = nullptr;
}

void main_thread::execute()
{
	add_log(LOG_TYPE_INFO, "Create RUDP Manager");
	udp_manager_ = new udp_manager(json_config_.rudp_config_.port_, json_config_.rudp_config_.delay_, json_config_.rudp_config_.delay_interval_);
	udp_manager_->init();
	current_state_ = tst_runing;
	while(tst_runing == current_state_)
	{
		business_dispense();
		check_static();
		ustd::rudp_public::sleep_delay(50);
	}
	current_state_ = tst_stoped;
}

void main_thread::check_static()
{
	time_t curr_time = time(nullptr);
	int second = static_cast<int>(difftime(curr_time, last_static_timer_));
	if(second >= STATIC_TIME)
	{
		add_log(LOG_TYPE_INFO, "****Static Flow ****");
		for(auto iter = linker_file_map_.begin(); iter != linker_file_map_.end(); iter++)
		{
			std::shared_ptr<linker_file> linker_file_ptr = iter->second;
			add_log(LOG_TYPE_INFO, "↓↓↓↓Single Flow Static Linker Handle %d Flow %s", linker_file_ptr->linker_handle_, ustd::string_list::flow_show(linker_file_ptr->flow_->flow_).c_str());
		}
		last_static_timer_ = time(nullptr);
	}
}

void main_thread::business_dispense()
{
	server_message_buffer *work_ptr = nullptr, *next_ptr = nullptr;
	{
		std::lock_guard<std::recursive_mutex> gurad(messasge_lock_);
		if(work_ptr == nullptr && first_ != nullptr)
		{
			work_ptr = first_;
			first_ = nullptr;
			last_ = nullptr;
		}
	}

	while(work_ptr != nullptr)
	{
		next_ptr = work_ptr->next_;
		switch(work_ptr->message_id_)
		{
			//HLS协议
			case HLS_CONFIG:				//配置信息
			{
				hls_config(work_ptr);
				break;
			}
			case HLS_FILE_BUFFER:			//文件数据
			{
				hls_buffer(work_ptr);
				break;
			}
			case HLS_FILE_BUFFER_ENDED:	//返回文件接收完毕;
			{
				hls_sended(work_ptr);
				break;
			}
			//组播协议;
			case MULTICAST_CONFIG:			//组播配置;
			{
				multicast_config(work_ptr);
				break;
			}
			case MULTICAST_BUFFER:			//组播数据;
			{
				multicast_buffer(work_ptr);
				break;
			}
			//RUDP协议;
			case CLOSE_LINKER:				//关闭信息
			{
				linker_offline(work_ptr);
				break;
			}
			default:
			{
				add_log(LOG_TYPE_ERROR, "Not Found Message Id %d", work_ptr->message_id_);
				break;
			}
		}
		if (work_ptr->data_ != nullptr && work_ptr->size_ > 0)
		{
			delete[] work_ptr->data_;
		}
		delete work_ptr;
		work_ptr = next_ptr;
	}
}

void main_thread::multicast_config(server_message_buffer *message_buffer)
{
	std::string json_string(message_buffer->data_, message_buffer->size_);

	cJSON *root = cJSON_Parse(json_string.c_str());
	if(nullptr == root)
	{
		add_log(LOG_TYPE_ERROR, "multicast_config Parse Config File Failed file_buffer=%s error %s\n", json_string.c_str(), cJSON_GetErrorPtr());
		return;
	}

	std::vector<std::string> tmp_vector;
	std::string group_ip = ustd::json::get_value_string(root, tmp_vector, "group_ip");
	int group_port = ustd::json::get_value_int(root, tmp_vector, "group_port");
	add_linker_file(message_buffer->linker_handle_, group_ip, group_port);
	add_log(LOG_TYPE_INFO, "Multicast Config Group IP %s Group Port %d", group_ip.c_str(), group_port);
	cJSON_Delete(root);
}

void main_thread::multicast_buffer(server_message_buffer *message_buffer)
{
	std::shared_ptr<linker_file> linker_file_ptr = find_linker_file(message_buffer->linker_handle_);
	if(nullptr == linker_file_ptr)
	{
		return;
	}

	add_flow(message_buffer->linker_handle_, message_buffer->size_);
	if(1 == linker_file_ptr->protocol_)
	{
		if(nullptr != linker_file_ptr->multicast_->multicast_thread_)
		{
			linker_file_ptr->multicast_->multicast_thread_->add_queue(message_buffer->data_, message_buffer->size_);
		}
	}
}

void main_thread::hls_config(server_message_buffer *message_buffer)
{
	std::string json_string(message_buffer->data_, message_buffer->size_);

	cJSON *root = cJSON_Parse(json_string.c_str());
	if(nullptr == root)
	{
		add_log(LOG_TYPE_ERROR, "Json Parse Failed %s", json_string.c_str());
		return;
	}

	int protocol = ustd::json::get_value_int(root, "protocol");
	if(0 != protocol)
	{
		add_log(LOG_TYPE_ERROR, "Not Know Protocol protocol=%d", protocol);
		return;
	}

	std::vector<std::string> tmp_vector;
	tmp_vector.push_back("hls");
	std::string remote_path = ustd::json::get_value_string(root, tmp_vector, "path");
	std::string extra_path = ustd::json::get_value_string(root, tmp_vector, "extra");
	std::string file_rename = ustd::json::get_value_string(root, tmp_vector, "rename");
	int64_t max_id = ustd::json::get_value_int(root, tmp_vector, "maxId");
	std::string cur_path = get_local_path(extra_path, file_rename);

	if (!ustd::path::create_directory(cur_path.c_str()))
	{
		add_log(LOG_TYPE_ERROR, "Create Dir Failed cur_path=%s", cur_path.c_str());
		return;
	}

	std::string cur_file_path = get_local_file(extra_path, file_rename);
	if (ustd::path::is_file_exist(cur_file_path))
	{
		if (!ustd::path::remove_file(cur_file_path))
		{
			add_log(LOG_TYPE_ERROR, "File Already Exit name=%s cur_file_path=%s", file_rename.c_str(), cur_file_path.c_str());
			return;
		}
	}

	int file_handle = open(cur_file_path.c_str(), O_CREAT|O_RDWR , S_IRUSR | S_IWUSR);
	if (ERROR_HANDLE == file_handle)
	{
		add_log(LOG_TYPE_ERROR, "Open Function Failed path=%s errno=%d", cur_file_path.c_str(), errno);
		return;
	}
	close(file_handle);
	add_linker_file(message_buffer->linker_handle_,json_string,remote_path,extra_path,file_rename,max_id,cur_file_path);

	if (json_config_.hls_.echo_)
	{
		send_buffer_to_server(message_buffer->message_id_, message_buffer->data_, message_buffer->size_, message_buffer->linker_handle_);
	}
}

std::string main_thread::get_local_path(const std::string &extra_path, const std::string &file_name)
{
	if (extra_path == "")
		return json_config_.hls_.base_;
	else
		return json_config_.hls_.base_ + "/" + extra_path;
}

std::string main_thread::get_local_file(const std::string &extra_path, const std::string &file_name)
{
	if (extra_path == "")
		return json_config_.hls_.base_ + "/" + file_name;
	else
		return json_config_.hls_.base_ + "/" + extra_path + "/" + file_name;
}

bool main_thread::write_file(char *data, int size, int cur_id, std::shared_ptr<linker_file> file_ptr)
{
	int file_handle = open(file_ptr->hls_->file_path_.c_str(), O_CREAT|O_RDWR , S_IRUSR | S_IWUSR);
	if (ERROR_HANDLE == file_handle)
	{
		add_log(LOG_TYPE_ERROR, "Open Function Failed path=%s errno=%d", file_ptr->hls_->file_path_.c_str(), errno);
		return false;
	}

	lseek(file_handle, 0, SEEK_END);
	ssize_t ret = write(file_handle, data, size);
	close(file_handle);
	if (-1 == ret)
	{
		add_log(LOG_TYPE_ERROR, "Write File Function Failed path=%s  errno=%d size=%d", file_ptr->hls_->file_path_.c_str(), errno, size);
		return false;
	}
	if (cur_id == file_ptr->hls_->max_id_)
	{
		return true;
	}
	return false;
}

void main_thread::hls_buffer(server_message_buffer *message_buffer)
{
	if(json_config_.hls_.echo_)
	{
		send_buffer_to_server(message_buffer->message_id_, message_buffer->data_, message_buffer->size_, message_buffer->linker_handle_);
	}

	file_protocol *file_ptr = (file_protocol *)message_buffer->data_;
	std::shared_ptr<linker_file> linker_file_ptr = find_linker_file(message_buffer->linker_handle_);
	if(nullptr == linker_file_ptr)
	{
		return;
	}

	bool ret = write_file(message_buffer->data_ + sizeof(file_protocol), message_buffer->size_ - sizeof(file_protocol), file_ptr->cur_id_, linker_file_ptr);
	if (ret)
	{
		send_file_ended(message_buffer->linker_handle_);
		std::string ext = ustd::path::get_file_ext(linker_file_ptr->hls_->file_path_);
		transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		if (ext != "m3u8")
		{
			linker_file_ptr->hls_->file_thread_->add_queue(linker_file_ptr->hls_->file_path_);
		}
	}
	add_flow(message_buffer->linker_handle_, message_buffer->size_);
}

void main_thread::hls_sended(server_message_buffer *message)
{
	std::string json_string(message->data_, message->size_);

	cJSON *root = cJSON_Parse(json_string.c_str());
	if(nullptr == root)
	{
		add_log(LOG_TYPE_ERROR, "Json Parse Failed %s", json_string.c_str());
		return;
	}

	int protocol = ustd::json::get_value_int(root, "protocol");
	if(0 != protocol)
	{
		add_log(LOG_TYPE_ERROR, "Not Know Protocol protocol=%d", protocol);
		return;
	}

	std::vector<std::string> tmp_vector;
	tmp_vector.push_back("hls");
	std::string remote_path = ustd::json::get_value_string(root, tmp_vector, "path");
	std::string rename = ustd::json::get_value_string(root, tmp_vector, "rename");

	cJSON_Delete(root);
}


void main_thread::send_file_ended(int linker_handle)
{
	header_protocol header_ptr;
	memset(&header_ptr, 0, sizeof(header_ptr));
	header_ptr.protocol_id_ = HLS_FILE_BUFFER_ENDED;

	std::shared_ptr<linker_file> file_ptr = find_linker_file(linker_handle);
	if(nullptr == file_ptr)
		return;

	int buffer_len = strlen(file_ptr->hls_->config_string_.c_str()) + sizeof(header_ptr);
	char *buffer_ptr = new char[buffer_len];
	memcpy(buffer_ptr, &header_ptr, sizeof(header_ptr));

	if (strlen(file_ptr->hls_->config_string_.c_str()) > 0)
		memcpy(buffer_ptr + sizeof(header_ptr), file_ptr->hls_->config_string_.c_str(), strlen(file_ptr->hls_->config_string_.c_str()));

	udp_manager_->rudp_manager_ptr_->send_buffer(buffer_ptr, buffer_len, linker_handle);
	delete[] buffer_ptr;
	return;
}

void main_thread::linker_offline(server_message_buffer *message_buffer)
{
	std::shared_ptr<linker_file> linker_file_ptr = find_linker_file(message_buffer->linker_handle_);
	if(nullptr == linker_file_ptr)
		return;

	delete_linker_file(message_buffer->linker_handle_);
}

void main_thread::add_on_log(const int &message_type, const std::string &message)
{
	std::lock_guard<std::recursive_mutex> guard_log(log_lock_);

	write_log_ptr_->write_log3(message_type, message.c_str());
}

void main_thread::add_log(const int &log_type, const char *log_text_format, ...)
{
	std::lock_guard<std::recursive_mutex> guard_log(log_lock_);

	const int array_length = 1024 * 10;
	char log_text[array_length] = {0};
	memset(log_text, 0x00, array_length);

	va_list arg_ptr;
	va_start(arg_ptr, log_text_format);
	int result = vsprintf(log_text, log_text_format, arg_ptr);
	va_end(arg_ptr);
	if (result <= 0)
		return;

	if (result > array_length)
		return;

	if(nullptr != write_log_ptr_)
	{
		write_log_ptr_->write_log3(log_type, log_text);
	}
}

int main_thread::send_buffer_to_server(char *data, const int &size, const int &linker_handle)
{
	std::lock_guard<std::recursive_mutex> guard(send_lock_);

	udp_manager_->rudp_manager_ptr_->send_buffer(data, size, linker_handle);
	return 0;
}

int main_thread::send_buffer_to_server(const unsigned short &message_id, char *data, const int &size, const int &linker_handle)
{
	std::lock_guard<std::recursive_mutex> guard(send_lock_);

	header_protocol tmp_header_ptr;
	memset(&tmp_header_ptr, 0, sizeof(tmp_header_ptr));
	tmp_header_ptr.protocol_id_ = message_id;

	int buffer_len = size + sizeof(tmp_header_ptr);
	char *buffer_ptr = new char[buffer_len];
	memcpy(buffer_ptr, &tmp_header_ptr, sizeof(tmp_header_ptr));

	if (size > 0)
		memcpy(buffer_ptr + sizeof(tmp_header_ptr), data, size);

	udp_manager_->rudp_manager_ptr_->send_buffer(buffer_ptr, buffer_len, linker_handle);

	delete[] buffer_ptr;
	return size;
}

int main_thread::send_buffer_to_server(const unsigned short &message_id, const int &cur_id, char *data, const int &size, const int &linker_handle)
{
	std::lock_guard<std::recursive_mutex> guard(send_lock_);

	header_protocol tmp_header_ptr;
	memset(&tmp_header_ptr, 0, sizeof(tmp_header_ptr));
	tmp_header_ptr.protocol_id_ = message_id;

	file_protocol tmp_file_protocol_ptr;
	memset(&tmp_file_protocol_ptr, 0, sizeof(tmp_file_protocol_ptr));
	tmp_file_protocol_ptr.cur_id_ = cur_id;

	int buffer_len = size + sizeof(tmp_header_ptr) + sizeof(tmp_file_protocol_ptr);
	char *buffer_ptr = new char[buffer_len];
	memcpy(buffer_ptr, &tmp_header_ptr, sizeof(tmp_header_ptr));
	memcpy(buffer_ptr + sizeof(tmp_header_ptr), &tmp_file_protocol_ptr, sizeof(tmp_file_protocol_ptr));

	if (size > 0)
		memcpy(buffer_ptr + sizeof(tmp_header_ptr) + sizeof(tmp_file_protocol_ptr), data, size);

	udp_manager_->rudp_manager_ptr_->send_buffer(buffer_ptr, buffer_len, linker_handle);

	delete[] buffer_ptr;
	return size;
}

void main_thread::add_linker_file(const int &linker_handle, const std::string &config_string, const std::string &remote_path, const std::string &extra_path, const std::string &file_rename, const int64_t &max_id, const std::string &file_path)
{
	std::lock_guard<std::recursive_mutex> guard(linker_file_lock_);

	linker_file_map::iterator iter = linker_file_map_.find(linker_handle);
	if(iter != linker_file_map_.end())
	{
		std::shared_ptr<linker_file> file_ptr = iter->second;

		file_ptr->linker_handle_ = linker_handle;
		file_ptr->protocol_ = 0;
		file_ptr->hls_->config_string_ = config_string;
		file_ptr->hls_->remote_path_ = remote_path;
		file_ptr->hls_->extra_path_ = extra_path;
		file_ptr->hls_->file_rename_ = file_rename;
		file_ptr->hls_->max_id_ = max_id;
		file_ptr->hls_->file_path_ = file_path;
		file_ptr->hls_->begin_timer_ = time(nullptr);
		file_ptr->hls_->last_timer_ = time(nullptr);
	}
	else
	{
		std::shared_ptr<linker_file> file_ptr(new linker_file);

		file_ptr->linker_handle_ = linker_handle;
		file_ptr->protocol_ = 0;

		file_ptr->flow_ = new flow_linker;
		file_ptr->flow_->begin_timer_ = time(nullptr);
		file_ptr->flow_->last_timer_ = time(nullptr);
		file_ptr->flow_->flow_ = 0;

		file_ptr->hls_ = new hls_linker;
		file_ptr->hls_->config_string_ = config_string;
		file_ptr->hls_->remote_path_ = remote_path;
		file_ptr->hls_->extra_path_ = extra_path;
		file_ptr->hls_->file_rename_ = file_rename;
		file_ptr->hls_->max_id_ = max_id;
		file_ptr->hls_->file_path_ = file_path;
		file_ptr->hls_->begin_timer_ = time(nullptr);
		file_ptr->hls_->last_timer_ = time(nullptr);

		std::shared_ptr<ustd::files_thread> file_thread_ptr(new ustd::files_thread());
		file_ptr->hls_->file_thread_ = file_thread_ptr;
		file_ptr->hls_->file_thread_->init(json_config_.hls_.max_files_, json_config_.hls_.delete_files_);

		file_ptr->multicast_ = nullptr;

		linker_file_map_.insert(std::make_pair(file_ptr->linker_handle_, file_ptr));
	}
}

void main_thread::add_linker_file(const int &linker_handle, const std::string &group_ip, const int &group_port)
{
	std::lock_guard<std::recursive_mutex> guard(linker_file_lock_);

	linker_file_map::iterator iter = linker_file_map_.find(linker_handle);
	if(iter != linker_file_map_.end())
	{
		std::shared_ptr<linker_file> file_ptr = iter->second;

		file_ptr->linker_handle_ = linker_handle;
		file_ptr->protocol_ = 1;
		file_ptr->multicast_->group_ip_ = group_ip;
		file_ptr->multicast_->group_port_ = group_port;
	}
	else
	{
		std::shared_ptr<linker_file> file_ptr(new linker_file);

		file_ptr->linker_handle_ = linker_handle;
		file_ptr->protocol_ = 1;

		file_ptr->hls_ = nullptr;

		file_ptr->flow_ = new flow_linker;
		file_ptr->flow_->begin_timer_ = time(nullptr);
		file_ptr->flow_->last_timer_ = time(nullptr);
		file_ptr->flow_->flow_ = 0;

		file_ptr->multicast_ = new multicast_linker;
		file_ptr->multicast_->group_ip_ = group_ip;
		file_ptr->multicast_->group_port_ = group_port;

		std::shared_ptr<multicast_server> multicast_thread_ptr(new multicast_server());
		file_ptr->multicast_->multicast_thread_ = multicast_thread_ptr;
		file_ptr->multicast_->multicast_thread_->write_log_ptr_ = write_log_ptr_;
		file_ptr->multicast_->multicast_thread_->begin_multicast(file_ptr->multicast_->group_ip_, file_ptr->multicast_->group_port_);
		linker_file_map_.insert(std::make_pair(file_ptr->linker_handle_, file_ptr));
	}
}

void main_thread::add_flow(const int &linker_handle, const uint64 &flow)
{
	std::lock_guard<std::recursive_mutex> guard(linker_file_lock_);

	linker_file_map::iterator iter = linker_file_map_.find(linker_handle);
	if(iter != linker_file_map_.end())
	{
		std::shared_ptr<linker_file> file_ptr = iter->second;
		file_ptr->flow_->flow_ += flow;
		file_ptr->flow_->last_timer_ = time(nullptr);
		total_flow_ += flow;
	}
}

std::shared_ptr<linker_file> main_thread::find_linker_file(const int &linker_handle)
{
	std::lock_guard<std::recursive_mutex> guard(linker_file_lock_);

	linker_file_map::iterator iter = linker_file_map_.find(linker_handle);
	if(iter != linker_file_map_.end())
	{
		return iter->second;
	}
	return nullptr;
}

void main_thread::delete_linker_file(const int &linker_handle)
{
	std::lock_guard<std::recursive_mutex> guard(linker_file_lock_);

	linker_file_map::iterator iter = linker_file_map_.find(linker_handle);
	if(iter != linker_file_map_.end())
	{
		linker_file_map_.erase(iter);
	}
}


