#include "pch.h"
#include "LogonPacketDispatcher.h"


LogonPacketDispatcher::LogonPacketDispatcher ()
{
}


LogonPacketDispatcher::~LogonPacketDispatcher ()
{
}

PacketDispatcher::COMMAND_TYPE LogonPacketDispatcher::GetCommandType (const char * pData)
{
	return PacketDispatcher::GetCommandType (pData);
}
