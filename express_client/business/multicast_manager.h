/*
 * multicast_manager.h
 *
 *  Created on: 2019年1月2日
 *      Author: fxh7622
 */

#ifndef MULTICAST_MANAGER_H_
#define MULTICAST_MANAGER_H_

#include <stdint.h>
#include "multicast_client.h"

class multicast_manager
{
public:
	multicast_manager(const std::string &group_ip, const int &group_port);
	~multicast_manager(void);

public:
	std::string group_ip_ = "";
	int group_port_ = 9999;
	bool init();

public:
	multicast_client *multicast_client_ptr_ = nullptr;
	void multicast_on_recv(char *data, const int &size);
	void multicast_on_disconnected();
	void multicast_on_error(const int &error);

public:
	ustd::log::write_log* log_ptr_ = nullptr;
	void add_log(const int log_type, const char *context, ...);

};

#endif /* MULTICAST_MANAGER_H_ */
