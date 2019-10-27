#pragma once 

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <string>
#include <vector>

#include "ServerSession.h"
#include "Protocol.h"
#include "Server.h"

class LogonServer : public Server
{
public:
	LogonServer (boost::asio::io_service & io_service);
	~LogonServer ();

	void ProcessPacket (const int nSessionID, const char*pData) override;

private:
	void handle_accept (Session* pSession, const boost::system::error_code& error) override;
	
};



