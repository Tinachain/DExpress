#pragma once

#include <map>
#include <list>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>

namespace ustd
{
	namespace video_server
	{
		enum _server_type_{st_gate, st_chat, st_media, st_web, st_interface};
		enum _message_type_{user_message, web_message, center_message, balance_message, transit_message, self_message, none_message};
	};
};

enum error_code
{
	EC_OK						= 0,
	EC_REDIS_DISCONNECTED,
	EC_REDIS_NOT_FOUND,
	EC_REDIS_ERROR,
};
