#include "stdafx.h"
#include "Server.h"

Server::Server (int port)
	: m_IsAccepting (false),
	m_Acceptor (io_service,
		boost::asio::ip::tcp::endpoint (boost::asio::ip::tcp::v4 (), port),
		true)
{ 
}

Server::~Server ()
{
}

void Server::Start ()
{
	Accept ();
	BOOST_LOG_TRIVIAL (info) << "Start thread pool.";
	for (std::size_t i = 0; i < m_ThreadPool.size(); ++i) {
		m_ThreadPool[i]->join ();
	}
}

void Server::MoveSessionToQueue (int sessionID)
{
	BOOST_LOG_TRIVIAL (info) << "Return to client connectin pool. SessionId: " \
		<< sessionID \
		<< " Nickname: " \
		<< m_SessionList[sessionID]->GetNickname ();
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
		BOOST_LOG_TRIVIAL (error) \
			<< "Func: " \
			<< __FUNCTION__ \
			<< " Line: " \
			<< __LINE__ \
			<< " ErrorCode: " \
			<< error;
	}

	OnAccept (sessionId);
	m_SessionList[sessionId]->Receive ();
	Accept ();
}
