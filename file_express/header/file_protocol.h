#ifndef FILE_PROTOCOL_H_
#define FILE_PROTOCOL_H_
#pragma once

#include <string>
#include <functional>

//默认的WEB端口;
const int WEB_PORT					=	9999;
const int WEB_BASE					=	10000;
const int WEB_SEND_FILE			=	WEB_BASE + 1;

//数据消息;
const unsigned short MESSAGE_BASE				= 1000;

//HLS协议
const unsigned short HLS_CONFIG 					= MESSAGE_BASE + 1;		//配置结构
const unsigned short HLS_CONFIG_RESULT 			= MESSAGE_BASE + 2;		//配置结构返回;
const unsigned short HLS_FILE_BUFFER				= MESSAGE_BASE + 3;		//接收到文件数据;
const unsigned short HLS_FILE_BUFFER_ENDED		= MESSAGE_BASE + 4;		//文件接收完毕;

//MULTICAST协议;
const unsigned short MULTICAST_CONFIG 				= MESSAGE_BASE + 10;	//组播配置
const unsigned short MULTICAST_CONFIG_RESULT 	= MESSAGE_BASE + 11;	//组播配置返回
const unsigned short MULTICAST_BUFFER 				= MESSAGE_BASE + 12;	//组播数据

//RUDP协议;
const unsigned short CLOSE_LINKER					= MESSAGE_BASE + 99;	//关闭RUDP链路;

//常用定义
const int FILE_CHECK = 5;
const int FILE_TIMEOUT = 5 * 60;
const unsigned short STATIC_TIME		= 5 ;
const unsigned short SINGLE_BLOCK 		= 32 * 1024;
const unsigned short SHOW_TIME			= 30;
const unsigned short CHECK_TIME			= 2;
const int ERROR_HANDLE = -1;

struct header_protocol
{
	unsigned short protocol_id_;
};

struct file_protocol
{
	int cur_id_;							//当前文件块序号
};

#endif	//FILE_PROTOCOL_H_
