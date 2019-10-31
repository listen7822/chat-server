#pragma once

#include <PacketDispatcher.h>

class LogonPacketDispatcher :
	public PacketDispatcher
{
public:
	LogonPacketDispatcher ();
	virtual ~LogonPacketDispatcher ();

private:
protected:
	void GetCommandTypeAndParam (COMMAND_TYPE& commandType, std::string& param, const char* pData) override;
};

