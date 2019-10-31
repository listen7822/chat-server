#pragma once
#include "pch.h"
#include <boost/asio.hpp>
#include "LogonServer.h"


const int MAX_SESSION_COUNT = 1;

int main()
{
	LogonServer server(MAX_SESSION_COUNT, 31400);
	server.Init(MAX_SESSION_COUNT);
	server.Start();


	std:: cout << "네트웍 접속 종료" << std::endl;

	getchar();
	return 0;
}
