#include "pch.h"
#include "LogonODBCObject.h"
#include <MySqlDatabase.h>
#include "LogonServer.h"
#include "LogonPacketDispatcher.h"


LogonServer::LogonServer (int threadPoolSize, int port)
	: Server(threadPoolSize, port)
{
}

LogonServer::~LogonServer ()
{
}

void LogonServer::Init (const int nMaxSessionCount)
{
	LogonODBCObject::Instance ()->AddDatabase<MySqlDatabase>();

	for (int i = 0; i < nMaxSessionCount; ++i)
	{
		boost::shared_ptr<Session> pSession(new Session (this, i, io_service));
		m_SessionList.push_back (pSession);
		m_SessionQueue.push_back (i);
	}

	for (int i = 0; i < m_ThreadPoolSize; ++i) {
		boost::shared_ptr<boost::thread> thread (
			new boost::thread (boost::bind (&boost::asio::io_service::run, &io_service))
		);
		m_ThreadPool.push_back (thread);
	}
}

void LogonServer::OnAccept (int sessionId)
{
	std::string nickname;
	std::string token;
	((LogonODBCObject*)LogonODBCObject::Instance ())->CreateUser (nickname, token);
	m_SessionList[sessionId]->SetNickname (nickname);
	std::cout << "로그인 접속 성공. SessionID: "
		<< std::to_string(sessionId)
		<< " Nickname: "
		<< m_SessionList[sessionId]->GetNickname()
		<< std::endl;

	m_SessionList[sessionId]->SetDispatcher (new LogonPacketDispatcher ());
	std::string msg;
	msg.append ("Hello ");
	msg.append (m_SessionList[sessionId]->GetNickname ());
	msg.append ("\r\n");
	msg.append ("Token: ");
	msg.append (token);
	msg.append ("\r\n");
	m_SessionList[sessionId]->Send (false, msg.length(), msg.c_str());
}
