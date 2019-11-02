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
	Room ();
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
	static const int MAX_USER_COUNT = 5;
	static const int MAX_MESSAGE_QUEUE_SIZE = 5;
	static const int TimerCycleSec = 60;
	void OnTimer ();

	std::list<boost::shared_ptr<Session>> m_UserList;
	std::queue<std::string> m_MessageQueue;
	std::string m_RoomName;
};

