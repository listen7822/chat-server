#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <string>
#include <vector>

#include "Session.h"

class PacketDispatcher;

class Server
{
public:
	Server (boost::asio::io_service & io_service);
	virtual ~Server ();

	void Init (const int nMaxSessionCount);
	void Start ();
	void MoveSessionToQueue (const int nSessionID);
	PacketDispatcher* GetPacketDispather () { return m_pPacketDispatcher; }	

protected:
	bool PostAccept ();
	virtual void handle_accept (Session* pSession, const boost::system::error_code& error) = 0;

	bool m_bIsAccepting;

	boost::asio::ip::tcp::acceptor m_acceptor;

	std::vector<Session*> m_SessionList;
	std::deque<int> m_SessionQueue;

	PacketDispatcher* m_pPacketDispatcher;
};

