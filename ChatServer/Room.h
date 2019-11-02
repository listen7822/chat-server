#pragma once

#include <list>
#include <queue>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class Session;

class Room 
{
public:
	Room (boost::asio::io_service& io_service);
	virtual ~Room ();

	void AddUser (boost::shared_ptr<Session> pSession);
	void RemoveUser (std::string nickname);
	void SaveMessage (std::string nickname, std::string message);
	void SetRoomName (std::string roomName) { m_RoomName = roomName; }
	bool IsEmpty () { return m_UserList.size () > 0 ? false : true; }
	void Clear ();
	std::string GetRoomName () { return m_RoomName; }
	std::list<boost::shared_ptr<Session>> GetUserList () { return m_UserList; }
	std::queue<std::string> GetMessageQueue () { return m_MessageQueue; }

private:
	void OnTimer ();

	boost::asio::deadline_timer m_Timer;
	std::list<boost::shared_ptr<Session>> m_UserList;
	std::queue<std::string> m_MessageQueue;
	std::string m_RoomName;
};

