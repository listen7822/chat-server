#pragma once 

#include <Server.h>

class LogonServer : public Server
{
public:
	LogonServer (boost::asio::io_service& io_service, int port);
	virtual ~LogonServer ();

	void Init (boost::asio::io_service& io_service, std::size_t maxSessionCount, std::size_t maxThreadCount, std::size_t maxRoomCount) override;

private:

protected:
	void OnAccept (int sessionId) override;
};



