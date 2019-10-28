#include "pch.h"
#include "Handler.h"
#include "ServerSession.h"
#include "Define.h"
#include <string>

bool Handler::WrongCommand (Session * session, const char * pContext)
{
	char sendMsg[MAX_MESSAGE_LEN] = "You sent wrong command.\r\n";
	session->PostSend (false, std::strlen (sendMsg), sendMsg);

	return true;
}
