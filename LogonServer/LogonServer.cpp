#include "pch.h"
#include "LogonServer.h"
#include "MySqlObject.h"
#include <MySqlDatabase.h>

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
	MySqlObject::Instance ()->AddDatabase<MySqlDatabase>();
	Server::Init (nMaxSessionCount);
}

void LogonServer::OnAccept (int sessionId)
{
	std::string nickname;
	std::string token;
	((MySqlObject*)MySqlObject::Instance ())->CreateUser (nickname, token);
	m_SessionList[sessionId]->SetNickname (nickname);
	std::cout << "로그인 접속 성공. SessionID: "
		<< std::to_string(sessionId)
		<< " Nickname: "
		<< m_SessionList[sessionId]->GetNickname()
		<< std::endl;

	std::string msg;
	msg.append ("Hello ");
	msg.append (m_SessionList[sessionId]->GetNickname ());
	msg.append ("\r\n");
	msg.append ("Token: ");
	msg.append (token);
	msg.append ("\r\n");
	m_SessionList[sessionId]->Send (false, msg.length(), msg.c_str());
}
