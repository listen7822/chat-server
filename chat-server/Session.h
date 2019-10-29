#pragma once

#include <deque>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "Monitor.h"

class Server;

class Session : Monitor
{
public:
	Session(int nSessionID, boost::asio::io_context& io_context, Server* pServer);
	~Session();

	int SessionID() { return m_nSessionID; }

	boost::asio::ip::tcp::socket& Socket() { return m_Socket; }

	void Init (std::string name);

	void PostReceive();
	
	void PostSend( const bool bImmediately, const int nSize, char* pData );
	bool CloseSocket ();

	const char* GetName() { return m_Name.c_str(); }
	void SetName (std::string name) { m_Name = name; }

private:
	void handle_write(const boost::system::error_code& error, size_t bytes_transferred);	
	void handle_receive( const boost::system::error_code& error, size_t bytes_transferred);
	
	int m_nSessionID;
	boost::asio::ip::tcp::socket m_Socket;
	std::deque< char* > m_SendDataQueue;
	std::string m_Name;
	boost::asio::streambuf m_buffer;
	Server* m_pServer;
	Monitor m_csSession;
};