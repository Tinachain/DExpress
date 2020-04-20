
#include "main_thread.h"
#include "path.h"
#include <time.h>
#include <fstream>
#include <string.h>
#include <algorithm>

#include <iostream> // std::cout
#include <sstream> // std::stringstream

#include "file_protocol.h"
#include "ini.h"
#include "write_log.h"
#include "udp_manager.h"
#include "web_protocol_manager.h"
#include "json.h"

#include "string_list.h"
#include "fcntl.h"
#include "files.h"

using namespace std;

main_thread::main_thread()
{
	rudp_disconnected_timer_ = time(nullptr);
	last_show_timer_ = time(nullptr);
	check_file_timer_ = time(nullptr);

	load_json();

	write_log_ptr_ = new ustd::log::write_log();
	write_log_ptr_->init("RUDP File Client Manager", json_config_.log_path_.c_str(), 1);

	file_thread_ = new ustd::files_thread();
	file_thread_->init(json_config_.hls_.max_files_, json_config_.hls_.delete_files_);

	echo_file_thread_ = new ustd::files_thread();
	echo_file_thread_->init(json_config_.hls_.max_files_, json_config_.hls_.delete_files_);
}

void main_thread::init()
{
	add_log(LOG_TYPE_INFO, "Create Web Manager");
	web_protocol_ = new web_protocol();
	web_protocol_->init(json_config_.web_port_);
	web_protocol_->on_add_queue_ = std::bind(&main_thread::add_web_queue, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	web_protocol_->on_check_message_ = std::bind(&main_thread::add_on_log, this, std::placeholders::_1, std::placeholders::_2);

	add_log(LOG_TYPE_INFO, "Create Main Thread");
	thread_ptr_ = std::thread(&main_thread::execute, this);
	thread_ptr_.detach();
}

void main_thread::init_json()
{
	json_config_.web_port_ = 8081;
	json_config_.log_path_ = "log";
	json_config_.tested_ = false;

	//rudp的配置;
	json_config_.rudp_config_.ip_ = "127.0.0.1";
	json_config_.rudp_config_.port_ = 9999;
	json_config_.rudp_config_.start_ = false;
	json_config_.rudp_config_.delay_ = false;
	json_config_.rudp_config_.delay_interval_ = 2000;
	json_config_.rudp_config_.reconnect_ = 30;

	json_config_.protocol_ = 0;

	//HLS协议;
	json_config_.hls_.base_ = "/home/fxh7622/channellist";
	json_config_.hls_.concurrency_ = 10;
	json_config_.hls_.max_files_ = 200;
	json_config_.hls_.delete_files_ = 100;
	json_config_.hls_.resend_interval_ = 30;

	//组播协议;
	json_config_.multicast_.group_ip_ = "";
	json_config_.multicast_.group_port_ = 0;
}

void main_thread::load_json()
{
	init_json();

	//
	std::string json_path = ustd::path::get_app_path() + "/config.json";
	if (!ustd::path::is_file_exist(json_path))
	{
		add_log(LOG_TYPE_ERROR, "Json Config File Not Found path=%s", json_path.c_str());
		return;
	}
	printf("path=%s", json_path.c_str());

	//
	long long file_size = ustd::path::get_file_size(json_path);
	int readhandle = open(json_path.c_str(), O_RDONLY);
	if(-1 == readhandle)
	{
		add_log(LOG_TYPE_ERROR, "Open File Failed path=%s", json_path.c_str());
		return;
	}

	char *file_buffer = new char[file_size];
	memset(file_buffer, 0, file_size);
	int read_len = read(readhandle, file_buffer, file_size);
	if(read_len != file_size)
	{
		add_log(LOG_TYPE_ERROR, "Read File Failed path=%s", json_path.c_str());
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
		add_log(LOG_TYPE_ERROR, "Parse Config File Failed file_buffer=%s error %s\n", file_buffer, cJSON_GetErrorPtr());
		return;
	}

	json_config_.tested_ = ustd::json::get_value_bool(root, "test");

	std::vector<std::string> tmp_vector;
	tmp_vector.push_back("web");
	json_config_.web_port_ = ustd::json::get_value_int(root, tmp_vector, "port");

	tmp_vector.clear();
	tmp_vector.push_back("log");
	json_config_.log_path_ = ustd::json::get_value_string(root, tmp_vector, "path");

	tmp_vector.clear();
	tmp_vector.push_back("rudp");
	json_config_.rudp_config_.ip_ = ustd::json::get_value_string(root, tmp_vector, "ip");
	json_config_.rudp_config_.port_ = ustd::json::get_value_int(root, tmp_vector, "port");
	json_config_.rudp_config_.start_ = ustd::json::get_value_bool(root, tmp_vector, "start");
	json_config_.rudp_config_.delay_ = ustd::json::get_value_bool(root, tmp_vector, "delay");
	json_config_.rudp_config_.delay_interval_ = ustd::json::get_value_int(root, tmp_vector, "delay_interval");
	json_config_.rudp_config_.reconnect_ = ustd::json::get_value_int(root, tmp_vector, "reconnect");

	tmp_vector.clear();
	tmp_vector.push_back("hls");
	json_config_.hls_.concurrency_ = ustd::json::get_value_int(root, tmp_vector, "concurrency");
	json_config_.hls_.resend_interval_ = ustd::json::get_value_int(root, tmp_vector, "resend_interval");

	tmp_vector.clear();
	tmp_vector.push_back("hls");
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

main_thread::~main_thread(void)
{
	current_state_ = tst_stoping;
	if(nullptr != web_protocol_)
	{
		delete web_protocol_;
		web_protocol_ = nullptr;
	}
	free_queue();
}

void main_thread::add_queue(header_protocol head_ptr, char *data, int size, int linker_handle)
{
	server_message_buffer *buffer_ptr = new server_message_buffer();
	buffer_ptr->message_id_ = head_ptr.protocol_id_;
	buffer_ptr->linker_handle_ = linker_handle;
	buffer_ptr->next_ = nullptr;
	buffer_ptr->size_ = size;
	buffer_ptr->data_ = nullptr;

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

void main_thread::add_web_queue(std::string message_type, std::string params, struct evhttp_request *request)
{
	web_message_buffer *web_buffer_ptr = new web_message_buffer();

	web_buffer_ptr->message_type_ = message_type;
	web_buffer_ptr->params_ = params;
	web_buffer_ptr->request_ = request;
	web_buffer_ptr->next_ = nullptr;

	{
		std::lock_guard<std::recursive_mutex> gurad(web_messasge_lock_);
		if(web_first_ != nullptr)
			web_last_->next_ = web_buffer_ptr;
		else
			web_first_ = web_buffer_ptr;

		web_last_ = web_buffer_ptr;
	}
}

void main_thread::free_web_queue()
{
	web_message_buffer *next_ptr = nullptr;
	while(web_first_ != nullptr)
	{
		next_ptr = web_first_->next_;
		delete web_first_;
		web_first_ = next_ptr;
	}
	web_first_ = nullptr;
	web_last_ = nullptr;
}

void main_thread::simulation_hls_message()
{
	std::string test_string = "{\"protocol\": 0,\"hls\": {\"path\": \"/mnt/hgfs/projects/Cross_Live/file_express/express_client/file/live.zip\",\"extra\": \"file\",\"rename\": \"live.zip\"}}";
	add_web_queue("POST", test_string, nullptr);
}

void main_thread::send_multicast_config(const std::string &group_ip, const int &group_port)
{
	char* json_buffer = nullptr;
	std::vector<std::string> tmp_vector;
	cJSON * root =  cJSON_CreateObject();
	ustd::json::set_value_string(root, tmp_vector, "group_ip", group_ip);
	ustd::json::set_value_int(root, tmp_vector, "group_port", group_port);
	json_buffer = cJSON_Print(root);
	std::string json_string(json_buffer);
	send_buffer_to_server(MULTICAST_CONFIG, (char*)json_string.c_str(), strlen(json_string.c_str()));
	free(json_buffer);
	cJSON_Delete(root);
}

void main_thread::execute()
{
	init_file_request_state();

	//开启RUDP;
	if (json_config_.rudp_config_.start_)
	{
		add_log(LOG_TYPE_INFO, "UDP Start Config Is True Create RUDP Manager");
		udp_manager_ = new udp_manager(json_config_.rudp_config_.ip_, json_config_.rudp_config_.port_, json_config_.rudp_config_.delay_, json_config_.rudp_config_.delay_interval_);
		udp_manager_->init();
		linker_handle_ = udp_manager_->connect();
		if (ERROR_HANDLE != linker_handle_)
		{
			rudp_disconnected_ = false;
			add_log(LOG_TYPE_INFO, "UDP Start Success");
		}
		else
		{
			rudp_disconnected_ = true;
			add_log(LOG_TYPE_ERROR, "UDP Start Failed");
		}
	}
	else
	{
		add_log(LOG_TYPE_INFO, "UDP Start Config Is False Not Create RUDP Manager");
	}

	if (!rudp_disconnected_ && 1 == json_config_.protocol_)
	{
		add_log(LOG_TYPE_INFO, "Start Multicast Group IP %s Group Port %d", json_config_.multicast_.group_ip_.c_str(), json_config_.multicast_.group_port_);
		multicast_manager_ = new multicast_manager(json_config_.multicast_.group_ip_, json_config_.multicast_.group_port_);
		multicast_manager_->log_ptr_ = write_log_ptr_;
		if(multicast_manager_->init())
		{
			add_log(LOG_TYPE_INFO, "Create Multicast Success Group IP %s Group Port %d", json_config_.multicast_.group_ip_.c_str(), json_config_.multicast_.group_port_);
			send_multicast_config(json_config_.multicast_.group_ip_, json_config_.multicast_.group_port_);
		}
	}

	current_state_ = tst_runing;

	while(tst_runing == current_state_)
	{
		business_dispense();
		web_business_dispense();
		if (!rudp_disconnected_)
		{
			check_file_timer();
			check_file_request();
		}
		check_file_handle();
		check_rudp_disconnect();
		if (json_config_.tested_)
		{
			if (linker_handle_ != ERROR_HANDLE)
			{
				check_simulation();
			}
		}
		ustd::rudp_public::sleep_delay(50);
	}
	current_state_ = tst_stoped;
}

void main_thread::check_file_request()
{
	while(1)
	{
		if (get_sending_file_count() >= json_config_.hls_.concurrency_)
			return;

		std::shared_ptr<file_request> file_ptr = get_init_file();
		if (file_ptr == nullptr)
			return;

		if(read_file(file_ptr->file_path_, file_ptr->file_extra_, file_ptr->rename_))
		{
			file_ptr->state_ = trs_sending;
			file_ptr->send_timer_ = time(nullptr);
			return;
		}
		else
		{
			add_log(LOG_TYPE_ERROR, "Read File Failed %s Delete File From Request", file_ptr->file_path_.c_str());
			delete_file_request(file_ptr->file_path_);
		}
	}
}

void main_thread::web_post_message(web_message_buffer *message)
{
	cJSON *root = cJSON_Parse(message->params_.c_str());
	if(nullptr == root)
	{
		add_log(LOG_TYPE_ERROR, "Json Parse Failed %s", message->params_.c_str());
		return;
	}

	std::vector<std::string> tmp_vector;
	int protocol = ustd::json::get_value_int(root, tmp_vector, "protocol");

	if(0 == protocol)
	{
		tmp_vector.clear();
		tmp_vector.push_back("hls");
		std::string file_path = ustd::json::get_value_string(root, tmp_vector, "path");
		std::string extra_path = ustd::json::get_value_string(root, tmp_vector, "extra");
		std::string file_rename = ustd::json::get_value_string(root, tmp_vector, "rename");
		if((!ustd::path::is_file_exist(file_path)) || (static_cast<int>(ustd::path::get_file_size(file_path)) <= 0))
		{
			add_log(LOG_TYPE_ERROR, "POST HLS Protocol Not Found File Or File Size Equal is Zero %s", file_path.c_str());
			cJSON_Delete(root);
			return;
		}
		add_file_request(file_path, extra_path, file_rename);
	}
	else
	{
		add_log(LOG_TYPE_ERROR, "Not Know Protocol protocol=%d", protocol);
	}
	cJSON_Delete(root);
}

void main_thread::web_business_dispense()
{
	web_message_buffer *work_ptr = nullptr, *next_ptr = nullptr;
	{
		std::lock_guard<std::recursive_mutex> gurad(web_messasge_lock_);
		if(work_ptr == nullptr && web_first_ != nullptr)
		{
			work_ptr = web_first_;
			web_first_ = nullptr;
			web_last_ = nullptr;
		}
	}

	while(work_ptr != nullptr)
	{
		next_ptr = work_ptr->next_;

		if ("POST" == work_ptr->message_type_)
		{
			web_post_message(work_ptr);
		}
		else
		{
			add_log(LOG_TYPE_ERROR, "web_dispense Not Found Massage Type %s", work_ptr->message_type_.c_str());
			break;
		}

		delete work_ptr;
		work_ptr = next_ptr;
	}
}

void main_thread::show_static()
{
	time_t curr_time = time(nullptr);
	int second = static_cast<int>(difftime(curr_time, last_show_timer_));
	if(second >= SHOW_TIME)
	{
		int postion = 0;
		int speed = get_speed();
		speed = speed / second;

		std::lock_guard<std::recursive_mutex> guard(file_lock_);

		add_log(LOG_TYPE_INFO, "Files=%d Check Speed Is %s/Second", static_cast<int>(file_thread_->get_files_size()), ustd::string_list::flow_show(speed).c_str());
		for(auto iter = file_request_list_.begin(); iter != file_request_list_.end(); iter++)
		{
			std::shared_ptr<file_request> file_ptr = *iter;
			if (trs_sending == file_ptr->state_)
			{
				add_log(LOG_TYPE_INFO, "Wait Send File Count %d Sending File Count %d Path %s", file_request_list_.size(), postion, file_ptr->file_path_.c_str());
				postion++;
			}
		}
		last_show_timer_ = time(nullptr);
	}
}

void main_thread::check_simulation()
{
	time_t curr_time = time(nullptr);
	int second = static_cast<int>(difftime(curr_time, last_simulation_timer_));
	if(second >= 20)
	{
		simulation_hls_message();
		last_simulation_timer_ = time(nullptr);
	}
}

void main_thread::check_rudp_disconnect()
{
	time_t curr_time = time(nullptr);
	int second = static_cast<int>(difftime(curr_time, rudp_disconnected_timer_));
	if(second >= json_config_.rudp_config_.reconnect_)
	{
		add_log(LOG_TYPE_INFO, "Check RUDP Manager Connect");
		if (udp_manager_ != nullptr)
		{
			if (rudp_disconnected_)
			{
				linker_handle_ = udp_manager_->connect();
				if (ERROR_HANDLE != linker_handle_)
				{
					rudp_disconnected_ = false;
					init_file_request_state();
					add_log(LOG_TYPE_INFO, "UDP Start Success");
				}
				else
				{
					rudp_disconnected_ = true;
					add_log(LOG_TYPE_ERROR, "UDP Start Failed");
				}
			}
		}
		rudp_disconnected_timer_ = time(nullptr);
	}
}

void main_thread::rudp_offline(server_message_buffer *message)
{
	if (!rudp_disconnected_)
	{
		rudp_disconnected_ = true;
		//这里要清空发送队列，避免出现正在发送的数据无法返回，而队列无法清空的清空;
		init_file_request_state();
		rudp_disconnected_timer_ = time(nullptr);
	}
}

bool main_thread::read_file(const std::string &file_path, const std::string &extra_path, const std::string &file_name)
{
	int size = static_cast<int>(ustd::path::get_file_size(file_path));
	int maxId = (size + SINGLE_BLOCK - 1) / SINGLE_BLOCK;

	//封装Json格式
	cJSON * root =  cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "protocol", 0);

	cJSON *hls = cJSON_CreateObject();
	cJSON_AddStringToObject(hls, "path", file_path.c_str());
	cJSON_AddStringToObject(hls, "extra", extra_path.c_str());
	cJSON_AddStringToObject(hls, "rename", file_name.c_str());
	cJSON_AddNumberToObject(hls, "maxId", maxId);
	cJSON_AddItemToObject(root, "hls", hls);
	char* json_buffer = cJSON_Print(root);

	std::string json_string(json_buffer);
	send_buffer_to_server(HLS_CONFIG, (char*)json_string.c_str(), strlen(json_string.c_str()));
	free(json_buffer);
	cJSON_Delete(root);

	//发送文件数据
	int file_handle_ = open(file_path.c_str(), O_RDONLY);
	if (-1 == file_handle_)
	{
		add_log(LOG_TYPE_ERROR, "Open Function Failed error=%d", errno);
		return false;
	}

	char buffer[SINGLE_BLOCK] = {0};
	int postion = 0;
	int sended_size = 0;
	while(true)
	{
		postion++;
		memset(buffer, 0, SINGLE_BLOCK);
		ssize_t len = read(file_handle_, buffer, SINGLE_BLOCK);
		if (0 == len)
		{
			if (0 == sended_size)
			{
				add_log(LOG_TYPE_ERROR, "Read File Result Zero sended_size=%d", sended_size);
			}
			break;
		}
		else if (len < 0)
		{
			//读取失败, 已经读取完毕;
			add_log(LOG_TYPE_ERROR, "Read File Failed file_handle_=%d error=%d", file_handle_, errno);
			break;
		}
		else if (len > 0 && len < SINGLE_BLOCK)
		{
			//最后一次;
			send_buffer_to_server(HLS_FILE_BUFFER, postion, buffer, len);
			sended_size = sended_size + len;

			break;
		}
		else if (len == SINGLE_BLOCK)
		{
			send_buffer_to_server(HLS_FILE_BUFFER, postion, buffer, len);
			sended_size = sended_size + len;
		}
	}
	close(file_handle_);
	return true;
}

void main_thread::check_file_handle()
{
	time_t curr_time = time(nullptr);
	int second = static_cast<int>(difftime(curr_time, last_check_timer_));
	if(second >= FILE_TIMEOUT)
	{
		if (ERROR_HANDLE != file_handle_)
		{
			close(file_handle_);
		}
		last_check_timer_ = time(nullptr);
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
			case HLS_FILE_BUFFER_ENDED:	//返回文件接收完毕;
			{
				hls_sended(work_ptr);
				break;
			}
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
			case CLOSE_LINKER:				//断开链接;
			{				
				rudp_offline(work_ptr);
				break;
			}
			default:
			{
				add_log(LOG_TYPE_ERROR, "Not Found MessageID %d", work_ptr->message_id_);
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

//配置信息
void main_thread::hls_config(server_message_buffer *message_buffer)
{
	std::string json_string(message_buffer->data_, message_buffer->size_);

	cJSON *root = cJSON_Parse(json_string.c_str());
	if(nullptr == root)
	{
		add_log(LOG_TYPE_ERROR, "Json Parse Failed %s", json_string.c_str());
		return;
	}

	std::vector<std::string> tmp_vector;
	int protocol = ustd::json::get_value_int(root, tmp_vector, "protocol");

	if(0 == protocol)
	{
		tmp_vector.clear();
		tmp_vector.push_back("hls");
		remote_path_ = ustd::json::get_value_string(root, tmp_vector, "path");
		extra_path_ = ustd::json::get_value_string(root, tmp_vector, "extra");
		file_rename_ = ustd::json::get_value_string(root, tmp_vector, "rename");
		max_id_ = ustd::json::get_value_int(root, tmp_vector, "maxId");
		std::string cur_path = get_local_path(extra_path_, file_rename_);
		if (!ustd::path::create_directory(cur_path.c_str()))
		{
			add_log(LOG_TYPE_ERROR, "Create Dir Failed cur_path=%s", cur_path.c_str());
			cJSON_Delete(root);
			return;
		}

		std::string cur_file_path = get_local_file(extra_path_, file_rename_);
		if (ustd::path::is_file_exist(cur_file_path))
		{
			if (!ustd::path::remove_file(cur_file_path))
			{
				add_log(LOG_TYPE_ERROR, "File Already Exit name=%s cur_file_path=%s", file_name_.c_str(), cur_file_path.c_str());
				cJSON_Delete(root);
				return;
			}
		}

		if (file_handle_ != ERROR_HANDLE)
		{
			close(file_handle_);
			file_handle_ = ERROR_HANDLE;
		}
		file_handle_ = open(cur_file_path.c_str(), O_CREAT|O_RDWR , S_IRUSR | S_IWUSR);
		if (ERROR_HANDLE == file_handle_)
		{
			add_log(LOG_TYPE_ERROR, "Open Function Failed path=%s errno=%d", cur_file_path.c_str(), errno);
			cJSON_Delete(root);
			return;
		}
		file_path_ = cur_file_path;
		config_string_ = json_string;
		time(&last_check_timer_);
		time(&begin_timer_);
	}
	else
	{
		add_log(LOG_TYPE_ERROR, "Not Know Protocol protocol=%d", protocol);
	}
	cJSON_Delete(root);
}

void main_thread::hls_sended(server_message_buffer *message)
{
	//将数据转换成string类型
	std::string json_string(message->data_, message->size_);

	cJSON *root = cJSON_Parse(json_string.c_str());
	if(nullptr == root)
	{
		add_log(LOG_TYPE_ERROR, "Json Parse Failed %s", json_string.c_str());
		return;
	}

	std::vector<std::string> tmp_vector;
	int protocol = ustd::json::get_value_int(root, tmp_vector, "protocol");
	if(0 == protocol)
	{
		tmp_vector.push_back("hls");
		std::string remote_path = ustd::json::get_value_string(root, tmp_vector, "path");
		std::string rename = ustd::json::get_value_string(root, tmp_vector, "extra");

		add_log(LOG_TYPE_INFO, "<<<<Return To Sended Complete File %s", remote_path.c_str());
		delete_file_request(remote_path);

		//由于M3U8不进行删除，因此这里需要判断文件结尾是否是m3u8
		std::string ext = ustd::path::get_file_ext(rename);
		transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		if (ext != "m3u8")
		{
			echo_file_thread_->add_queue(remote_path);
		}
	}
	else
	{
		add_log(LOG_TYPE_ERROR, "Not Know Protocol protocol=%d", protocol);
	}
}

int main_thread::send_buffer_to_server(char *data, const int &size)
{
	std::lock_guard<std::recursive_mutex> guard(send_lock_);

	udp_manager_->rudp_manager_ptr_->send_buffer(data, size, linker_handle_);
	return 0;
}

int main_thread::send_buffer_to_server(const unsigned short &message_id, char *data, const int &size)
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

	udp_manager_->rudp_manager_ptr_->send_buffer(buffer_ptr, buffer_len, linker_handle_);

	delete[] buffer_ptr;
	return size;
}

int main_thread::send_buffer_to_server(const unsigned short &message_id, const int &cur_id, char *data, const int &size)
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

	udp_manager_->rudp_manager_ptr_->send_buffer(buffer_ptr, buffer_len, linker_handle_);

	delete[] buffer_ptr;
	return size;
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
	char log_text[array_length];
	memset(log_text, 0x00, array_length);

	va_list arg_ptr;
	va_start(arg_ptr, log_text_format);
	int result = vsprintf(log_text, log_text_format, arg_ptr);
	va_end(arg_ptr);
	if (result <= 0)
		return;

	if (result > array_length)
		return;

	if(write_log_ptr_ != nullptr)
	{
		write_log_ptr_->write_log3(log_type, log_text);
	}
}

void main_thread::add_file_request(const std::string &file_path, const std::string &file_extra, std::string &rename)
{
	std::lock_guard<std::recursive_mutex> guard(file_lock_);

	std::shared_ptr<file_request> file_ptr(new file_request);
	file_ptr->file_path_ = file_path;
	file_ptr->file_extra_ = file_extra;
	file_ptr->rename_ = rename;
	file_ptr->last_timer_ = time(nullptr);
	file_ptr->state_ = trs_init;
	file_request_list_.push_back(file_ptr);

	//判断是否达到最大
	if (json_config_.hls_.max_files_ < static_cast<int>(file_request_list_.size()))
	{
		int postion = 0;
		while(postion < json_config_.hls_.delete_files_)
		{
			for(auto iter = file_request_list_.begin(); iter != file_request_list_.end(); )
			{
				std::shared_ptr<file_request> file_ptr = *iter;
				if (file_ptr->state_ == trs_init)
				{
					iter = file_request_list_.erase(iter);
					break;
				}
				else
				{
					iter++;
				}
			}
			postion++;
		}
	}

	int size = static_cast<int>(ustd::path::get_file_size(file_path));
	add_log(LOG_TYPE_INFO, "↓↓↓↓Insert Request Send File %s File Size %d File List Size %d ", file_path.c_str(), size, static_cast<int>(file_request_list_.size()));
}

std::shared_ptr<file_request> main_thread::find_file_request(const std::string &file_path)
{
	std::lock_guard<std::recursive_mutex> guard(file_lock_);

	for(auto iter = file_request_list_.begin(); iter != file_request_list_.end(); iter++)
	{
		std::shared_ptr<file_request> file_ptr = *iter;
		if (file_ptr->file_path_ == file_path)
		{
			return file_ptr;
		}
	}
	return nullptr;
}

std::shared_ptr<file_request> main_thread::get_init_file()
{
	std::lock_guard<std::recursive_mutex> guard(file_lock_);

	for(auto iter = file_request_list_.begin(); iter != file_request_list_.end(); iter++)
	{
		std::shared_ptr<file_request> file_ptr = *iter;
		if (trs_init == file_ptr->state_)
		{
			if (ustd::path::is_file_exist(file_ptr->file_path_))
			{
				return file_ptr;
			}
		}
	}
	return nullptr;
}

void main_thread::check_file_timer()
{
	time_t curr_time = time(nullptr);
	int interval = static_cast<int>(difftime(curr_time, check_file_timer_));
	if(interval >= SHOW_TIME)
	{
		std::lock_guard<std::recursive_mutex> guard(file_lock_);

		add_log(LOG_TYPE_INFO, "Check File Timeout");
		for(auto iter = file_request_list_.begin(); iter != file_request_list_.end(); iter++)
		{
			std::shared_ptr<file_request> file_ptr = *iter;
			if (file_ptr->state_ == trs_sending)
			{

				int second = static_cast<int>(difftime(curr_time, file_ptr->last_timer_));
				if (second >= json_config_.hls_.resend_interval_)
				{
					add_log(LOG_TYPE_INFO, "Check File Timeout Set File %s", file_ptr->file_path_.c_str());
					file_ptr->state_ = trs_init;
					file_ptr->last_timer_ = time(nullptr);
				}
			}
		}
		check_file_timer_ = time(nullptr);
	}
}

void main_thread::delete_file_request(const std::string &file_path)
{
	std::lock_guard<std::recursive_mutex> guard(file_lock_);

	for(auto iter = file_request_list_.begin(); iter != file_request_list_.end(); iter++)
	{
		std::shared_ptr<file_request> file_ptr = *iter;
		if (file_ptr->file_path_ == file_path)
		{
			iter = file_request_list_.erase(iter);
			return;
		}
	}
}

int main_thread::get_sending_file_count()
{
	std::lock_guard<std::recursive_mutex> guard(file_lock_);

	int count = 0;
	for(auto iter = file_request_list_.begin(); iter != file_request_list_.end(); iter++)
	{
		std::shared_ptr<file_request> file_ptr = *iter;
		if (trs_sending == file_ptr->state_)
		{
			count++;
		}
	}
	return count;
}

void main_thread::init_file_request_state()
{
	std::lock_guard<std::recursive_mutex> guard(file_lock_);

	for(auto iter = file_request_list_.begin(); iter != file_request_list_.end(); iter++)
	{
		std::shared_ptr<file_request> file_ptr = *iter;
		file_ptr->state_ = trs_init;
	}
}

void main_thread::free_file_request()
{
	std::lock_guard<std::recursive_mutex> guard(file_lock_);

	file_request_list_.clear();
}

void main_thread::add_speed(const std::string &file_path, const int &size)
{
	std::lock_guard<std::recursive_mutex> guard(speed_lock_);
	if (size >= 0)
	{
		std::shared_ptr<file_speed> file_ptr(new file_speed);
		file_ptr->file_path_ = file_path;
		file_ptr->file_size_ = size;
		speed_list_.push_back(file_ptr);
	}
}

int main_thread::get_speed()
{
	std::lock_guard<std::recursive_mutex> guard(speed_lock_);
	int speed = 0;
	for (auto iter = speed_list_.begin(); iter != speed_list_.end(); iter++)
	{
		std::shared_ptr<file_speed> file_ptr = *iter;
		speed = speed + file_ptr->file_size_;
	}
	speed_list_.clear();
	return speed;
}

void main_thread::hls_buffer(server_message_buffer *message)
{
	file_protocol *file_ptr = (file_protocol *)message->data_;
	bool ret = write_file(message->data_ + sizeof(file_protocol), message->size_ - sizeof(file_protocol), file_ptr->cur_id_);
	if (ret)
	{
		close(file_handle_);

		time_t cur_timer = time(nullptr);
		int second = static_cast<int>(difftime(cur_timer, begin_timer_));
		add_log(LOG_TYPE_INFO, "<<<<RUDP File Recv Ended File Name=%s Time Len=%d Second", file_path_.c_str(), second);

		send_file_ended(message->linker_handle_);
		std::string ext = ustd::path::get_file_ext(file_path_);
		transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		if (ext != "m3u8")
		{
			echo_file_thread_->add_queue(file_path_);
		}
	}
}

void main_thread::send_file_ended(int linker_handle)
{
	header_protocol tmp_header_ptr;
	memset(&tmp_header_ptr, 0, sizeof(tmp_header_ptr));
	tmp_header_ptr.protocol_id_ = HLS_FILE_BUFFER_ENDED;

	int buffer_len = strlen(config_string_.c_str()) + sizeof(tmp_header_ptr);
	char *buffer_ptr = new char[buffer_len];
	memcpy(buffer_ptr, &tmp_header_ptr, sizeof(tmp_header_ptr));

	if (strlen(config_string_.c_str()) > 0)
		memcpy(buffer_ptr + sizeof(tmp_header_ptr), config_string_.c_str(), strlen(config_string_.c_str()));

	udp_manager_->rudp_manager_ptr_->send_buffer(buffer_ptr, buffer_len, linker_handle);
	delete[] buffer_ptr;
	return;
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

bool main_thread::write_file(char *data, int size, int cur_id)
{
	if (ERROR_HANDLE == file_handle_ )
	{
		add_log(LOG_TYPE_ERROR, "File Not Open FileHandle is -1");
		return false;
	}

	//移动
	lseek(file_handle_, 0, SEEK_END);

	//写入文件
	ssize_t ret = write(file_handle_, data, size);
	if (-1 == ret)
	{
		add_log(LOG_TYPE_ERROR, "Write File Function Failed errno=%d", errno);
		close(file_handle_);
		return false;
	}

	if (cur_id == max_id_)
	{
		return true;
	}
	return false;
}


