#include "pch.h"
#include "Session.h"
#include "LogonServer.h"
#include "Define.h"
#include "PacketDispatcher.h"


Session::Session(int nSessionID, boost::asio::io_context& io_context, Server* pServer)
		: m_Socket(io_context)
		, m_nSessionID( nSessionID )
		, m_pServer( pServer )
		, m_buffer(MAX_MESSAGE_LEN)
{
}

Session::~Session()
{
	while( m_SendDataQueue.empty() == false )
	{
		delete[] m_SendDataQueue.front();
		m_SendDataQueue.pop_front();
	}
}

void Session::Init(std::string name)
{
	m_Name = name;
}

void Session::PostReceive()
{
	m_buffer.consume (MAX_MESSAGE_LEN);
	boost::asio::async_read_until (m_Socket, m_buffer, "\r\n",
		boost::bind (&Session::handle_receive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);
}

void Session::PostSend( const bool bImmediately, const int nSize, char* pData )
{
	char* pSendData = nullptr;

	if( bImmediately == false )
	{
		pSendData = new char[nSize];
		memcpy( pSendData, pData, nSize);

		m_SendDataQueue.push_back( pSendData );
	}
	else
	{
		pSendData = pData;
	}

	if (bImmediately == false && m_SendDataQueue.size() > 1)
	{
		return;
	}

	boost::asio::async_write( m_Socket, boost::asio::buffer( pSendData, nSize ),
							 boost::bind( &Session::handle_write, this,
								boost::asio::placeholders::error,
								boost::asio::placeholders::bytes_transferred )
							);
}

bool Session::CloseSocket ()
{
	Monitor::Owner lock (m_csSession);
	{
		if (m_Socket.is_open ()) {
			m_Socket.close ();
			return true;
		}
	}

	return false;
}

void Session::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
	Monitor::Owner lock (m_csSession);
	{
		delete[] m_SendDataQueue.front();
		m_SendDataQueue.pop_front();

		if( m_SendDataQueue.empty() == false )
		{
			char* pData = m_SendDataQueue.front();
			
			PostSend( true, std::strlen(pData), pData );
		}
	}
}

void Session::handle_receive( const boost::system::error_code& error, size_t bytes_transferred)
{
	Monitor::Owner lock (m_csSession);
	{
		if( error )
		{
			if( error == boost::asio::error::eof )
			{
				std::cout << "클라이언트와 연결이 끊어졌습니다" << std::endl;
			}
			else 
			{
				std::cout << "error No: " << error.value() << " error Message: " << error.message() << std::endl;
			}

			m_pServer->MoveSessionToQueue( m_nSessionID );
		}
		else
		{
			m_pServer->GetPacketDispather()->DoDispatch( this,
				boost::asio::buffer_cast<const char*>(m_buffer.data())
			);	
			PostReceive(); 
		}
	}
}

