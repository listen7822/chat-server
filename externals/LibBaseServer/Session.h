#pragma once

#include <deque>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <Monitor.h>
#include "PacketDispatcher.h"

class Server;

class Session : public boost::enable_shared_from_this<Session>,
	public Monitor,
	private boost::noncopyable
{
public:
	Session (Server* pServer, int sessionId, boost::asio::io_service& io_service);
	virtual ~Session ();

	boost::asio::ip::tcp::socket& Socket () { return m_Socket; }
	void Init (std::string nickname);
	void Receive ();
	void Send (const bool immediately, int size, const char* pData);
	const char* GetNickname () { return m_Nickname.c_str (); }
	void SetNickname (std::string nickname) { m_Nickname = nickname; }
	int SessionID () { return m_SessionId; }
	void SetDispatcher (PacketDispatcher* pDispatcher) { m_pDispatcher.reset(pDispatcher); }
	Server* GetServer () { return m_pServer; }

private:
	void OnReceive (const boost::system::error_code& error, std::size_t bytes_transferred);
	void OnWrite (const boost::system::error_code& error, std::size_t bytes_transferred);

	int m_SessionId;
	boost::asio::ip::tcp::socket m_Socket;
	std::deque<char*> m_SendDataQueue;
	std::string m_Nickname;
	boost::asio::streambuf m_Buffer;
	boost::asio::io_service::strand m_Strand;
	boost::shared_ptr<PacketDispatcher> m_pDispatcher;

protected:
	Server* m_pServer;
	boost::shared_ptr<PacketDispatcher> m_pPacketDispatcher;
	Monitor m_csSession;
};