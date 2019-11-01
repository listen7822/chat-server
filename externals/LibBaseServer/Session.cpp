#include "stdafx.h"
#include "Session.h"
#include "Server.h"
#include "PacketDispatcher.h"

Session::Session(Server* pServer, int sessionId, boost::asio::io_service& io_service)
		: m_pServer(pServer)
		, m_SessionId(sessionId)
		, m_Socket(io_service)
		, m_Nickname("")
		, m_Strand(io_service)
{
}

Session::~Session()
{
}

void Session::Init(std::string nickname)
{
}

void Session::Receive ()
{
	boost::asio::async_read_until (m_Socket, m_Buffer, "\r\n",
		m_Strand.wrap(
			boost::bind (&Session::OnReceive, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
		)
	);
}

void Session::Send (const bool immediately, int size, const char * pData)
{
	char* pSendData = nullptr;

	if(false == immediately)
	{
		static const int MAX_MESSAGE_SIZE = 128;
		pSendData = new char[MAX_MESSAGE_SIZE];
		memcpy( pSendData, pData, size + 1);

		m_SendDataQueue.push_back( pSendData );
	}
	else
	{
		memcpy( pSendData, pData, size);
	}

	if (false == immediately && m_SendDataQueue.size() > 1)
	{
		return;
	}

	boost::asio::async_write( m_Socket, boost::asio::buffer( pSendData, size),
		m_Strand.wrap (
			boost::bind( &Session::OnWrite, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred )
		)
	);
}


void Session::OnReceive (const boost::system::error_code & error, std::size_t bytes_transferred)
{
	if (error)
	{
		if( error == boost::asio::error::eof )
		{
			std::cout << "클라이언트와 연결이 끊어졌습니다" << std::endl;
		}
		else 
		{
			std::cout << "error No: " << error.value() << " error Message: " << error.message() << std::endl;
		}

		m_pServer->MoveSessionToQueue(m_SessionId);
		return;
	}

	std::string tmp (boost::asio::buffers_begin (m_Buffer.data()),
		boost::asio::buffers_begin (m_Buffer.data()) + bytes_transferred);
	m_pDispatcher->DoDispatch(shared_from_this(), tmp.c_str());	
	m_Buffer.consume (bytes_transferred);
	Receive ();
}

void Session::OnWrite (const boost::system::error_code & error, std::size_t bytes_transferred)
{
	delete[] m_SendDataQueue.front();
	m_SendDataQueue.pop_front();

	if( m_SendDataQueue.empty() == false )
	{
		char* pData = m_SendDataQueue.front();
		
		Send( true, std::strlen(pData), pData );
	}
}
