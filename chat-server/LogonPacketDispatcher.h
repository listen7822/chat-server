#pragma once
#include "PacketDispatcher.h"

class LogonPacketDispatcher :
	public PacketDispatcher
{
public:
	LogonPacketDispatcher ();
	virtual ~LogonPacketDispatcher ();

	PacketDispatcher::COMMAND_TYPE GetCommandType (const char* pData) override;
};

