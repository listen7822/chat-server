#pragma once
#include "pch.h"
#include <boost/asio.hpp>
#include "LogonServer.h"
#include "Config.h"


const int MAX_SESSION_COUNT = 1;

int main ()
{
	LogonServer server (LOGON::SERVER_PORT);
	server.Init (LOGON::MAX_SESSION_COUNT, LOGON::MAX_THREAD_POOL_SIZE, 0);
	BOOST_LOG_TRIVIAL (info) << "Logon server start ...";
	server.Start ();

	BOOST_LOG_TRIVIAL (info) << "Closed server.";

	getchar ();
	return 0;
}
