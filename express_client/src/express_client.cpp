//============================================================================
// Name        : express_client.cpp
// Author      : fxh7622
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "../business/main_thread.h"
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <cstring>
#include "path.h"

using namespace std;
const std::string curVersion = "Dexpress Client version: 2.0.1";
const std::string noticeString = "This Is Dexpress Client Can Max Pool...";

int main(int argc, char *argv[])
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
