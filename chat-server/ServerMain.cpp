#include <boost/thread.hpp>
//#include <mysql.h>

#include "pch.h"
#include "ChattingServer.h"

const int MAX_SESSION_COUNT = 100;

int main()
{
	boost::asio::io_service io_service;
    
	LogonServer server(io_service);
	server.Init(MAX_SESSION_COUNT);
	server.Start();
    
	//MYSQL* conn = nullptr;
	//conn = mysql_init (0);
	//conn = mysql_real_connect (conn, "localhost", "root", "123!@#", "chatserver", 13306, 0, 0);

	//if (nullptr == conn) {
	//	return 0;
	//}
	io_service.run();

	std:: cout << "네트웍 접속 종료" << std::endl;

	getchar();
	return 0;
}
