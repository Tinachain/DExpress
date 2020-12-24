// server_windows_demo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "interface.h"

#if defined(_WIN32)

	#include <windows.h>
	#include <Mmsystem.h>

	#define lib_load(a) LoadLibrary(a)
	#define lib_handle HINSTANCE
	#define lib_error() GetLastError()
	#define lib_function(a, b) GetProcAddress(a, b)
	#define lib_close(a) FreeLibrary(a)
#else

	#include <dlfcn.h>
	#define lib_load(a) dlopen(a, RTLD_LAZY)
	#define lib_handle void*
	#define lib_error() dlerror()
	#define lib_function(a, b) dlsym(a, b)
	#define lib_close(a) dlclose(a)
#endif

#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <memory>
#include <vector>
#include <time.h>

/*�ӿں�������*/
typedef int(*OPEN_CLIENT)(char* bind_ip,
	char* remote_ip, 
	int remote_port,
	char* log,
	char *harq_so_path,
	char* session,
	bool encrypted,
	ON_EXPRESS_LOGIN on_login, 
	ON_EXPRESS_PROGRESS on_progress,
	ON_EXPRESS_FINISH on_finish,
	ON_EXPRESS_DISCONNECT on_disconnect,
	ON_EXPRESS_ERROR on_error);

typedef bool(*SEND_FILE)(int express_handle, void *param, char* file_path, char* save_relative_path);
typedef bool(*SEND_DIR)(int express_handle, void *param, char* dir_path, char* save_relative_path);
typedef void(*STOP_SEND)(int express_handle, void *param, char* file_path);
typedef void(*CLOSE_CLIENT)(int express_handle);

typedef char* (*VERSION)();

/*�ص�����ʵ��*/
void on_login(char* remote_ip, int remote_port, char* session)
{
	return;
}

void on_finish(void *param, char* file_path, long long size)
{
	printf("on_finish file_path=%s size=%lld\n", file_path, size);
}

bool on_progress(void *param, char* file_path, int max, int cur)
{
	//printf("on_progress file_path=%s max=%d cur=%d\n", file_path, max, cur);
	return true;
}

void on_disconnect(char* remote_ip, int remote_port)
{
	return;
}

void on_error(int error, char* remote_ip, int remote_port)
{
	return;
}

int main()
{
	//�ӿڶ���;
	lib_handle lib_handle_ = nullptr;
	OPEN_CLIENT open_client_ptr = nullptr;
	SEND_FILE send_file_ptr = nullptr;
	SEND_DIR send_dir_ptr = nullptr;
	CLOSE_CLIENT close_client_ptr = nullptr;
	VERSION version_ptr = nullptr;

	//����dll
	std::string current_path = "D:/projects/Chainware/���ղ�Ʒ/windows/lib/client/client.dll";
	lib_handle_ = lib_load(current_path.c_str());
	if (nullptr == lib_handle_)
	{
		printf("lib_load error\n");
		return -1;
	}

	//����so͸���ĺ���;
	open_client_ptr = (OPEN_CLIENT)lib_function(lib_handle_, "open_client");
	if (nullptr == open_client_ptr)
	{
		printf("lib_function OPEN_CLIENT error\n");
		lib_close(lib_handle_);
		return -1;
	}

	send_file_ptr = (SEND_FILE)lib_function(lib_handle_, "send_file");
	if (nullptr == send_file_ptr)
	{
		printf("lib_function SEND_FILE error\n");
		lib_close(lib_handle_);
		return -1;
	}

	send_dir_ptr = (SEND_DIR)lib_function(lib_handle_, "send_dir");
	if (nullptr == send_dir_ptr)
	{
		printf("lib_function SEND_DIR error\n");
		lib_close(lib_handle_);
		return -1;
	}

	close_client_ptr = (CLOSE_CLIENT)lib_function(lib_handle_, "close_client");
	if (nullptr == close_client_ptr)
	{
		printf("lib_function CLOSE_CLIENT error\n");
		lib_close(lib_handle_);
		return -1;
	}

	version_ptr = (VERSION)lib_function(lib_handle_, "version");
	if (nullptr == version_ptr)
	{
		printf("lib_function VERSION error\n");
		lib_close(lib_handle_);
		return -1;
	}

	std::vector<std::string> remote_array;
	remote_array.push_back("127.0.0.1");
	remote_array.push_back("10.10.50.251");
	//remote_array.push_back("47.110.15.241");

	std::string log = "log";
	std::string session = "123456";

	std::vector<std::string> send_file_array;
	send_file_array.push_back("D:/Test/Test1.mp4");
	send_file_array.push_back("E:/tools/liteide-master.zip");
	send_file_array.push_back("E:/tools/WinMerge_opdown.com.zip");
	send_file_array.push_back("E:/tools/liteidex37.3.linux64-qt4.8.7.tar.gz");
	send_file_array.push_back("E:/tools/go1.15.4.linux-amd64.tar.gz");

	std::vector<std::string> send_file_array2;
	send_file_array2.push_back("E:/tools/go1.14.4.linux-amd64.tar.gz");
	send_file_array2.push_back("E:/tools/qt-opensource-linux-x64-5.14.0.run");
	send_file_array2.push_back("E:/tools/livego.tar.gz");
	send_file_array2.push_back("E:/tools/qtxmlpatterns-everywhere-src-5.15.0.zip");
	send_file_array2.push_back("E:/tools/vs_Community_2017.exe");

	std::vector<int> remote_handle_array;

	//ѭ���������Ӷ���;
	for (auto iter = remote_array.begin(); iter != remote_array.end(); iter++)
	{
		std::string remote_ip_string = *iter;

		//���ӷ����
		std::string libharqpath = "D:/projects/Chainware/���ղ�Ʒ/windows/lib/harq/harq.dll";
		int express_handle = open_client_ptr("0.0.0.0", (char*)remote_ip_string.c_str(), 41002, (char*)log.c_str(), (char*)libharqpath.c_str(), (char*)session.c_str(), true, &on_login, &on_progress, &on_finish, &on_disconnect, &on_error);
		if (express_handle <= 0)
		{
			printf("open_client_ptr error remote_ip=%s\n", remote_ip_string.c_str());
			lib_close(lib_handle_);
			return -1;
		}
		remote_handle_array.push_back(express_handle);
	}
	::Sleep(1000);

	time_t last_send_time = time(nullptr);
	int postion = 0;
	while (1)
	{
		time_t current_timer = time(nullptr);
		int second = static_cast<int>(difftime(current_timer, last_send_time));
		if (second >= 1)
		{
			//ѭ�������ļ�����Ӧ�ķ�����
			int handle_postion = 0;
			for (auto iter = remote_handle_array.begin(); iter != remote_handle_array.end(); iter++)
			{
				handle_postion++;
				int handle = *iter;

				//�Զ�Ŀ�ĵ�;
				std::string save_relative_path = "20201219/debug";
				std::string file_path = "";
				if (handle_postion == 1)
				{
					file_path = send_file_array[postion];
				}
				else if (handle_postion == 2)
				{
					file_path = send_file_array2[postion];
				}

				//�����ļ�;
				bool sended = send_file_ptr(handle, nullptr, (char*)file_path.c_str(), (char*)save_relative_path.c_str());

				//������־
				printf("send_file_ptr handle=%d file=%s\n", handle, file_path.c_str());			
			}
			postion++;
			if (postion >= send_file_array.size())
				break;
			else
				last_send_time = time(nullptr);
		}
	}

	int checkPostion = 0;
	while (1)
	{
		checkPostion++;
		//printf("postion=%d\n", checkPostion);
		::Sleep(1000);
	}
	return 0;

}

