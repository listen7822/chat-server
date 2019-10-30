#pragma once

#include <deque>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "Monitor.h"

class PacketDispatcher;

class Session
{
public:
	Session(int nSessionID, boost::asio::io_context& io_context);
	~Session();

	boost::asio::ip::tcp::socket& Socket() { return m_Socket; }
	void Init (std::string nickname);
	void Receive();
	void Send( const bool bImmediately, const int nSize, char* pData );
	const char* GetName() { return m_Nicname.c_str(); }
	void SetName (std::string nickname) { m_Nicname = nickname; }
	int SessionID() { return m_SessionId; }

private:
	void OnReceieve( const boost::system::error_code& error, size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error, size_t bytes_transferred);	
	
	int m_SessionId;
	boost::asio::ip::tcp::socket m_Socket;
	std::deque<char*> m_SendDataQueue;
	std::string m_Nicname;
	boost::asio::streambuf m_Buffer;
	boost::asio::io_service::strand m_Strand;
	boost::shared_ptr<PacketDispatcher> m_pDispatcher;

protected:
	void OnReceive (const char* pData) = 0;
};