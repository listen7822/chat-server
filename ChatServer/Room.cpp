#include "pch.h"
#include "Room.h"
#include "ChatSession.h"


Room::Room ()
{

	//m_Timer.async_wait (boost::bind (&Room::OnTimer, this));
	//m_TimerThread.reset (new boost::thread (boost::bind (&boost::asio::io_service::run, &io_service)));
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
	for (iter; iter != m_UserList.end ();) {
		iter->get ()->Send (false, msg.length (), msg.c_str ());
		if (nickname == iter->get ()->GetNickname ()) {
			m_UserList.erase (iter++);
			continue;
		}
		else {
			msg.append (iter->get()->GetNickname ());
			msg.append ("leave this room.");
			msg.append ("\r\n");
			iter++;
		}
	}
}

void Room::SaveMessage (std::string nickname, std::string message)
{
	if (5 <= m_MessageQueue.size ())
	{
		m_MessageQueue.pop ();
	}

	m_MessageQueue.push (message);
	auto iter = m_UserList.begin ();
	for (iter; iter != m_UserList.end (); ++iter) {
		if (nickname == iter->get ()->GetNickname ()) {
			continue;
		}
		iter->get ()->Send (false, message.length (), message.c_str ());
	}
}

void Room::Clear ()
{
	m_UserList.clear ();
	m_RoomName = "";
	while (!m_MessageQueue.empty ()) {
		m_MessageQueue.pop ();
	}
}

void Room::OnTimer ()
{
	std::cout << "Call Timer." << std::endl;
	//m_Timer.expires_at (m_Timer.expires_at () + boost::posix_time::seconds (10));
	//m_Timer.async_wait (boost::bind (&Room::OnTimer, this));
}
