#include "pch.h"
#include "LogonHandler.h"
#include "LogonServer.h"
#include "LogonODBCObject.h"

bool __stdcall LogonHandler::WrongCommand (boost::shared_ptr <Session> pSession, std::string param)
{
	std::string message = ">You sent wrong command.\r\n";
	pSession->Send (false, message.length(), message.c_str());

	return true;
}

bool __stdcall LogonHandler::ChangeNickName (boost::shared_ptr <Session> pSession, std::string param)
{
	std::string msg;
	if (false == ((LogonODBCObject*)LogonODBCObject::Instance ())->ChangeUserNickname (pSession->GetNickname (), param))
	{
		msg.append (">");
		msg.append (param);
		msg.append (" is already used by someone.\r\n");
		pSession->Send (false, msg.length (), msg.c_str ());
		return false;
	}

	pSession->SetNickname (param);
	msg.append (">Hello ");
	msg.append (pSession->GetNickname ());
	msg.append ("\r\n");
	pSession->Send (false, msg.length (), msg.c_str ());
	return false;
}
