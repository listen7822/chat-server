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
	msg.append ("You can join these rooms.");
	msg.append ("\r\n");
	auto iter = roomList.begin ();
	if (roomList.empty ()) {
		msg.append ("Empty");
		msg.append ("\r\n");
	} else {
		for (iter; iter != roomList.end (); ++iter) {
			msg.append (iter->get()->GetRoomName());
			msg.append ("\r\n");
		}
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
		auto iter = pRoom->GetUserList ().begin ();
		for (iter; iter != pRoom->GetUserList ().end (); ++iter) {
			msg.append (pChatSession->GetNickname ());
			msg.append ("leave this room.");
			msg.append ("\r\n");
			iter->get ()->Send (false, msg.length (), msg.c_str ());
		}
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

	std::list<boost::shared_ptr<Room>> roomList = pChatServer->GetRoomList ();
	auto roomIter = roomList.begin ();
	for (roomIter; roomIter != roomList.end (); ++roomIter) {
		std::list<boost::shared_ptr<Session>> userList = roomIter->get ()->GetUserList ();
		auto userIter = userList.begin ();
		for (userIter; userIter != userList.end (); ++userIter) {
			userIter->get ()->Send (false, param.length (), param.c_str ());
		}
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
	pRoom->SaveMessage (param);
	pChatSession->AddChatCount ();
	auto iter = pRoom->GetUserList ().begin ();
	for (iter; iter != pRoom->GetUserList ().begin (); ++iter) {
		iter->get ()->Send (false, param.length (), param.c_str ());
	}

	return true;
}
