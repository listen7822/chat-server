#include "pch.h"
#include "LogonServer.h"
#include "LogonODBCObject.h"
#include "Session.h"
#include "LogonPacketDispatcher.h"
#include <MySqlDatabase.h>


LogonServer::LogonServer (int port)
	: Server(port)
{
}

LogonServer::~LogonServer ()
{
}

void LogonServer::Init (std::size_t maxSessionCount, std::size_t maxThreadCount, std::size_t maxRoomCount)
{
	LogonODBCObject::Instance ()->AddDatabase<MySqlDatabase>();
	::Sleep (1000);
	for (std::size_t i = 0; i < maxSessionCount; ++i)
	{
		boost::shared_ptr<Session> pSession(new Session (this, i, io_service));
		m_SessionList.push_back (pSession);
		m_SessionQueue.push_back (i);
	}

	for (std::size_t i = 0; i < maxThreadCount; ++i) {
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
	bool ret = ((LogonODBCObject*)LogonODBCObject::Instance ())->CreateUser (nickname, token);
	if (false == ret) {
		std::string msg = "Server interanl error is occured. Please retry to login.\r\n";
		m_SessionList[sessionId]->Send (false, msg.length(), msg.c_str());
		m_SessionList[sessionId]->Socket ().close ();
		return;
	}
	m_SessionList[sessionId]->SetNickname (nickname);
	BOOST_LOG_TRIVIAL (info) << "Success to login." \
		<< " SessionId: " \
		<< std::to_string(sessionId) \
		<< " Nickname: " \
		<< m_SessionList[sessionId]->GetNickname() \
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
