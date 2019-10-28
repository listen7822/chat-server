#include "pch.h"
#include "Server.h"
#include "Define.h"


Server::Server (boost::asio::io_service & io_service)
	: m_acceptor (io_service, boost::asio::ip::tcp::endpoint (boost::asio::ip::tcp::v4 (), PORT_NUMBER))
{
	m_bIsAccepting = false;
}


Server::~Server ()
{
	for (size_t i = 0; i < m_SessionList.size (); ++i)
	{
		if (m_SessionList[i]->Socket ().is_open ())
		{
			m_SessionList[i]->Socket ().close ();
		}

		delete m_SessionList[i];
	}
}

void Server::Init (const int nMaxSessionCount)
{
	for (int i = 0; i < nMaxSessionCount; ++i)
	{
		Session* pSession = new Session (i, (boost::asio::io_context&)(m_acceptor.get_executor ().context ()), this);
		m_SessionList.push_back (pSession);
		m_SessionQueue.push_back (i);
	}
}

void Server::Start ()
{
	std::cout << "서버 시작....." << std::endl;

	PostAccept ();
}

void Server::CloseSession (const int nSessionID)
{
	std::cout << "클라이언트 접속 종료. 세션 ID: " << nSessionID << std::endl;

	m_SessionList[nSessionID]->Socket ().close ();

	m_SessionQueue.push_back (nSessionID);

	if (m_bIsAccepting == false)
	{
		PostAccept ();
	}
}

bool Server::PostAccept ()
{
	if (m_SessionQueue.empty ())
	{
		m_bIsAccepting = false;
		return false;
	}

	m_bIsAccepting = true;
	int nSessionID = m_SessionQueue.front ();

	m_SessionQueue.pop_front ();

	m_acceptor.async_accept (m_SessionList[nSessionID]->Socket (),
			boost::bind (&Server::handle_accept,
			this,
			m_SessionList[nSessionID],
			boost::asio::placeholders::error)
	);

	return true;
}
