#pragma once
#include "Server.h"

class RoomServer :
	public Server
{
public:
	RoomServer (boost::asio::io_service & io_service);
	virtual ~RoomServer ();

	void handle_accept (Session* pSession, const boost::system::error_code& error) override;

private:
	//std::vector<Room* > m_Room;
};

