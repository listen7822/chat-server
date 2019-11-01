#include "pch.h"
#include "Room.h"
#include "ChatSession.h"


Room::Room ()
{
}


Room::~Room ()
{
}

void Room::AddUser (boost::shared_ptr<Session> pSession)
{
	m_UserList.push_back (pSession);
}

void Room::RemoveUser (std::string nickname)
{
	std::string msg;
	auto iter = m_UserList.begin ();
	for (iter; iter != m_UserList.end (); ++iter) {
		msg.append (iter->get()->GetNickname ());
		msg.append ("leave this room.");
		msg.append ("\r\n");
		iter->get ()->Send (false, msg.length (), msg.c_str ());
		//if (nickname == iter->get ()->GetNickname ()) {
		//	iter = m_UserList.erase (iter);
		//}
	}
}

void Room::SaveMessage (std::string message)
{
}

void Room::Clear ()
{
	m_UserList.clear ();
	m_RoomName = "";
	while (!m_MessageQueue.empty ()) {
		m_MessageQueue.pop ();
	}
}
