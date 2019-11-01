#include "pch.h"
#include "LogonHandler.h"
#include "LogonServer.h"
#include "LogonODBCObject.h"

bool __stdcall LogonHandler::WrongCommand (Session * pSession, std::string command)
{
	std::string message = "You sent wrong command.\r\n";
	pSession->Send (false, message.length(), message.c_str());

	return true;
}

bool __stdcall LogonHandler::QuitServer (Session * pSession, std::string param)
{
	return false;
}

bool __stdcall LogonHandler::ChangeNickName (Session * pSession, std::string param)
{
	((LogonODBCObject*)LogonODBCObject::Instance ())->ChangeUserNickname (pSession->GetNickname(), param);
	pSession->SetNickname (param);
	std::string msg;
	msg.append ("Hello ");
	msg.append (pSession->GetNickname ());
	msg.append ("\r\n");
	pSession->Send (false, msg.length (), msg.c_str ());
	return false;
}
