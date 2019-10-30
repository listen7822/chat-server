#include "stdafx.h"
#include "Server.h"


Server::Server (int threadPoolSize, int port)
	: m_IsAccepting(false),
	m_ThreadPoolSize(threadPoolSize),
	m_Acceptor (io_service,
		boost::asio::ip::tcp::endpoint (boost::asio::ip::tcp::v4 (), port),
		true)
{
}


Server::~Server ()
{
}

void Server::Init (const int nMaxSessionCount)
{
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

void Server::Start ()
{
	std::cout << "서버 시작....." << std::endl;
	Accept ();
	for (int i = 0; i < m_ThreadPoolSize; ++i) {
		m_ThreadPool[i]->join ();
	}

}

void Server::MoveSessionToQueue (const int sessionID)
{
	std::cout << "클라이언트 접속 종료. 세션 ID: " << sessionID << std::endl;

	m_SessionList[sessionID]->Socket ().close ();
	m_SessionQueue.push_back (sessionID);

	if (false == m_IsAccepting)
	{
		Accept ();
	}
}

bool Server::Accept ()
{
	if (m_SessionQueue.empty ())
	{
		m_IsAccepting = false;
		return false;
	}

	m_IsAccepting = true;
	int sessionId = m_SessionQueue.front ();

	m_SessionQueue.pop_front ();
	m_Acceptor.async_accept (m_SessionList[sessionId]->Socket (),
			boost::bind (&Server::OnAccept,
			this,
			sessionId,
			boost::asio::placeholders::error)
	);

	return true;
}

void Server::OnAccept (int sessionId, const boost::system::error_code & error)
{
	if (error)
	{
		std::cout << "error No: " << error.value () << " error Message: " << error.message () << std::endl;
	}

	OnAccept (sessionId);
	m_SessionList[sessionId]->Receive ();
	Accept ();
}
