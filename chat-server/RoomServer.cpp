#include "pch.h"
#include "RoomServer.h"
#include "Define.h"
#include <string>

RoomServer::RoomServer (boost::asio::io_service & io_service) : Server(io_service)
{
}

RoomServer::~RoomServer ()
{
}

void RoomServer::handle_accept (Session * pSession, const boost::system::error_code & error)
{
	if (!error)
	{
		std::cout << "·ë Á¢¼Ó ¼º°ø. SessionID: "
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
