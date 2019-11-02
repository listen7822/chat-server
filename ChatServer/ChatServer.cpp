#include "pch.h"
#include "ChatServer.h"
#include "ChatODBCObject.h"
#include "ChatSession.h"
#include "LobbyPacketDispatcher.h"
#include "RoomPacketDispatcher.h"
#include <MySqlDatabase.h>
#include "Config.h"


ChatServer::ChatServer (int port)
	: Server(port)
{
}

ChatServer::~ChatServer ()
{
}

void ChatServer::Init (std::size_t maxSessionCount, std::size_t maxThreadCount, std::size_t maxRoomCount)
{
	ChatODBCObject::Instance ()->AddDatabase<MySqlDatabase>();
	
	for (std::size_t i = 0; i < maxSessionCount; ++i)
	{
		boost::shared_ptr<Session> pSession(new ChatSession (this, i, io_service));
		m_SessionList.push_back (pSession);
		m_SessionQueue.push_back (i);
	}

	for (std::size_t i = 0; i < maxRoomCount; ++i) {
		boost::shared_ptr<Room> pRoom (new Room(io_service));
		m_RoomList.push_back (pRoom);
	}

	for (std::size_t i = 0; i < maxThreadCount; ++i) {
		boost::shared_ptr<boost::thread> thread (
			new boost::thread (boost::bind (&boost::asio::io_service::run, &io_service))
		);
		m_ThreadPool.push_back (thread);
	}
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

		if (iter != m_RoomList.end ()) {
			iter->get()->SetRoomName (param);
			iter->get()->AddUser (pSession);
		}
		else {
			error.append (">This server can't make a room anymore.");
			error.append ("\r\n");
			return false;
		}
	} else {
		if (ROOM::MAX_USER_COUNT <= iter->get()->GetUserList ().size ()) {
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
