#pragma once
#include <string>
#include <vector>
#include <queue>

class Session;

class Room
{
public:
	Room ();
	virtual ~Room ();

private:
	std::string m_RoomName;
	int m_RoomNumber;
	int m_UserCount;
	std::vector<Session* > m_UserList;
	std::queue<std::string> m_HistoryQueue;
};

