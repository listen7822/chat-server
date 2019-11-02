#pragma once

#include <Server.h>
#include <list>
#include "Room.h"


class ChatServer : public Server
{
public:
	ChatServer (boost::asio::io_service& io_service, int port);
	virtual ~ChatServer ();

	void Init (boost::asio::io_service& io_service, std::size_t maxSessionCount, std::size_t maxThreadCount, std::size_t maxRoomCount) override;
	bool JoinRoom (boost::shared_ptr<Session> pSession, boost::shared_ptr<Room> pRoom, std::string param, std::string& error);
	std::list<boost::shared_ptr<Room>> GetRoomList () { return m_RoomList; }

private:
	std::list<boost::shared_ptr<Room>> m_RoomList;
	

protected:
	void OnAccept (int sessionId) override;
};

