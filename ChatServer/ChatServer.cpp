#include "pch.h"
#include "ChatServer.h"
#include "ChatODBCObject.h"
#include "ChatSession.h"
#include "LobbyPacketDispatcher.h"
#include "RoomPacketDispatcher.h"
#include <MySqlDatabase.h>


ChatServer::ChatServer (int threadPoolSize, int port)
	: Server(threadPoolSize, port)
{
}

ChatServer::~ChatServer ()
{
}

void ChatServer::Init (int nMaxSessionCount)
{
	ChatODBCObject::Instance ()->AddDatabase<MySqlDatabase>();
	
	for (int i = 0; i < 3; ++i)
	{
		boost::shared_ptr<Session> pSession(new ChatSession (this, i, io_service));
		m_SessionList.push_back (pSession);
		m_SessionQueue.push_back (i);
	}

	for (int i = 0; i < 1; ++i) {
		boost::shared_ptr<boost::thread> thread (
			new boost::thread (boost::bind (&boost::asio::io_service::run, &io_service))
		);
		m_ThreadPool.push_back (thread);
	}

	boost::shared_ptr<Room> pRoom (new Room());
	m_RoomList.push_back (pRoom);
}

bool ChatServer::JoinRoom (boost::shared_ptr<Session> pSession, boost::shared_ptr<Room> pRoom, std::string param, std::string& error)
{
	if (param.empty ()) {
		error.append (">You must send a room title.");
		error.append ("\r\n");
		return false;
	}

	auto iter = m_RoomList.begin ();
	for (iter; iter != m_RoomList.end (); ++iter) {
		if (param == iter->get ()->GetRoomName ()) {
			break;
		}
	}

	if (iter == m_RoomList.end ()) {
		iter = m_RoomList.begin ();
		for (iter; iter != m_RoomList.end (); ++iter) {
			if (true == iter->get ()->IsEmpty ()) {
				break;
			}
		}

		iter->get()->SetRoomName (param);
		iter->get()->AddUser (pSession);
	} else {
		static const std::size_t MAX_USER_COUNT = 2;
		if (MAX_USER_COUNT <= iter->get()->GetUserList ().size ()) {
			error.append (">Room is full.");
			error.append ("\r\n");
			return false;
		} else {
			iter->get ()-> AddUser (pSession);
		}
	}

	std::string msg;
	msg.append (">Welcome to ");
	msg.append (iter->get ()->GetRoomName());
	msg.append ("Room, there are ");
	msg.append (std::to_string(iter->get()->GetUserList().size()));
	msg.append (" user connected.");
	msg.append ("\r\n");
	std::queue<std::string> tmpQueue (iter->get ()->GetMessageQueue ());
	while (!tmpQueue.empty ()) {
		msg.append(tmpQueue.front ());
		tmpQueue.pop ();
	}
	msg.append ("\r\n");

	boost::dynamic_pointer_cast<ChatSession>(pSession)->SetRoom (*iter);

	pSession->Send (false, msg.length (), msg.c_str());

	return true;
}

void ChatServer::OnAccept (int sessionId)
{
	m_SessionList[sessionId]->SetDispatcher (new LobbyPacketDispatcher ());

	std::string msg;
	msg.append (">Provide Auth Token.");
	msg.append ("\r\n");
	m_SessionList[sessionId]->Send (false, msg.length(), msg.c_str());
}
