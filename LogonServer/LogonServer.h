#pragma once 

#include <Server.h>

class LogonServer : public Server
{
public:
	LogonServer (int threadPoolSize, int port);
	virtual ~LogonServer ();

	void Init (int nMaxSessionCount) override;

private:

protected:
	void OnAccept (int sessionId) override;
};



