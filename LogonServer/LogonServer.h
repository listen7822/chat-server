#pragma once 

#include <Server.h>

class LogonServer : public Server
{
public:
	LogonServer (int port);
	virtual ~LogonServer ();

	void Init (std::size_t maxSessionCount, std::size_t maxThreadCount, std::size_t maxRoomCount) override;

private:

protected:
	void OnAccept (int sessionId) override;
};



