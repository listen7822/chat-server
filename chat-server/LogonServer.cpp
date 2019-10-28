#include "pch.h"
#include "LogonServer.h"
#include "Define.h"
#include "PacketDispatcher.h"

LogonServer::LogonServer (boost::asio::io_service & io_service)
	: Server(io_service)
{
}

LogonServer::~LogonServer ()
{
}

void LogonServer::handle_accept (Session * pSession, const boost::system::error_code & error)
{
	if (!error)
	{
		std::cout << "클라이언트 접속 성공. SessionID: "
			<< pSession->SessionID ()
			<< "Name: "
			<< pSession->GetName()
			<< std::endl;

		pSession->Init ("Spiderman");
		char szWelcomMsg[MAX_MESSAGE_LEN] = { 0 };
		strcpy_s (szWelcomMsg, MAX_MESSAGE_LEN, "Hello ");
		::strcat_s (szWelcomMsg, pSession->GetName ());
		::strcat_s (szWelcomMsg, "\r\n");
		pSession->PostSend (false, strlen(szWelcomMsg), szWelcomMsg);
		pSession->PostReceive ();

		PostAccept ();
	}
	else
	{
		std::cout << "error No: " << error.value () << " error Message: " << error.message () << std::endl;
	}
}

