#pragma once
#include "PacketDispatcher.h"

class LogonPacketDispatcher :
	public PacketDispatcher
{
public:
	LogonPacketDispatcher ();
	virtual ~LogonPacketDispatcher ();
};

