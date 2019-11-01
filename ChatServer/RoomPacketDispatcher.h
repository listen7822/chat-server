#pragma once

#include <PacketDispatcher.h>

class RoomPacketDispatcher :
	public PacketDispatcher
{
public:
	RoomPacketDispatcher ();
	virtual ~RoomPacketDispatcher ();
private:

protected:
	void GetCommandTypeAndParam (COMMAND_TYPE& commandType, std::string& param, const char* pData) override;
};

