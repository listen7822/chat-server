//#include <boost/thread.hpp>
#include "pch.h"
#include <iostream>
#include <mysqlx/xdevapi.h>
//#include "ChattingServer.h"


const int MAX_SESSION_COUNT = 100;

int main()
{
	//boost::asio::io_service io_service;
 //   
	//LogonServer server(io_service);
	//server.Init(MAX_SESSION_COUNT);
	//server.Start();
    
	mysqlx::Session sess ("mysqlx://root@127.0.0.1:13306");

	std::cout << "Session accepted, creating collection..." << std::endl;
	//mysql_init (&conn);
	//mysql_real_connect (&conn, "localhost", "root", "123!@#", "chatserver", 13306, 0, 0);

	//io_service.run();

	//std:: cout << "네트웍 접속 종료" << std::endl;

	//getchar();
	return 0;
}
