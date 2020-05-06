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
const std::string curVersion = "Dexpress Server version: 2.0.1";
const std::string noticeString = "This Is Dexpress Server Can Echo ...";

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
