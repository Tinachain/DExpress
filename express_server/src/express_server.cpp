//============================================================================
// Name        : express_server.cpp
// Author      : fxh7622
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <cstring>
#include <algorithm>
#include "../business/main_thread.h"

using namespace std;
const std::string curVersion = "express_server version: 1.4.0";
const std::string noticeString = "This Is Transmission Server Can Echo ...";

int main()
{
	cout << curVersion << endl;
	cout << noticeString << endl;
	main_thread::get_instance()->init();
	while(1)
	{
		::usleep(1000);
	}
	cout << "!!!Hello World!!!" << endl;
	return 0;
}
