#pragma once

#include <string>
#include <map>
#include <vector>

#pragma pack(push, 1)

namespace ustd
{

namespace http_parser
{

typedef std::map<std::string, std::string> request_param_map;

#define CRLF "\x0d\x0a"

int request_param_parse(const std::string &request_text, request_param_map &request_param);

enum REQUEST_TYPE
{
	ERROR_ = -1,
	GET_ = 1,
	POST_
};

class http_request_data
{
public:
	http_request_data();
	~http_request_data();

public:
	void init();

public:
	REQUEST_TYPE request_type_;
	std::string referer_url_;
	size_t upload_data_size_;
	char *upload_data_ptr_;
	std::string forwarded_;
	std::string connection_;
};

class http_request
{
public:
	static bool request_parse(const char *data, const size_t &size, http_request_data &request_info);
	static bool request_get_parse(const char *data, const size_t &size, http_request_data &request_info);
	static bool request_post_parse(const char *data, const size_t &size, http_request_data &request_info);
	static REQUEST_TYPE get_request_type(const char *data, const size_t &size);
	static bool request_package_check(const char *data, const size_t &size, const REQUEST_TYPE &request_type);

private:
	static std::string get_header_item(const char *data, const size_t &size, const char *key, const bool &ignore_case = false);
	static size_t get_header_length(const char *data, const size_t &size);
	static std::string get_content_type(const char *data, const size_t &size);
	static std::string get_boundary_string(const char *data, const size_t &size);
	static std::string get_body_string(const char *data, const size_t &size);
	static void get_referer_info(const char *data, const size_t &size, const REQUEST_TYPE &request_type, std::string &url);
	static void get_connection_info(const char *data, const size_t &size, const REQUEST_TYPE &request_type, std::string &connection);
	static int get_formdata_stream(const char *data, const size_t &size, const char *boundary_info, http_request_data &request_info);
	static bool get_form_urlencode(const char *data, const size_t &size, http_request_data &request_info);
	static int get_form_multipart(const char *data, const size_t &size, const char *boundary_info, http_request_data &request_info);
	static int get_binary_stream(const char *data, const size_t &size, http_request_data &request_info);
	static int memfind(const char *src, const size_t &size_src, const char *dst, const size_t &size_dst = 0);
	static int memifind(const char *src, const size_t &size_src, const char *dst, const size_t &size_dst = 0);
};

class http_response_data
{
public:
	http_response_data();
	~http_response_data();

public:
	void init();

public:
	size_t size_;
	char *data_ptr_;
};


}
}

#pragma pack(pop)
