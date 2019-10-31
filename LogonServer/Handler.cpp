#include "pch.h"
#include "Handler.h"
#include "LogonServer.h"
#include "MySqlObject.h"

bool __stdcall Handler::WrongCommand (Session * pSession, std::string command)
{
	std::string message = "You sent wrong command.\r\n";
	pSession->Send (false, message.length(), message.c_str());

	return true;
}

bool __stdcall Handler::QuitServer (Session * pSession, std::string param)
{
	pSession->QuitServer ();
	return false;
}

bool __stdcall Handler::ChangeNickName (Session * pSession, std::string param)
{
	std::string nickname;
	((MySqlObject*)MySqlObject::Instance ())->ChangeUserNickname (param, nickname);
	pSession->SetNickname (nickname);
	return false;
}
