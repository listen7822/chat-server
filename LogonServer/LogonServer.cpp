#include "pch.h"
#include "LogonServer.h"

LogonServer::LogonServer (int threadPoolSize, int port)
	: Server(threadPoolSize, port)
{
}

LogonServer::~LogonServer ()
{
}

void LogonServer::Init (const int nMaxSessionCount)
{
	m_pPacketDispatcher.reset(new LogonPacketDispatcher ());
	Server::Init (nMaxSessionCount);
}

void LogonServer::OnAccept (int sessionId)
{
	m_SessionList[sessionId]->SetNickname ("Spiderman");
	std::cout << "로그인 접속 성공. SessionID: "
		<< std::to_string(sessionId)
		<< " Nickname: "
		<< m_SessionList[sessionId]->GetNickname()
		<< std::endl;

	std::string msg;
	msg.append ("Hello ");
	msg.append (m_SessionList[sessionId]->GetNickname ());
	msg.append ("\r\n");
	m_SessionList[sessionId]->Send (false, msg.length(), msg.c_str());
}
