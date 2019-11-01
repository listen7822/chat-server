#include "pch.h"
#include "ChatSession.h"


ChatSession::ChatSession (Server * pServer, int sessionId, boost::asio::io_service & io_service)
	: m_location(LOCATION::NONE),
	Session(pServer, sessionId, io_service)
{
}

ChatSession::~ChatSession ()
{
}
