#include "stdafx.h"
#include "Handler.h"
#include "Session.h"
#include <string>

bool Handler::WrongCommand (Session * session, const char * pContext)
{
	std::string message = "You sent wrong command.\r\n";
	session->Send (false, message.length(), message.c_str());

	return true;
}
