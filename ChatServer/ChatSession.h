#pragma once
#include <Session.h>
#include "Room.h"

class ChatSession : public Session
{
public:
	enum LOCATION {
		NONE = 0,
		LOBBY,
		ROOM
	};

	ChatSession (Server* pServer, int sessionId, boost::asio::io_service& io_service);
	virtual ~ChatSession ();

	LOCATION GetLocation () { return m_location; }
	void SetLocation (LOCATION location) { m_location = location; }
	void AddChatCount ();
	void ResetChatCount ();
	int GetChatCount ();
	boost::shared_ptr<Room> GetRoom () { return m_pRoom; }
	void SetRoom (boost::shared_ptr<Room> pRoom) { m_pRoom = pRoom; }

private:
	LOCATION m_location;
	int m_ChatCount;
	boost::shared_ptr<Room> m_pRoom;
};

