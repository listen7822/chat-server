#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <string>
#include <vector>

#include "ServerSession.h"
#include "Protocol.h"

class Server
{
public:
	Server (boost::asio::io_service & io_service);
	virtual ~Server ();

	void Init (const int nMaxSessionCount);
	void Start ();
	void CloseSession (const int nSessionID);
	virtual void ProcessPacket (const int nSessionID, const char*pData) = 0;

protected:
	bool PostAccept ();
	virtual void handle_accept (Session* pSession, const boost::system::error_code& error) = 0;

	bool m_bIsAccepting;

	boost::asio::ip::tcp::acceptor m_acceptor;

	std::vector< Session* > m_SessionList;
	std::deque< int > m_SessionQueue;
};

