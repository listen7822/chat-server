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

void ChatSession::AddChatCount ()
{
	Monitor::Owner lock (m_csSession);
	{
		++m_ChatCount;
	}
}

void ChatSession::ResetChatCount ()
{
	Monitor::Owner lock (m_csSession);
	{
		m_ChatCount = 0;
	}
}

int ChatSession::GetChatCount ()
{
	Monitor::Owner lock (m_csSession);
	{
		return m_ChatCount;
	}
}
