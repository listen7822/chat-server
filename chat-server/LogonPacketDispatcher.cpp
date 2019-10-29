#include "pch.h"
#include "LogonPacketDispatcher.h"
#include "LogonHandler.h"


LogonPacketDispatcher::LogonPacketDispatcher ()
{
	AddHandler (CAHNGE_NAME, &LogonHandler::ChangeName);
}


LogonPacketDispatcher::~LogonPacketDispatcher ()
{
}

PacketDispatcher::COMMAND_TYPE LogonPacketDispatcher::GetCommandType (const char * pData)
{
	return COMMAND_TYPE ();
}
