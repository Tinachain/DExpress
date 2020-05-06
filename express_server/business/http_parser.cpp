#include <memory>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>

#include "string_list.h"
#include "http_parser.h"

namespace ustd
{
namespace http_parser
{


std::string tolower_string(std::string low_string)
{
	std::string tmp_string = low_string;
	transform(tmp_string.begin(), tmp_string.end(), tmp_string.begin(), tolower);
	return tmp_string;
}

std::string utf8_to_url(const char *utf8)
{
	size_t idx = 0;
	std::string url_text = "";
	std::string utf8_text(utf8);
	char hex[] = "0123456789ABCDEF";

	size_t str_size = utf8_text.size();

	while (idx < str_size)
	{
		unsigned char ch = utf8_text[idx++];
		if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||	ch == '-' || ch == '_' || ch == '.')
		{
			url_text += ch;
		}
		else
		{
			url_text += "%";
			url_text += hex[ch / 16];
			url_text += hex[ch % 16];
		}
	}

	return url_text;
}

int request_param_parse(const std::string &request_text, request_param_map &request_param)
{
	int param_count = 0;
	request_param.clear();
	int idx = static_cast<int>(request_text.find("?"));
	if (idx > 0)
	{
		std::string request_t = request_text.substr(idx + 1);
		while ((idx = static_cast<int>(request_t.find("&"))) > 0)
		{
			std::string str = request_t.substr(0, idx);
			request_t = request_t.substr(idx + 1);
			if ((idx = static_cast<int>(str.find("="))) > 0)
			{
				std::string key = tolower_string(str.substr(0, idx));
				if (request_param.find(key) != request_param.end())
				{
					request_param.erase(key);
				}
				else
				{
					param_count += 1;
				}
				request_param.insert(request_param_map::value_type(key, str.substr(idx + 1)));
			}
		}

		if ((idx = static_cast<int>(request_t.find("="))) > 0)
		{
			std::string key = tolower_string(request_t.substr(0, idx));

			if (request_param.find(key) != request_param.end())
			{
				request_param.erase(key);
			}
			else
			{
				param_count += 1;
			}

			request_param.insert(request_param_map::value_type(key, request_t.substr(idx + 1)));
		}
	}

	return param_count;
}

http_request_data::http_request_data()
{
	this->request_type_ = ERROR_;
	this->referer_url_ = "";
	this->upload_data_ptr_ = NULL;
	this->upload_data_size_ = 0;
}

void http_request_data::init()
{
	this->request_type_ = ERROR_;
	this->referer_url_ = "";

	if (this->upload_data_ptr_ != NULL)
	{
		delete [] this->upload_data_ptr_;
		this->upload_data_ptr_ = NULL;
	}

	this->upload_data_size_ = 0;
}

http_request_data::~http_request_data()
{
	if (this->upload_data_ptr_ != NULL)
	{
		delete [] this->upload_data_ptr_;
		this->upload_data_ptr_ = NULL;
	}
}

bool http_request::request_parse(const char *data, const size_t &size, http_request_data &request_info)
{
	bool ret = false;

	request_info.init();
	switch (http_request::get_request_type(data, size))
	{
	case GET_:
		{
			ret = http_request::request_get_parse(data, size, request_info);
			break;
		}
		
	case POST_:
		{
			ret = http_request::request_post_parse(data, size, request_info);
			break;
		}

	default:
		{
			request_info.request_type_ = ERROR_;
			break;
		}
	}

	return ret;
}

std::string http_request::get_boundary_string(const char *data, const size_t &size)
{
	const char *key = "boundary=";
	int start_idx = http_request::memfind(data, size, key);
	if (start_idx <= 0)
	{
		return "";
	}

	start_idx += static_cast<int>(strlen(key));
	int len = http_request::memfind(data + start_idx, size - start_idx, CRLF);
	if (len <= 0)
	{
		return "";
	}
	return (std::string(data + start_idx, len));
}

std::string http_request::get_body_string(const char *data, const size_t &size)
{
	const char *key = "Content-Length:";
	int start_idx = http_request::memfind(data, size, key);
	if (start_idx <= 0)
	{
		return "";
	}
	start_idx += static_cast<int>(strlen(key));
	int len = http_request::memfind(data + start_idx, size - start_idx, CRLF);
	if (len <= 0)
	{
		return "";
	}
	return (std::string(data + start_idx, len));
}

bool http_request::request_package_check(const char *data, const size_t &size, const REQUEST_TYPE &request_type)
{
	if (request_type != GET_ && request_type != POST_)
	{
		return false;
	}

	size_t header_size = http_request::get_header_length(data, size);

	if (0 == header_size)
	{
		return false;
	}

	size_t content_size = 0;

	if (request_type == POST_)
	{
		content_size = strtoul(http_request::get_header_item(data, header_size, "Content-Length").c_str(), NULL, 10);
	}

	if (header_size + content_size > size)
	{
		return false;
	}

	return true;
}

size_t http_request::get_header_length(const char *data, const size_t &size)
{
	size_t header_size = 0;

	int idx = http_request::memfind(data, size, CRLF CRLF);

	if (idx > 0)
	{
		header_size = idx + strlen(CRLF CRLF);
	}

	return (header_size);
}

void http_request::get_connection_info(const char *data, const size_t &size, const REQUEST_TYPE &request_type, std::string &connection)
{
	connection.clear();
	std::string header(data, size);

	std::vector<std::string> line_list;
	ustd::string_list::string_split(header, "\r\n", line_list, true);

	for(size_t i = 0; i < line_list.size(); i++)
	{
		std::string tmp = line_list[i];
		if(line_list[i].find("Connection: ") != std::string::npos)
		{
			std::string tmp_connection = line_list[i].substr(strlen("Connection: "), line_list[i].length() - strlen("Connection: "));
			connection.assign(tmp_connection.c_str(), tmp_connection.length());
			return;
		}
	}
}

void http_request::get_referer_info(const char *data, const size_t &size, const REQUEST_TYPE &request_type, std::string &url)
{
	url.clear();
	size_t start_idx = 0;

	switch (request_type)
	{
	case GET_:
		{
			start_idx += strlen("GET ");
			break;
		}

	case POST_:
		{
			start_idx += strlen("POST ");
			break;
		}

	default:
		{
			break;
		}
	}

	if (start_idx <= 0)
	{
		return;
	}

	int len = http_request::memfind(data + start_idx, size - start_idx, " HTTP/1.");
	if (len < 0)
	{
		return;
	}

	url.assign(data + start_idx, len);
}

int http_request::get_formdata_stream(const char *data, const size_t &size, const char *boundary_info, http_request_data &request_info)
{
	char *key = "Content-Type: application/octet-stream" CRLF CRLF;
	int start_idx = http_request::memfind(data, size, key);
	if (start_idx < 0)
	{
		return -1;
	}
	start_idx += static_cast<int>(strlen(key));
	int len = http_request::memfind(data + start_idx, size - start_idx, boundary_info);
	if (len < 0)
	{
		return -1;
	}
	
	len -= static_cast<int>(strlen(CRLF "--"));

	return (http_request::get_binary_stream(data + start_idx, len, request_info));
}

int http_request::get_binary_stream(const char *data, const size_t &size, http_request_data &request_info)
{
	request_info.upload_data_ptr_ = new char[size];
	if (NULL == request_info.upload_data_ptr_)
	{
		return -1;
	}

	memcpy(request_info.upload_data_ptr_, data, size);
	request_info.upload_data_size_ = size;

	return (static_cast<int>(request_info.upload_data_size_));
}

int http_request::get_form_multipart(const char *data, const size_t &size, const char *boundary_info, http_request_data &request_info)
{
	int chunk_count = 0;
	int idx, len, offset = 0;
	int boundary_size = (int)strlen(boundary_info);
	const char *key_start = "Content-Disposition: form-data; name=\"";
	const char *key_end = "\"";

	while ((idx = http_request::memfind(data + offset, size - offset, boundary_info)) >= 0)
	{
		offset += idx + boundary_size;

		if ((idx = http_request::memfind(data + offset, size - offset, key_start)) < 0)
		{
			break;
		}

		offset += idx + (int)strlen(key_start);

		if ((len = http_request::memfind(data + offset, size - offset, key_end)) < 0)
		{
			break;
		}

		std::string key_text(data + offset, len);

		offset += len + (int)strlen(key_end);

		if ((idx = http_request::memfind(data + offset, size - offset, CRLF CRLF)) < 0)
		{
			break;
		}

		offset += idx + (int)strlen(CRLF CRLF);

		if ((len = http_request::memfind(data + offset, size - offset, boundary_info)) < 0)
		{
			break;
		}

		len -= (int)strlen(CRLF "--");

		std::string value_text(data + offset, len);
		value_text = utf8_to_url(value_text.c_str());

		offset += len;

		if (request_info.referer_url_.find("?") != std::string::npos)
		{
			request_info.referer_url_ += "&";
		}
		else
		{
			request_info.referer_url_ += "?";
		}

		request_info.referer_url_ += key_text + "=" + value_text;

		chunk_count += 1;
	}

	return chunk_count;
}

bool http_request::get_form_urlencode(const char *data, const size_t &size, http_request_data &request_info)
{
	if (size > 0)
	{
		std::string value_text(data, size);

		if (request_info.referer_url_.find("?") != std::string::npos)
		{
			request_info.referer_url_ += "&";
		}
		else
		{
			request_info.referer_url_ += "?";
		}

		request_info.referer_url_ += value_text;

		return true;
	}

	return false;
}

std::string http_request::get_content_type(const char *data, const size_t &size)
{
	std::string content_type = http_request::get_header_item(data, size, "Content-Type");
	size_t idx = content_type.find(";");

	if (idx != std::string::npos)
	{
		return (content_type.substr(0, idx));
	}

	return content_type;
}

bool http_request::request_post_parse(const char *data, const size_t &size, http_request_data &request_info)
{
	request_info.init();
	request_info.request_type_ = POST_;
	size_t header_size = http_request::get_header_length(data, size);
	size_t content_size = strtoul(http_request::get_header_item(data, header_size, "Content-Length").c_str(), NULL, 10);
	if (0 == header_size || header_size + content_size > size)
	{
		return false;
	}
	//size_t package_size = header_size + content_size;
	http_request::get_referer_info(data, header_size, POST_, request_info.referer_url_);
	if (request_info.referer_url_.empty())
	{
		return false;
	}

	std::string content_type = http_request::get_content_type(data, header_size);
	if (content_type.compare("multipart/form-data") == 0)
	{
		std::string boundaryinfo = http_request::get_boundary_string(data, header_size);
		if (boundaryinfo.empty())
		{
			return false;
		}

		if (http_request::get_form_multipart(data + header_size, content_size, boundaryinfo.c_str(), request_info) <= 0)
		{
			return false;
		}
	}
	else if(content_type.compare("application/json") == 0)
	{
		std::string body = http_request::get_body_string(data, header_size);
		if (body.empty())
		{
			return false;
		}

		request_info.upload_data_size_ = content_size;
		request_info.upload_data_ptr_ = new char[content_size];
		memcpy(request_info.upload_data_ptr_, data + header_size, content_size);
		return true;
	}
	else if (content_type.compare("application/x-www-form-urlencoded") == 0)
	{
		if (!http_request::get_form_urlencode(data + header_size, content_size, request_info))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	std::string user_id = http_request::get_header_item(data, header_size, "UserId", true);
	std::string user_token = http_request::get_header_item(data, header_size, "UserToken", true);
	request_info.forwarded_ = http_request::get_header_item(data, header_size, "X-Forwarded-For", true);

	if (!user_id.empty() && !user_token.empty())
	{
		if (request_info.referer_url_.find("?") != std::string::npos)
		{
			request_info.referer_url_ += "&";
		}
		else
		{
			request_info.referer_url_ += "?";
		}

		request_info.referer_url_ += "UserId=" + user_id + "&UserToken=" + user_token;
	}

	return true;
}

std::string http_request::get_header_item(const char *data, const size_t &size, const char *key, const bool &ignore_case)
{
	if (NULL == data || NULL == key)
	{
		return "";
	}

	std::string item(key);
	item = CRLF + item + ": ";

	int start_idx = 0;
	if (!ignore_case)
	{
		start_idx = http_request::memfind(data, size, item.c_str());
	}
	else
	{
		start_idx = http_request::memifind(data, size, item.c_str());
	}
	
	if (start_idx <= 0)
	{
		return "";
	}

	start_idx += static_cast<int>(item.length());

	int len = http_request::memfind(data + start_idx, size - start_idx, CRLF);
	if (len <= 0)
	{
		return "";
	}

	return (std::string(data + start_idx, len));
}

bool http_request::request_get_parse(const char *data, const size_t &size, http_request_data &request_info)
{
	request_info.init();
	request_info.request_type_ = GET_;
	size_t header_size = http_request::get_header_length(data, size);
	if (0 == header_size || header_size > size)
	{
		return false;
	}

	http_request::get_referer_info(data, header_size, GET_, request_info.referer_url_);
	if (request_info.referer_url_.empty())
	{
		return false;
	}

	//http_request::get_connection_info(data, header_size, GET_, request_info.connection_);

	std::string user_id = http_request::get_header_item(data, header_size, "UserId", true);
	std::string user_token = http_request::get_header_item(data, header_size, "UserToken", true);
	request_info.forwarded_ = http_request::get_header_item(data, header_size, "X-Forwarded-For", true);

	if (!user_id.empty() && !user_token.empty())
	{
		if (request_info.referer_url_.find("?") != std::string::npos)
		{
			request_info.referer_url_ += "&";
		}
		else
		{
			request_info.referer_url_ += "?";
		}

		request_info.referer_url_ += "UserId=" + user_id + "&UserToken=" + user_token;
	}

	return true;
}

REQUEST_TYPE http_request::get_request_type(const char *data, const size_t &size)
{
	if (size > 10)
	{
		if (0 == memcmp(data, "GET ", strlen("GET ")))
		{
			return GET_;
		}
		else if (0 == memcmp(data, "POST ", strlen("POST ")))
		{
			return POST_;
		}
	}

	return ERROR_;
}

int http_request::memfind(const char *src, const size_t &size_src, const char *dst, const size_t &size_dst)
{
	int idx = 0;
	size_t size = (size_dst > 0) ? size_dst : strlen(dst);

	while (size_src - idx >= size)
	{
		if (0 == memcmp(src + idx++, dst, size))
		{
			return idx - 1;
		}
	}

	return -1;
}

int my_memicmp(const void *buffer1,const void *buffer2,int count)
{
	int f = 0;
	int l = 0;
	while (count--)
	{
		if ( (*(unsigned char *)buffer1 == *(unsigned char *)buffer2) ||
				((f = toupper( *(unsigned char *)buffer1 )) == (l = toupper( *(unsigned char *)buffer2 ))) )
		{
			buffer1 = (char *)buffer1 + 1;
			buffer2 = (char *)buffer2 + 1;
		}
		else
			break;
	}
	return ( f - l );
}

int http_request::memifind(const char *src, const size_t &size_src, const char *dst, const size_t &size_dst)
{
	int idx = 0;
	size_t size = (size_dst > 0) ? size_dst : strlen(dst);

	while (size_src - idx >= size)
	{
		if (0 == my_memicmp(src + idx++, dst, size))
		{
			return idx - 1;
		}
	}

	return -1;
}

http_response_data::http_response_data()
{
	this->data_ptr_ = NULL;
	this->size_ = 0;
}

void http_response_data::init()
{
	if (this->data_ptr_ != NULL)
	{
		delete [] this->data_ptr_;
		this->data_ptr_ = NULL;
	}

	this->size_ = 0;
}

http_response_data::~http_response_data()
{
	if (this->data_ptr_ != NULL)
	{
		delete [] this->data_ptr_;
		this->data_ptr_ = NULL;
	}
}


}
}
