#pragma once 

#include <Server.h>
#include <Singleton.h>
#include "LogonPacketDispatcher.h"

class LogonServer : public Server
{
public:
	LogonServer (int threadPoolSize, int port);
	virtual ~LogonServer ();

	void Init (const int nMaxSessionCount) override;

private:

protected:
	void OnAccept (int sessionId) override;
};



