#pragma once
#include <PacketDispatcher.h>


class LobbyPacketDispatcher : public PacketDispatcher
{
public:
	LobbyPacketDispatcher ();
	virtual ~LobbyPacketDispatcher ();

private:

protected:
	void GetCommandTypeAndParam (COMMAND_TYPE& commandType, std::string& param, const char* pData) override;
};

