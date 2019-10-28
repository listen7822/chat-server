#pragma once
//#include <boost/thread.hpp>
#include "pch.h"
#include <iostream>
#include <boost/asio.hpp>
#include "MySqlDatabase.h"
#include "MySqlObject.h"
#include "LogonServer.h"


const int MAX_SESSION_COUNT = 100;

int main()
{
	boost::asio::io_service io_service;
    
	LogonServer server(io_service);
	server.Init(MAX_SESSION_COUNT);
	server.Start();
	io_service.run();

	//MySqlObject DBObject;
	//DBObject.AddDatabase<MySqlDatabase> ();
	//DBObject.GetUserToken ();
	std:: cout << "네트웍 접속 종료" << std::endl;

	getchar();
	return 0;
}
