#include "pch.h"
#include "ChatHandler.h"
#include "ChatSession.h"
#include "ChatServer.h"
#include "RoomPacketDispatcher.h"
#include "LobbyPacketDispatcher.h"
#include "ChatODBCObject.h"

bool __stdcall ChatHandler::WrongCommand (boost::shared_ptr <Session> pSession, std::string param)
{
	std::string message = "You sent wrong command.\r\n";
	pSession->Send (false, message.length(), message.c_str());

	return true;
}

bool __stdcall ChatHandler::AuthToken (boost::shared_ptr <Session> pSession, std::string param)
{
	boost::shared_ptr<ChatSession> pChatSession(
		boost::dynamic_pointer_cast<ChatSession>(pSession));
	ChatServer* pChatServer = ((ChatServer*)pChatSession->GetServer ());

	std::string nickname;
	((ChatODBCObject*)ChatODBCObject::Instance ())->GetUserNameUsingToken (nickname, param);
	if (nickname.empty()) {
		pChatSession->SetLocation (ChatSession::NONE);
		std::string msg;
		msg.append ("Fail to auth token. Please try again.");
		msg.append ("\r\n");
		pSession->Send (false, msg.length(), msg.c_str());
		return false;
	}

	pChatSession->SetLocation (ChatSession::LOBBY);
	pChatSession->SetNickname (nickname);
	std::string msg;
	msg.append ("Hello ");
	msg.append (pSession->GetNickname ());
	msg.append (".\r\n");
	pSession->Send (false, msg.length(), msg.c_str());
	return true;
}

bool __stdcall ChatHandler::GetRoomList (boost::shared_ptr <Session> pSession, std::string param)
{
	boost::shared_ptr<ChatSession> pChatSession(
		boost::dynamic_pointer_cast<ChatSession>(pSession));
	ChatServer* pChatServer = ((ChatServer*)pChatSession->GetServer ());
	if (ChatSession::LOCATION::LOBBY != pChatSession->GetLocation ()) {
		return false;
	}
	std::list<boost::shared_ptr<Room>> roomList = pChatServer->GetRoomList ();
	std::string msg;
	msg.append (">");
	msg.append ("You can join these rooms.");
	msg.append ("\r\n");
	bool isEmpty = true;
	auto iter = roomList.begin ();
	for (iter; iter != roomList.end (); ++iter) {
		if (false == iter->get ()->GetRoomName ().empty ()) {
			isEmpty = false;
			msg.append (">");
			msg.append (iter->get ()->GetRoomName ());
			msg.append ("\r\n");
			continue;
		}
	}

	if (true == isEmpty) {
		msg.append (">No rooms.");
		msg.append ("\r\n");
	}

	pChatSession->Send (false, msg.length (), msg.c_str ());
	return true;
}

bool __stdcall ChatHandler::JoinRoomList (boost::shared_ptr <Session> pSession, std::string param)
{
	boost::shared_ptr<ChatSession> pChatSession(
		boost::dynamic_pointer_cast<ChatSession>(pSession));
	ChatServer* pChatServer = ((ChatServer*)pChatSession->GetServer ());
	if (ChatSession::LOCATION::LOBBY != pChatSession->GetLocation ()) {
		return false;
	}
	std::string error;
	boost::shared_ptr<Room> pRoom;
	if (false == pChatServer->JoinRoom (pSession, pRoom, param, error)) {
		pChatSession->Send (false, error.length (), error.c_str());
		return false;
	}

	pChatSession->SetLocation (ChatSession::LOCATION::ROOM);
	pChatSession->SetDispatcher (new RoomPacketDispatcher);
	return true;
}

bool __stdcall ChatHandler::ExitRoom (boost::shared_ptr <Session> pSession, std::string param)
{
	boost::shared_ptr<ChatSession> pChatSession(
		boost::dynamic_pointer_cast<ChatSession>(pSession));
	ChatServer* pChatServer = ((ChatServer*)pChatSession->GetServer ());
	boost::shared_ptr<Room> pRoom = pChatSession->GetRoom ();
	if (ChatSession::LOCATION::ROOM != pChatSession->GetLocation ()) {
		return false;
	}

	pChatSession->SetLocation (ChatSession::LOCATION::LOBBY);
	std::string msg;
	if (1 == pRoom->GetUserList ().size ()) {
		pRoom->Clear ();
	} else {
		pRoom->RemoveUser (pChatSession->GetNickname());
	}

	pChatSession->SetDispatcher (new LobbyPacketDispatcher);
	return true;
}

bool __stdcall ChatHandler::BroadcastMessage (boost::shared_ptr <Session> pSession, std::string param)
{
	boost::shared_ptr<ChatSession> pChatSession (
		boost::dynamic_pointer_cast<ChatSession>(pSession));
	ChatServer* pChatServer = ((ChatServer*)pChatSession->GetServer ());
	if (ChatSession::LOCATION::ROOM != pChatSession->GetLocation ()) {
		return false;
	}

	//if(pChatSession)

	std::string msg;
	msg.append (">");
	msg.append (pChatSession->GetNickname ());
	msg.append ("'s global message: ");
	msg.append (param);

	std::list<boost::shared_ptr<Room>> roomList = pChatServer->GetRoomList ();
	auto roomIter = roomList.begin ();
	for (roomIter; roomIter != roomList.end (); ++roomIter) {
		roomIter->get ()->SaveMessage(pChatSession->GetNickname(), msg);
	}

	return true;
}

bool __stdcall ChatHandler::SendMessage (boost::shared_ptr <Session> pSession, std::string param)
{
	boost::shared_ptr<ChatSession> pChatSession (
		boost::dynamic_pointer_cast<ChatSession>(pSession));
	ChatServer* pChatServer = ((ChatServer*)pChatSession->GetServer ());
	if (ChatSession::LOCATION::ROOM != pChatSession->GetLocation ()) {
		return false;
	}

	boost::shared_ptr<Room> pRoom = pChatSession->GetRoom ();
	std::string msg;
	msg.append (">");
	msg.append (pChatSession->GetNickname());
	msg.append (": ");
	msg.append (param);
	pRoom->SaveMessage (pChatSession->GetNickname(), msg);
	pChatSession->AddChatCount ();

	return true;
}
