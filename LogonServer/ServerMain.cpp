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


	std:: cout << "��Ʈ�� ���� ����" << std::endl;

	getchar();
	return 0;
}
